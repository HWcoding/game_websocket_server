#ifndef SERVER_SOCKET_SYSTEM_WRAPPER_H_
#define SERVER_SOCKET_SYSTEM_WRAPPER_H_
//#include "source/server/socket/system_wrapper.h"

#include <sys/types.h>

/*
 * Wrappers around system calls to allow link seams for unit testing
 */
size_t epollWait(int epollFD,  struct epoll_event *events, int MAXEVENTS, int timeout);
bool epollControlAdd(int epoll, int FD, struct epoll_event *event);
void epollControlDelete(int epoll, int FD, struct epoll_event *event);
void epollControlMod(int epoll, int FD, struct epoll_event *event);
int epollCreate(int flags);
int getFlags(int FD);
void setFlags(int FD, int flags);
void closeFD(int FD);
size_t writeFD(int FD, const void *buf, size_t count);
size_t readFD(int FD, void *buf, size_t count, bool &done);
void getNameInfo(const struct sockaddr *sa, unsigned int salen, char *host , unsigned int hostlen,  char *serv, unsigned int servlen, int flags);
void getAddrInfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
void freeAddrInfo(struct addrinfo *res);
const char *gaiStrError(int errcode);
int getSockOpt(int sockfd, int level, int optname, void *optval, unsigned int *optlen);
char *strError(int errnum);
int createSocket(int domain, int type, int protocol);
void bindSocket(int sockfd, const struct sockaddr *addr, unsigned int addrlen);
void listenSocket(int sockfd, int backlog);
int acceptSocket(int sockfd, struct sockaddr *addr, unsigned int *addrlen, bool &done);

#endif /* SERVER_SOCKET_SYSTEM_WRAPPER_H_ */
