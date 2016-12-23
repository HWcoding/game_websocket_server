#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_READ_BUFFER_H_
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_READ_BUFFER_H_

#include <mutex>
#include <unordered_map>
#include <string>
#include <vector>
#include "source/data_types/byte_array.h"

class SetOfFileDescriptors;

class WebsocketReadBuffers
{
private:
	size_t maxMessageSize;
	std::mutex mut;

	SetOfFileDescriptors *fileDescriptors;

	WebsocketReadBuffers& operator=(const WebsocketReadBuffers&)=delete;
	WebsocketReadBuffers(const WebsocketReadBuffers&)=delete;
protected:
	struct Fracture{
		std::vector< ByteArray > buffer;
		size_t totalSize;
		size_t size() const{return totalSize;}
		Fracture() : buffer(), totalSize(0) {}
		void increaseSize(size_t addSize){
			totalSize += addSize;
		}
	};

	struct PartialMessage{
		std::vector< ByteArray > buffer;
		size_t totalSize;
		int64_t expectedSize;
		size_t size() const{return totalSize;}
		PartialMessage() : buffer(), totalSize(0), expectedSize(-1){}
		void increaseSize(size_t addSize){
			totalSize += addSize;
		}
		void clear(){
			buffer=std::vector< ByteArray >();
			totalSize = 0;
			expectedSize = -1;
		}
	};
	std::unordered_map<int,PartialMessage> messageBuffer;	//buffer to hold websocket frames that are too short to process
	std::unordered_map<int,Fracture> fractureBuffer;	//buffer to hold messages split across more than one frame
	std::unordered_map<int,bool> fractureBufferType;	//true = text  false = binary

public:
	void addMessage(int index, ByteArray &in);
	bool messageIsEmpty(int index);
	void setMessageSize(int index, size_t _size);
	bool extractMessage(ByteArray &out, size_t position, int index);
	void addFracture(int index,ByteArray &in);
	void extractFracture(ByteArray &out, size_t position, int index);
	void clearFracture(int index);
	void fractureTypeSet(int index, bool flag);
	bool getFractureType(int index);
	void eraseFractureType(int index);
	void eraseBuffers(int index);
	WebsocketReadBuffers(SetOfFileDescriptors *FDs, size_t size);
	~WebsocketReadBuffers();
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_READ_BUFFER_H_ */
