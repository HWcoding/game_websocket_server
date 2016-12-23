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
	virtual void addMessage(SocketMessage &message) =0;
	virtual bool writeData(int FD) =0;
	virtual void closeFDHandler(int FD) =0;
	virtual ~MessageSenderInterface();
protected:
	MessageSenderInterface(){}
};

class WebsocketMessageSender : public MessageSenderInterface {
public:
	WebsocketMessageSender(SystemInterface *_systemWrap);
	WebsocketMessageSender(WebsocketWriteBuffers *_writeBuffers );
	void addMessage(SocketMessage &message);
	bool writeData(int FD);
	void closeFDHandler(int FD);
	WebsocketMessageSender& operator=(const WebsocketMessageSender&)=delete;
	WebsocketMessageSender(const WebsocketMessageSender&)=delete;
	~WebsocketMessageSender();
protected:
	std::unique_ptr<WebsocketWriteBuffers> writeBuffers;
	ByteArray createFrameHeader(const ByteArray &in, uint8_t opcode);
private:
	size_t MaxWriteBufferSize;
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_MESSAGE_SENDER_H_ */
