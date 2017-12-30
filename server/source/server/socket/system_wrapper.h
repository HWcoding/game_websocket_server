#ifndef SERVER_SOCKET_SYSTEM_WRAPPER_H_
#define SERVER_SOCKET_SYSTEM_WRAPPER_H_
//#include "source/server/socket/system_wrapper.h"

#include <sys/types.h>
#include "source/server/socket/system_interface.h"

class SystemWrapper : public SystemInterface{
public:
	size_t epollWait(int epollFD,  struct epoll_event *events, int MAXEVENTS, int timeout) const override;
	bool epollControlAdd(int epoll, int FD, struct epoll_event *event) const override;
	void epollControlDelete(int epoll, int FD, struct epoll_event *event) const override;
	void epollControlMod(int epoll, int FD, struct epoll_event *event) const override;
	int epollCreate(int flags) const override;
	int getFlags(int FD) const override;
	void setFlags(int FD, int flags) const override;
	void closeFD(int FD) const override;
	size_t writeFD(int FD, const void *buf, size_t count) const override;
	size_t readFD(int FD, void *buf, size_t count, bool &done) const override;
	void getNameInfo(const struct sockaddr *sa, unsigned int salen, char *host , unsigned int hostlen,  char *serv, unsigned int servlen, int flags) const override;
	void getAddrInfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) const override;
	void freeAddrInfo(struct addrinfo *res) const override;
	const char *gaiStrError(int errcode) const override;
	int getSockOpt(int sockfd, int level, int optname, void *optval, unsigned int *optlen) const override;
	char *strError(int errnum) const override;
	int createSocket(int domain, int type, int protocol) const override;
	void bindSocket(int sockfd, const struct sockaddr *addr, unsigned int addrlen) const override;
	void listenSocket(int sockfd, int backlog) const override;
	int acceptSocket(int sockfd, struct sockaddr *addr, unsigned int *addrlen, bool &done) const override;
};

#endif /* SERVER_SOCKET_SYSTEM_WRAPPER_H_ */
