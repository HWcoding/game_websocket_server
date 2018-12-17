#include "source/server/socket/system_wrapper.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include "source/logging/exception_handler.h"

/**
 * @throws std::runtime_error
 */
size_t epollWait(int epollFD,  struct epoll_event *events, int MAXEVENTS, int timeout) {
	int n = 0;
	n = epoll_wait(epollFD,  events, MAXEVENTS, timeout);
	if(n<0){
		if(errno == EINTR){
			return 0;	//signal was interupted, we can continue but no data was collected
		}
		else {
			int error = errno;
			throw std::runtime_error(LOG_EXCEPTION(std::string()+std::strerror(error)+" in epoll_wait"));
		}
	}
	auto count = static_cast<size_t>(n);
	return count;
}

/**
 * @throws std::runtime_error
 */
bool epollControlAdd(int epoll, int FD, struct epoll_event *event) {
	int ret =  epoll_ctl(epoll, EPOLL_CTL_ADD, FD, event);
	if (ret == -1){
		if(errno != EEXIST){
			int error = errno;
			throw std::runtime_error(LOG_EXCEPTION(std::string()+std::strerror(error)+" in epoll_ctl"));
		}
		else {
			return true;
		}
	}
	return false;
}

void epollControlDelete(int epoll, int FD, struct epoll_event *event) {
	epoll_ctl(epoll, EPOLL_CTL_DEL, FD, event);
}

/**
 * @throws std::runtime_error
 */
void epollControlMod(int epoll, int FD, struct epoll_event *event) {
	int ret = epoll_ctl(epoll, EPOLL_CTL_MOD, FD, event);
	if (ret == -1){
		if(event->events == (EPOLLOUT|EPOLLET)){
			throw std::runtime_error(LOG_EXCEPTION(std::string()+"could not change FD "+std::to_string(FD)+" to EPOLLOUT for epoll "+std::to_string(epoll)));
		}

		else if(event->events == (EPOLLIN|EPOLLET)){
			throw std::runtime_error(LOG_EXCEPTION(std::string()+"could not change FD "+std::to_string(FD)+" to EPOLLIN for epoll "+std::to_string(epoll)));
		}

		else if(event->events == (EPOLLIN|EPOLLOUT|EPOLLET)){
			throw std::runtime_error(LOG_EXCEPTION(std::string()+"could not change FD "+std::to_string(FD)+" to EPOLLIN|EPOLLOUT for epoll "+std::to_string(epoll)));
		}
	}
}

/**
 * @throws std::runtime_error
 */
int epollCreate(int flags) {
	int epollFD = epoll_create1(flags);
	if(epollFD == -1){
		int error = errno;
		throw std::runtime_error(LOG_EXCEPTION(std::string()+std::strerror(error)+" in epoll_create1"));
	}
	return epollFD;
}

/**
 * @throws std::runtime_error
 */
int getFlags(int FD) {
	int flags = fcntl (FD,F_GETFL, 0);
	if (flags == -1){
		int error = errno;
		throw std::runtime_error(LOG_EXCEPTION(std::string()+std::strerror(error)+" in fcntl"));
	}
	return flags;
}

/**
 * @throws std::runtime_error
 */
void setFlags(int FD, int flags) {
	int ret = fcntl (FD, F_SETFL, flags);
	if (ret == -1){
		int error = errno;
		throw std::runtime_error(LOG_EXCEPTION(std::string()+std::strerror(error)+" in fcntl"));
	}
}

void closeFD(int FD) {
	int ret = close(FD);
	if(ret == -1){
		int error = errno;
		LOG_ERROR(std::string()+" File descriptor "+std::to_string(FD)+" failed to close properly. "+std::strerror(error) );
	}
}

/**
 * @throws std::runtime_error
 */
