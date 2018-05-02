/*#ifndef SERVER_SOCKET_SYSTEM_INTERFACE_H_
#define SERVER_SOCKET_SYSTEM_INTERFACE_H_
//#include "source/server/socket/system_interface.h"

#include <sys/types.h>

class SystemInterface {
public:
	virtual size_t epollWait(int epollFD,  struct epoll_event *events, int MAXEVENTS, int timeout) const = 0;
	virtual bool epollControlAdd(int epoll, int FD, struct epoll_event *event) const = 0;
	virtual void epollControlDelete(int epoll, int FD, struct epoll_event *event) const = 0;
	virtual void epollControlMod(int epoll, int FD, struct epoll_event *event) const = 0;
	virtual int epollCreate(int flags) const = 0;
	virtual int getFlags(int FD) const = 0;
	virtual void setFlags(int FD, int flags) const = 0;
	virtual void closeFD(int FD) const = 0;
	virtual size_t writeFD(int FD, const void *buf, size_t count) const = 0;
	virtual size_t readFD(int FD, void *buf, size_t count, bool &done) const = 0;
	virtual void getNameInfo(const struct sockaddr *sa, unsigned int salen, char *host , unsigned int hostlen,  char *serv, unsigned int servlen, int flags) const = 0;
	virtual void getAddrInfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) const = 0;
	virtual void freeAddrInfo(struct addrinfo *res) const = 0;
	virtual const char *gaiStrError(int errcode) const = 0;
	virtual int getSockOpt(int sockfd, int level, int optname, void *optval, unsigned int *optlen) const = 0;
	virtual char *strError(int errnum) const = 0;
	virtual int createSocket(int domain, int type, int protocol) const = 0;
	virtual void bindSocket(int sockfd, const struct sockaddr *addr, unsigned int addrlen) const = 0;
	virtual void listenSocket(int sockfd, int backlog) const = 0;
	virtual int acceptSocket(int sockfd, struct sockaddr *addr, unsigned int *addrlen, bool &done) const = 0;
	virtual ~SystemInterface();
protected:
	SystemInterface() = default;
};

#endif *//* SERVER_SOCKET_SYSTEM_INTERFACE_H_ */
