#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_READ_BUFFER_H_
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_READ_BUFFER_H_
//#include "source/server/socket/websocket/websocket_read_buffer.h"

#include <mutex>
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <cstring>
#include "source/data_types/byte_array.h"
#include "source/data_types/socket_message.h"

class SetOfFileDescriptors;

class WebsocketReadBuffers
{
private:
	size_t maxMessageSize;
	std::mutex mut;
	SetOfFileDescriptors *fileDescriptors;
	WebsocketReadBuffers& operator=(const WebsocketReadBuffers&) = delete;
	WebsocketReadBuffers(const WebsocketReadBuffers&) = delete;

protected:

	struct MessageBuffer
	{
		ByteArray workingMessage;
		ByteArray completeFragments;
		uint8_t currentOpcode;
		int64_t expectedSize;
		std::vector<ByteArray> completeMessages;
		MessageBuffer() : workingMessage(), completeFragments(), currentOpcode(0), expectedSize(-1), completeMessages() {}
		std::vector<SocketMessage> extractCompletedMessages(ByteArray &in, int FD, size_t maxMessageSize, SetOfFileDescriptors *fileDescriptors);
	};

	std::unordered_map<int,MessageBuffer> messages;

public:
	std::vector<SocketMessage> extractMessages(ByteArray &in, int FD);
	void eraseBuffers(int index);
	WebsocketReadBuffers(SetOfFileDescriptors *FDs, size_t size);
	~WebsocketReadBuffers();
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_READ_BUFFER_H_ */
