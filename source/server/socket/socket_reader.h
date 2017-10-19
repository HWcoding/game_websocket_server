#ifndef SERVER_SOCKET_SOCKET_READER_H_
#define SERVER_SOCKET_SOCKET_READER_H_
//#include "source/server/socket/socket_reader.h"


#include "source/server/socket/socket_node.h"
#include <mutex>
#include <condition_variable>
#include <set>
#include <memory>
#include <vector>

class SocketMessage;
class SystemInterface;
class MessageQueue;
class SetOfFileDescriptors;
class WebsocketMessageProcessor;
class ByteArray;
struct epoll_event;

class SocketReader: public SocketNode {
public:
	void closeFDHandler(int FD);
	void newConnectionHandler(int FD);
	SocketMessage getNextMessage(); //blocks thread while queue is empty
	void shutdown(); //unblocks getNextMessage() if it's blocking and sets running to false; closing the server.

	SocketReader(SystemInterface *_systemWrap, SetOfFileDescriptors *_FDs, std::atomic<bool>* run);
	~SocketReader();

private:
	int getWaitTime() override;
	void setupEpoll() override;
	void handleEpollRead(epoll_event &event) override;
	void handleEpollWrite(epoll_event &event) override;
	void readChunk();
	bool readChunkFromFD(int FD);
	void addToWaitingFDs(int FD);
	void removeFromWaitingFDs(int FD);
	void processSockMessage(ByteArray &in,  int FD);

	SocketReader& operator=(const SocketReader&) = delete;
	SocketReader(const SocketReader&) = delete;

	std::unique_ptr<WebsocketMessageProcessor> processor;
	MessageQueue *readerQueue;
	std::set<int> waitingFDs;
	std::recursive_mutex waitingMut;
	size_t maxBufferSize;
};

#endif /* SERVER_SOCKET_SOCKET_READER_H_ */
