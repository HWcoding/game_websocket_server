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

	struct Fracture
	{
		size_t totalSize;
		int64_t expectedSize;
		size_t size() const {return totalSize;}
		std::queue<ByteArray> buffer;
		bool fractureBufferType;
		bool complete;
		Fracture() : totalSize(0), expectedSize(0), buffer(), fractureBufferType(true), complete(false) {}
		void increaseSize(size_t addSize) {
			totalSize += addSize;
		}
		size_t addPartialMessage(ByteArray &in, size_t start) {
			ByteArray newArray;
			size_t newSize = in.size() - start;
			newArray.resize(newSize);
			memcpy(&newArray[0], &in[start], newSize);
			buffer.push(std::move(newArray));
			totalSize += newSize;
			return newSize;
		}
	};

	struct MessageBuffer
	{
		size_t totalSize;
		size_t size() const {return totalSize;}
		std::queue<Fracture> buffer;
		MessageBuffer() : totalSize(0), buffer() {}
		void addPartialMessage(ByteArray &in, size_t start) {
			if( buffer.empty() ) {
				buffer.push(Fracture());
			}
			totalSize += buffer.back().addPartialMessage(in, start);
		}

		void finishFracture() {
			buffer.back().complete = true;
			buffer.push(Fracture());
		}

		ByteArray extractFragments() {
			ByteArray out;
			if( buffer.empty() ) {
				return out;
			}

			out.reserve( totalSize );

			Fracture *firstElement = &(buffer.front());
			while( ! buffer.empty() && firstElement->complete) {
				while( ! firstElement->buffer.empty() ) {
					out.append( firstElement->buffer.front() );
					firstElement->buffer.pop();
				}
				buffer.pop();
				if( ! buffer.empty() ) {
					firstElement = &(buffer.front());
				}
			}
			return out;
		}

		ByteArray extractPartialMessage() {
			ByteArray out;
			if( buffer.empty() ) {
				return out;
			}

			Fracture *lastElement = &(buffer.back());

			if( lastElement->complete ) {
				return out;
			}

			out.reserve( lastElement->totalSize );

			while( ! lastElement->buffer.empty() ) {
				out.append( lastElement->buffer.front() );
				lastElement->buffer.pop();
			}
			buffer.pop();
			return out;
		}
	};





	//std::unordered_map<int,PartialMessage> messageBuffer; //buffer to hold websocket frames that are too short to process
	//std::unordered_map<int,Fracture> fractureBuffer; //buffer to hold messages split across more than one frame
	//std::unordered_map<int,bool> fractureBufferType; //true = text, false = binary
	std::unordered_map<int,MessageBuffer> currentMessage;

public:
	void addMessage(int index, ByteArray &in);
	bool extractMessage(ByteArray &out, size_t position, int index);
	bool messageIsEmpty(int index);
	void eraseBuffers(int index);
	WebsocketReadBuffers(SetOfFileDescriptors *FDs, size_t size);
	~WebsocketReadBuffers();

//private:
	//void setMessageSize(int index, size_t _size);
	//bool extractMessage(ByteArray &out, size_t position, int index);
	//void addFracture(int index,ByteArray &in);
	//void extractFracture(ByteArray &out, size_t position, int index);
	//void clearFracture(int index);
	//void fractureTypeSet(int index, bool flag);
	//bool getFractureType(int index);
	//void eraseFractureType(int index);
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_READ_BUFFER_H_ */
