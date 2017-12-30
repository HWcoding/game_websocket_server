#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_WRITE_BUFFER_H_
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_WRITE_BUFFER_H_
//#include "source/server/socket/websocket/websocket_write_buffer.h"

#include <mutex>
#include <unordered_map>
#include <vector>
#include "source/data_types/byte_array.h"

class WriteBuffersInterface {
public:
	virtual void addMessage(int index, const ByteArray &in) = 0;
	virtual bool writeData(int index) = 0;
	virtual size_t messageSize(int index) const = 0;
	virtual void eraseBuffers(int index) = 0;
	virtual ~WriteBuffersInterface();
protected:
	WriteBuffersInterface() = default;
};

class SystemInterface;
class SetOfFileDescriptors;

class WebsocketWriteBuffers : public WriteBuffersInterface{ //holds data shared across threads
private:
	SystemInterface *systemWrap;
	mutable std::mutex mut; // used for multi threaded writing.  A lock needs to be done before accessing the following variables

	WebsocketWriteBuffers& operator=(const WebsocketWriteBuffers&) = delete;
	WebsocketWriteBuffers(const WebsocketWriteBuffers&) = delete;
protected:
	struct pendingMessage{
		ByteArray message;
		size_t begin;
		pendingMessage() : message(), begin(0){}
	};
	std::unordered_map<int,pendingMessage> writeBuffer;	//buffer to hold messages waiting to be written to socket
public:
	void addMessage(int index, const ByteArray &in) override;
	bool writeData(int index) override;
	size_t messageSize(int index) const override;
	void eraseBuffers(int index) override;
	WebsocketWriteBuffers(SystemInterface *_systemWrap);
	~WebsocketWriteBuffers() override;
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_WRITE_BUFFER_H_ */
