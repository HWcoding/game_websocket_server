#ifndef SERVER_SOCKET_SOCKET_READER_H_
#define SERVER_SOCKET_SOCKET_READER_H_
//#include "source/server/socket/socket_reader.h"

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

class SocketReader{
public:
	void startPoll();
	void closeFDHandler(int FD);
	void newConnectionHandler(int FD);
	SocketMessage getNextMessage(); //blocks thread while queue is empty
	void shutdown(); //unblocks getNextMessage() if it's blocking and sets running to false; closing the server.

	SocketReader(SystemInterface *_systemWrap, SetOfFileDescriptors *_FDs, bool* run);
	~SocketReader();

private:
	void setupEpoll();
	void closeFD(int FD);
	void readChunk();
	bool readChunkFromFD(int FD);
	void addToWaitingFDs(int FD);
	void removeFromWaitingFDs(int FD);
	void processSockMessage(ByteArray &in,  int FD);

	SocketReader& operator=(const SocketReader&)=delete;
	SocketReader(const SocketReader&)=delete;

	SystemInterface *systemWrap;
	std::unique_ptr<WebsocketMessageProcessor> processor;
	bool *running;
	SetOfFileDescriptors *fileDescriptors;
	MessageQueue *readerQueue;
	std::set<int> waitingFDs;
	std::recursive_mutex waitingMut;
	int epollFD;
	int MAXEVENTS;
	size_t maxBufferSize;
};

#endif /* SERVER_SOCKET_SOCKET_READER_H_ */
