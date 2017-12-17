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

protected:
	void		storePartialMessage(ByteArray &in, uint64_t start, int FD);
	size_t 		extractMessages(ByteArray &in, std::vector< ByteArray > &out, std::vector<int> &types, int FD);
	int64_t 	getMessageSize(ByteArray &in, uint64_t &messageStart, const uint64_t &start, int FD);
	void 		completeFracture(ByteArray &out, int &types, size_t position, int FD);
	void 		handleFragment(ByteArray &in, uint8_t opcode, int FD);
	void 		unmask(ByteArray &in, ByteArray &out, uint64_t messageStart, uint64_t length);
	uint64_t 	getNet64bit(uint8_t *in);
	uint16_t 	getNet16bit(uint8_t *in);
private:
	MessageQueue *readerQueue;
	size_t MaxReadBufferSize;
	std::unique_ptr<WebsocketReadBuffers> ReadBuffers;
	SetOfFileDescriptors *fileDescriptors;

	WebsocketMessageProcessor& operator=(const WebsocketMessageProcessor&) = delete;
	WebsocketMessageProcessor(const WebsocketMessageProcessor&) = delete;
};

#endif /* SOURCE_SERVER_SOCKET_WEBSOCKET_WEBSOCKET_MESSAGE_PROCESSOR_H_ */