size_t writeFD(int FD, const void *buf, size_t count) {
	if(buf == nullptr || FD<0){
		throw std::runtime_error(LOG_EXCEPTION("bad input. buf: "+std::to_string(reinterpret_cast<uint64_t>(buf))+" FD: "+std::to_string(FD)));
	}
	if(count == 0){
		return 0;
	}

	ssize_t ret = write(FD, buf, count);
	if(ret<0){
		int error = errno;
		if(!(error == EAGAIN)){
			throw std::runtime_error(LOG_EXCEPTION(std::string()+std::strerror(error)+" write error"));	//error
		}
		return 0;
	}
	else {
		return static_cast<size_t>(ret);
	}
}

/**
 * @throws std::runtime_error
 */
size_t readFD(int FD, void *buf, size_t count, bool &done) {
	done = false;
	if(buf == nullptr || FD<0){
		throw std::runtime_error(LOG_EXCEPTION("bad input. buf: "+std::to_string(reinterpret_cast<uint64_t>(buf))+" FD: "+std::to_string(FD)));	//error
	}
	if(count == 0){
		return 0;
	}

	ssize_t ret = read(FD, buf, count);
	if(ret<0){
		int error = errno;
		if(error != EAGAIN){
			throw std::runtime_error(LOG_EXCEPTION(std::string()+std::strerror(error)+" read error"));	//error
		}
		if(error == EAGAIN){
			done = true;//if error == EAGAIN we read all data. signal read is complete
		}
		return 0;
	}
	else {
		return static_cast<size_t>(ret);
	}
}

/**
 * @throws std::runtime_error
 */
void getNameInfo(const struct sockaddr *sa, unsigned int salen, char *host , unsigned int hostlen,  char *serv, unsigned int servlen, int flags) {
	if(sa == nullptr || host == nullptr || serv == nullptr){
		throw std::runtime_error(LOG_EXCEPTION("Input pointer was null. "));
	}
	int ret = getnameinfo(sa, salen, host , hostlen,  serv, servlen, flags);
	if(ret != 0){
		throw std::runtime_error(LOG_EXCEPTION(" failed to get name Info for file descriptor. "));
	}
}

/**
 * @throws std::runtime_error
 */
void getAddrInfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) {
	int ret = getaddrinfo(node, service, hints, res);
	if (ret != 0){
		throw std::runtime_error(LOG_EXCEPTION("getaddrinfoWrap: "+gaiStrError (ret)));
	}
}

void freeAddrInfo(struct addrinfo *res) {
	freeaddrinfo(res);
}

const char* gaiStrError(int errcode) {
	return gai_strerror(errcode);
}

int getSockOpt(int sockfd, int level, int optname, void *optval, unsigned int *optlen) {
	return getsockopt(sockfd, level, optname, optval, optlen);
}

char* strError(int errnum) {
	return std::strerror(errnum);
}

/**
 * @throws std::runtime_error
 */
int createSocket(int domain, int type, int protocol) {
	int ret = socket(domain, type, protocol);
	if(ret < 0){
		throw std::runtime_error(LOG_EXCEPTION("failed to create socket"));
	}
	return ret;
}

/**
 * @throws std::runtime_error
 */
void bindSocket(int sockfd, const struct sockaddr *addr, unsigned int addrlen) {
	int ret =bind(sockfd, addr, addrlen);
	if(ret != 0){
		throw std::runtime_error(LOG_EXCEPTION("failed to bind socket"));
	}
}

/**
 * @throws std::runtime_error
 */
void listenSocket(int sockfd, int backlog) {
	int ret = listen(sockfd, backlog);
	if(ret == -1){
		int error = errno;
		throw std::runtime_error(LOG_EXCEPTION(std::strerror(error)+" in listen"));
	}
}


int acceptSocket(int sockfd, struct sockaddr *addr, unsigned int *addrlen, bool &done) {
	done = false;
	int ret = accept(sockfd, addr, addrlen );
	if(ret == -1){
		int error = errno;
		if (error == EAGAIN){	//We have processed all connections.

		}
		else{
			LOG_ERROR( std::strerror(error));
		}
		done = true;
	}
	return ret;
}
