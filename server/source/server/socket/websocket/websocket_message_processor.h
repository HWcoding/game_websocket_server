#ifndef SOURCE_SERVER_SOCKET_WEBSOCKET_WEBSOCKET_MESSAGE_PROCESSOR_H_
#define SOURCE_SERVER_SOCKET_WEBSOCKET_WEBSOCKET_MESSAGE_PROCESSOR_H_
//#include "source/server/socket/websocket/websocket_message_processor.h"

#include <vector>
#include <memory>
#include "source/data_types/byte_array.h"

class SetOfFileDescriptors;
class WebsocketReadBuffers;
class MessageQueue;
class WebsocketReadBuffers;

class WebsocketMessageProcessor {
public:
	WebsocketMessageProcessor(SetOfFileDescriptors *_FDs);
	~WebsocketMessageProcessor();

	MessageQueue * getQueue();
	void setReaderQueue(MessageQueue *_readerQueue);
	void processSockMessage(ByteArray &in, int FD);
	void closeFDHandler(int FD);

private:
	MessageQueue *readerQueue;
	size_t MaxReadBufferSize;
	std::unique_ptr<WebsocketReadBuffers> ReadBuffers;
	SetOfFileDescriptors *fileDescriptors;

	WebsocketMessageProcessor& operator=(const WebsocketMessageProcessor&) = delete;
	WebsocketMessageProcessor(const WebsocketMessageProcessor&) = delete;
};

#endif /* SOURCE_SERVER_SOCKET_WEBSOCKET_WEBSOCKET_MESSAGE_PROCESSOR_H_ */
