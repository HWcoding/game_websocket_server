#ifndef SERVER_SOCKET_SYSTEM_WRAPPER_H_
#define SERVER_SOCKET_SYSTEM_WRAPPER_H_
//#include "source/server/socket/system_wrapper.h"

#include <sys/types.h>

class SystemInterface {
public:
	virtual size_t epollWait(int epollFD,  struct epoll_event *events, int MAXEVENTS, int timeout) const 												=0;
	virtual bool epollControlAdd(int epoll, int FD, struct epoll_event *event) const 																	=0;
	virtual void epollControlDelete(int epoll, int FD, struct epoll_event *event) const 																=0;
	virtual void epollControlMod(int epoll, int FD, struct epoll_event *event) const 																	=0;
	virtual int epollCreate(int flags) const 																											=0;
	virtual int getFlags(int FD) const 																													=0;
	virtual void setFlags(int FD, int flags) const 																										=0;
	virtual void closeFD(int FD) const 																													=0;
	virtual size_t writeFD(int FD, const void *buf, size_t count) const 																				=0;
	virtual size_t readFD(int FD, void *buf, size_t count, bool &done) const 																			=0;
	virtual void getNameInfo(const struct sockaddr *sa, unsigned int salen, char *host , unsigned int hostlen,  char *serv, unsigned int servlen, int flags) const	=0;
	virtual void getAddrInfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) const 							=0;
	virtual void freeAddrInfo(struct addrinfo *res) const																								=0;
	virtual const char *gaiStrError(int errcode) const																									=0;
	virtual int getSockOpt(int sockfd, int level, int optname, void *optval, unsigned int *optlen) const 												=0;
	virtual char *strError(int errnum) const																											=0;
	virtual int createSocket(int domain, int type, int protocol) const																					=0;
	virtual void bindSocket(int sockfd, const struct sockaddr *addr, unsigned int addrlen) const 														=0;
	virtual void listenSocket(int sockfd, int backlog) const 																							=0;
	virtual int acceptSocket(int sockfd, struct sockaddr *addr, unsigned int *addrlen, bool &done) const 												=0;
	virtual ~SystemInterface();
protected:
	SystemInterface(){}
};

class SystemWrapper : public SystemInterface{
public:
	size_t epollWait(int epollFD,  struct epoll_event *events, int MAXEVENTS, int timeout) const;
	bool epollControlAdd(int epoll, int FD, struct epoll_event *event) const;
	void epollControlDelete(int epoll, int FD, struct epoll_event *event) const;
	void epollControlMod(int epoll, int FD, struct epoll_event *event) const;
	int epollCreate(int flags) const;
	int getFlags(int FD) const;
	void setFlags(int FD, int flags) const;
	void closeFD(int FD) const;
	size_t writeFD(int FD, const void *buf, size_t count) const;
	size_t readFD(int FD, void *buf, size_t count, bool &done) const;
	void getNameInfo(const struct sockaddr *sa, unsigned int salen, char *host , unsigned int hostlen,  char *serv, unsigned int servlen, int flags) const;
	void getAddrInfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) const;
	void freeAddrInfo(struct addrinfo *res) const;
	const char *gaiStrError(int errcode) const;
	int getSockOpt(int sockfd, int level, int optname, void *optval, unsigned int *optlen) const;
	char *strError(int errnum) const;
	int createSocket(int domain, int type, int protocol) const;
	void bindSocket(int sockfd, const struct sockaddr *addr, unsigned int addrlen) const;
	void listenSocket(int sockfd, int backlog) const;
	int acceptSocket(int sockfd, struct sockaddr *addr, unsigned int *addrlen, bool &done) const;
};

#endif /* SERVER_SOCKET_SYSTEM_WRAPPER_H_ */
