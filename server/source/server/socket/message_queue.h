#ifndef SERVER_SOCKET_MESSAGE_QUEUE_H_
#define SERVER_SOCKET_MESSAGE_QUEUE_H_
//#include "server/socket/message_queue.h"

#include <queue>
#include <condition_variable>
#include <mutex>
#include <atomic>

class SocketMessage;

class MessageQueue{
public:
	MessageQueue(std::atomic<bool> *run);
	~MessageQueue();
	SocketMessage getNextMessage_Blocking();
	bool isEmpty();
	SocketMessage getNextMessage();
	int pushMessage(SocketMessage &message);
	void shutdown();
private:
	MessageQueue& operator=(const MessageQueue&) = delete;
	MessageQueue(const MessageQueue&) = delete;

	std::mutex mut;
	std::queue<SocketMessage> Queue;
	std::condition_variable bufferReady;
	std::atomic<bool> *running;
};

#endif /* SERVER_SOCKET_MESSAGE_QUEUE_H_ */
