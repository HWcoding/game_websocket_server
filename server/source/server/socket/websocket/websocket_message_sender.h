#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_MESSAGE_SENDER_H_
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_MESSAGE_SENDER_H_
//#include "source/server/socket/websocket/websocket_message_sender.h"

#include <memory> // for std::unique_ptr
#include "source/data_types/byte_array.h"
#include "source/server/socket/message_sender_interface.h"

class SetOfFileDescriptors;
class SocketMessage;
class WebsocketWriteBuffers;

/**
 * Class responsible for sending data to clients using the Websocket protocol.
 * Use addMessage to queue data in the buffer, and then use writeData when
 * you are ready to begin sending it. Multiple calls to writeData may be
 * required to send all the data.
 *
 */
class WebsocketMessageSender : public MessageSenderInterface {
public:
	WebsocketMessageSender();

	/* see source/server/socket/message_sender_interface.h */
	void addMessage(SocketMessage &message) override;
	/* see source/server/socket/message_sender_interface.h */
	bool writeData(int FD) override;
	/* see source/server/socket/message_sender_interface.h */
	void closeFDHandler(int FD) override;

	/** destructor */
	~WebsocketMessageSender() override;

	/** deleted */
	WebsocketMessageSender& operator=(const WebsocketMessageSender&) = delete;

	/** deleted */
	WebsocketMessageSender(const WebsocketMessageSender&) = delete;
protected:

	/**
	 * Creates a frame header for a single frame of a message and contains the
	 * opcode and size of the frame.
	 *
	 * @param frameSize The size of the frame in bytes
	 * @param opcode The opcode for the frame
	 * @param finished true if this is the last frame in the message being sent
	 *
	 * @return ByteArray containing the FrameHeader
	 *
	 * @throws std::runtime_error if a fragmentation is used with a control opcode
	 */
	ByteArray createFrameHeader(size_t frameSize, uint8_t opcode, bool finished = true);
private:

	/** holds unsent messages for connected clients. */
	std::unique_ptr<WebsocketWriteBuffers> writeBuffers;

	/** maximum size that a single message can be */
	size_t MaxWriteBufferSize;
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_MESSAGE_SENDER_H_ */
