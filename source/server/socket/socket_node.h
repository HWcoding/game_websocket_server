#ifndef SERVER_SOCKET_SOCKET_NODE
#define SERVER_SOCKET_SOCKET_NODE
//#include "source/server/socket/socket_node.h"

#include <string>
#include <vector>
#include <memory>

class SystemInterface;
class SetOfFileDescriptors;
struct epoll_event;

class SocketNode {
public:
	void startPoll();
	virtual void closeFD(int FD);
	virtual ~SocketNode();

protected:
	SocketNode(SystemInterface *_systemWrap, SetOfFileDescriptors *FDs, std::atomic<bool>* run);
	virtual void setupEpoll();
	virtual bool handleEpollErrors(epoll_event &event);
	virtual void handleEpollRead(epoll_event &event) = 0;
	virtual void handleEpollWrite(epoll_event &event) = 0;
	virtual int getWaitTime();

	SystemInterface *systemWrap;
	std::atomic<bool> *running;
	SetOfFileDescriptors *fileDescriptors;
	int MAXEVENTS;
	int epollFD;

private:
	void processEvents(std::vector<epoll_event> &events);
	SocketNode& operator=(const SocketNode&) = delete;
	SocketNode(const SocketNode&) = delete;
};

#endif /* SERVER_SOCKET_SOCKET_NODE */
