/*#ifndef TESTS_SERVER_MOCK_SYSTEM_WRAPPER_H
#define TESTS_SERVER_MOCK_SYSTEM_WRAPPER_H
//#include "tests/server/mock_system_wrapper.h"

#include <memory>
#include <string>
#include <sys/types.h>
#include "server/socket/system_wrapper.h"

class MockSystemState;
struct addrinfo;
struct sockaddr_storage;

class MockSystemWrapper : public SystemInterface{
public:
//mock specific members
	char nullChar;
	std::unique_ptr<struct addrinfo> mockAddrinfo;
	struct addrinfo* mockAddrinfoPointer;
	std::unique_ptr<struct sockaddr_storage> mockSockaddr;
	mutable std::unique_ptr<MockSystemState> state;

	MockSystemWrapper();
	~MockSystemWrapper();
	void SetReadBuffer(int FD, std::string buf);
	std::string GetReadBuffer(int FD);
	std::string GetWriteBuffer(int FD);
	void SetBytesTillWriteFail(int socket, int bytes);
	void ClearWriteBuffer(int FD);

//emulated members
	size_t epollWait(int epollFD,  struct epoll_event *_events, int MAXEVENTS, int timeout) const;
	int epollControl(int epoll, int op, int FD, struct epoll_event *event) const;
	bool epollControlAdd(int epoll, int FD, struct epoll_event *event) const;
	void epollControlDelete(int epoll, int FD, struct epoll_event *event) const;
	void epollControlMod(int epoll, int FD, struct epoll_event *event) const;
	int epollCreate(int flags) const;
	int getFlags(int FD) const;
	void setFlags(int FD, int _flags) const;
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
private:
	MockSystemWrapper& operator=(const MockSystemWrapper&)=delete;
	MockSystemWrapper(const MockSystemWrapper&)=delete;
};

#endif *//* TESTS_SERVER_MOCK_SYSTEM_WRAPPER_H */