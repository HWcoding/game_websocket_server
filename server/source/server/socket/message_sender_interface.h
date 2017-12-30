#ifndef SERVER_SOCKET_MESSAGE_SENDER_INTERFACE_H_
#define SERVER_SOCKET_MESSAGE_SENDER_INTERFACE_H_
//#include "source/server/socket/message_sender_interface.h"

class SocketMessage;

/**
 * Interface for a class responsible for sending data to clients.
 * Use addMessage to queue data in the buffer, and then use writeData when
 * you are ready to begin sending it. Multiple calls to writeData may be
 * required to send all the data.
 *
 */
class MessageSenderInterface {
public:

	/**
	 * Adds a new message to the write buffer for a file descriptor. Which file
	 * descriptor is determined by examining the FD field of the message
	 *
	 * @param message A SocketMessage to be sent to the client.
	 *
	 */
	virtual void addMessage(SocketMessage &message) = 0;

	/**
	 * Writes the data stored in the file descriptor's write buffer to the
	 * network until either the buffer is empty or the write would have
	 * to wait and block the process.
	 *
	 * @param FD The file descriptor to write the data for.
	 *
	 * @return true if all the data in the buffer is sent. False if there
	 *         is still data left in the buffer after the write.
	 *
	 * @throws std::runtime_error if the message is larger than the maximum
	 *         allowed message or the OS encountered an error while writing.
	 */
	virtual bool writeData(int FD) = 0;

	/**
	 * Is called whenever a file descriptor that is connected to the server is
	 * disconnected. The method frees the buffers in use by that file descriptor.
	 *
	 * @param FD the file descriptor that was closed
	 */
	virtual void closeFDHandler(int FD) = 0;

	/* destructor */
	virtual ~MessageSenderInterface();
protected:
	/* constructor */
	MessageSenderInterface() = default;
};

#endif /* SERVER_SOCKET_MESSAGE_SENDER_INTERFACE_H_ */