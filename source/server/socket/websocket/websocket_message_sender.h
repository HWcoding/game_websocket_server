#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_MESSAGE_SENDER_H_
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_MESSAGE_SENDER_H_
//#include "source/server/socket/websocket/websocket_message_sender.h"

#include <unistd.h>
#include <memory>
#include <vector>
#include "source/data_types/byte_array.h"

class SetOfFileDescriptors;
class SocketMessage;
class SystemInterface;
class WebsocketWriteBuffers;

class MessageSenderInterface {
public:
	virtual void addMessage(SocketMessage &message) = 0;
	virtual bool writeData(int FD) = 0;
	virtual void closeFDHandler(int FD) = 0;
	virtual ~MessageSenderInterface();
protected:
	MessageSenderInterface() = default;
};

class WebsocketMessageSender : public MessageSenderInterface {
public:
	WebsocketMessageSender(SystemInterface *_systemWrap);
	void addMessage(SocketMessage &message) override;
	bool writeData(int FD) override;
	void closeFDHandler(int FD) override;
	WebsocketMessageSender& operator=(const WebsocketMessageSender&) = delete;
	WebsocketMessageSender(const WebsocketMessageSender&) = delete;
	~WebsocketMessageSender() override;
protected:
	std::unique_ptr<WebsocketWriteBuffers> writeBuffers;
	ByteArray createFrameHeader(const ByteArray &in, uint8_t opcode);
private:
	size_t MaxWriteBufferSize;
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_MESSAGE_SENDER_H_ */
