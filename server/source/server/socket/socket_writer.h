#ifndef SERVER_SOCKET_SOCKET_WRITER_H_
#define SERVER_SOCKET_SOCKET_WRITER_H_
//#include "source/server/socket/socket_writer.h"

#include "source/server/socket/socket_node.h"
#include <mutex>
#include <memory>

class SocketMessage;
class SetOfFileDescriptors;
class MessageSenderInterface;

class SocketWriter : public SocketNode {
public:
	void closeFDHandler(int FD);
	void newConnectionHandler(int FD);
	void sendMessage(SocketMessage &message);

	SocketWriter(SetOfFileDescriptors *FDs, std::atomic<bool>* run);
	~SocketWriter();
private:
	void setupEpoll() override;
	void handleEpollRead(epoll_event &event) override;
	void handleEpollWrite(epoll_event &event) override;
	void writeData(int FD);

	SocketWriter& operator=(const SocketWriter&) = delete;
	SocketWriter(const SocketWriter&) = delete;

	std::unique_ptr<MessageSenderInterface> sender;
	std::mutex writePollingMut;
};

#endif /* SERVER_SOCKET_SOCKET_WRITER_H_ */
