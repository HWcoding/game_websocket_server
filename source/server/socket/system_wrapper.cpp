#include "source/server/socket/system_wrapper.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <fcntl.h>
#include <cstring>
#include <errno.h>

SystemInterface::~SystemInterface(){}

size_t SystemWrapper::epollWait(int epollFD,  struct epoll_event *events, int MAXEVENTS, int timeout) const{
	int n = 0;
	n = epoll_wait(epollFD,  events, MAXEVENTS, timeout);
	if(n<0){
		if(errno == EINTR) return 0;	//signal was interupted, we can continue but no data was collected
		else throw -1;//throwInt(std::strerror(errno)<<" in epoll_wait");
	}
	size_t count = static_cast<size_t>(n);
	return count;
}

bool SystemWrapper::epollControlAdd(int epoll, int FD, struct epoll_event *event) const{
	int ret =  epoll_ctl(epoll, EPOLL_CTL_ADD, FD, event);
	if (ret == -1){
		if(errno != EEXIST){
			throw -1;//throwInt(std::strerror(errno)<<" in epoll_ctl");
		}
		else return true;
	}
	return false;
}

void SystemWrapper::epollControlDelete(int epoll, int FD, struct epoll_event *event) const{
	int ret =  epoll_ctl(epoll, EPOLL_CTL_DEL, FD, event);
	if (ret == -1){
		throw -1;//throwInt(std::strerror(errno)<<" in epoll_ctl");
	}
}

void SystemWrapper::epollControlMod(int epoll, int FD, struct epoll_event *event) const{
	int ret = epoll_ctl(epoll, EPOLL_CTL_MOD, FD, event);
	if (ret == -1){
		if(event->events == (EPOLLOUT|EPOLLET))					throw -1;//throwInt("could not change FD "<<FD<<" to EPOLLOUT for epoll "<<epoll);
		else if(event->events == (EPOLLIN|EPOLLET))				throw -1;//throwInt("could not change FD "<<FD<<" to EPOLLIN for epoll "<<epoll);
		else if(event->events == (EPOLLIN|EPOLLOUT|EPOLLET))	throw -1;//throwInt("could not change FD "<<FD<<" to EPOLLIN|EPOLLOUT for epoll "<<epoll);
	}
}


int SystemWrapper::epollCreate(int flags) const{
	int epollFD = epoll_create1(flags);
	if(epollFD == -1) throw -1;//throwInt(std::strerror(errno)<<" in epoll_create1");
	return epollFD;
}

int SystemWrapper::getFlags(int FD) const{
	int flags = fcntl (FD,F_GETFL, 0);
	if (flags == -1){
		throw -1;//throwInt(std::strerror(errno)<<" in fcntl");
	}
	return flags;
}

void SystemWrapper::setFlags(int FD, int flags) const{
	int ret = fcntl (FD, F_SETFL, flags);
	if (ret == -1){
		throw -1;//throwInt(std::strerror(errno)<<" in fcntl");
	}
}

void SystemWrapper::closeFD(int FD) const{
	int ret = close(FD);
	//int error = errno;
	if(ret == -1){
		//LOG_ERROR("SystemWrapper::closeFD"," File descriptor "<<FD<<"failed to close properly. "<< std::strerror(error) );
	}
}

size_t SystemWrapper::writeFD(int FD, const void *buf, size_t count) const{
	if(buf == NULL || FD<0){
		throw -1;//throwInt("bad input. buf: "<<(reinterpret_cast<uint64_t>(buf))<<" FD: "<<FD);	//error
	}
	if(count == 0)return 0;
	ssize_t ret = write(FD, buf, count);
	int errnoret = errno;
	if(ret<0){
		if(errnoret != EAGAIN && errnoret != EWOULDBLOCK){
			throw -1;//throwInt(std::strerror(errnoret)<<" write error");	//error
		}
		return 0;
	}
	else return static_cast<size_t>(ret);
}

size_t SystemWrapper::readFD(int FD, void *buf, size_t count, bool &done) const{
	done = false;
	if(buf == NULL || FD<0){
		throw -1;//throwInt("bad input. buf: "<<(reinterpret_cast<uint64_t>(buf))<<" FD: "<<FD);	//error
	}
	if(count == 0)return 0;
	ssize_t ret = read(FD, buf, count);
	int errnoret = errno;
	if(ret<0){
		if(errnoret != EAGAIN && errnoret != EWOULDBLOCK){
			throw -1;//throwInt(std::strerror(errnoret)<<" read error");	//error
		}
		if(errnoret == EAGAIN) done = true;//if error == EAGAIN we read all data. signal read is complete
		return 0;
	}
	else return static_cast<size_t>(ret);
}

void SystemWrapper::getNameInfo(const struct sockaddr *sa, unsigned int salen, char *host , unsigned int hostlen,  char *serv, unsigned int servlen, int flags) const{
	if(sa == NULL || host == NULL || serv == NULL) throw -1;//throwInt("Input pointer was null. ");
	int ret = getnameinfo(sa, salen, host , hostlen,  serv, servlen, flags);
	if(ret != 0) throw -1;//throwInt(" failed to get name Info for file descriptor. ");
}

void SystemWrapper::getAddrInfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) const{
	int ret = getaddrinfo(node, service, hints, res);
	if (ret != 0){
		throw -1;//throwInt("getaddrinfoWrap: "<<gaiStrError (ret));
	}
}

void SystemWrapper::freeAddrInfo(struct addrinfo *res) const{
	freeaddrinfo(res);
}

const char* SystemWrapper::gaiStrError(int errcode) const{
	return gai_strerror(errcode);
}

int SystemWrapper::getSockOpt(int sockfd, int level, int optname, void *optval, unsigned int *optlen) const{
	return getsockopt(sockfd, level, optname, optval, optlen);
}

char* SystemWrapper::strError(int errnum) const{
	return std::strerror(errnum);
}

int SystemWrapper::createSocket(int domain, int type, int protocol) const{
	int ret = socket(domain, type, protocol);
	if(ret < 0) throw -1;
	return ret;
}

void SystemWrapper::bindSocket(int sockfd, const struct sockaddr *addr, unsigned int addrlen) const{
	int ret =bind(sockfd, addr, addrlen);
	if(ret != 0) throw -1;
}

void SystemWrapper::listenSocket(int sockfd, int backlog) const{
	int ret = listen(sockfd, backlog);
	if(ret == -1){
		throw -1;//throwInt(std::strerror(errno)<<" in listen");
	}
}

int SystemWrapper::acceptSocket(int sockfd, struct sockaddr *addr, unsigned int *addrlen, bool &done) const{
	done = false;
	int ret = accept(sockfd, addr, addrlen );
	if(ret == -1){
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK)){	//We have processed all connections.

		}
		else{
			//LOG_ERROR("Error in accept: ", std::strerror(errno));
		}
		done = true;
	}
	return ret;
}
