#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_WRITE_BUFFER_H_
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_WRITE_BUFFER_H_

#include <mutex>
#include <unordered_map>
#include <vector>
#include "source/data_types/byte_array.h"

class WriteBuffersInterface {
public:
	virtual void addMessage(int index, const ByteArray &in) =0;
	virtual bool writeData(int index)				=0;
	virtual size_t messageSize(int index) const		=0;
	virtual void eraseBuffers(int index)			=0;
	virtual ~WriteBuffersInterface();
protected:
	WriteBuffersInterface(){}
};

class SystemInterface;
class SetOfFileDescriptors;

class WebsocketWriteBuffers : public WriteBuffersInterface{ //holds data shared across threads
private:
	struct pendingMessage{
		ByteArray message;
		size_t begin;
		pendingMessage() : message(), begin(0){}
	};

	SystemInterface *systemWrap;
	mutable std::mutex mut; // used for multi threaded writing.  A lock needs to be done before accessing the following variables
	std::unordered_map<int,pendingMessage> writeBuffer;	//buffer to hold messages waiting to be written to socket

	WebsocketWriteBuffers& operator=(const WebsocketWriteBuffers&)=delete;
	WebsocketWriteBuffers(const WebsocketWriteBuffers&)=delete;
public:
	void addMessage(int index, const ByteArray &in);
	bool writeData(int index);
	size_t messageSize(int index) const;
	void eraseBuffers(int index);
	WebsocketWriteBuffers(SystemInterface *_systemWrap);
	~WebsocketWriteBuffers();
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_WRITE_BUFFER_H_ */
