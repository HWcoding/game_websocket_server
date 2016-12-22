#include "source/server/socket/websocket/websocket_write_buffer.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/system_wrapper.h"
#include <string.h>

WriteBuffersInterface::~WriteBuffersInterface(){}

void WebsocketWriteBuffers::addMessage(int index, const ByteArray &in){
	std::lock_guard<std::mutex> lck(mut);
	pendingMessage& tempBuff = writeBuffer[index];
	//if writeBuffer is large and most of it has been sent, clip off the processed part
	if(tempBuff.begin > 100000 && tempBuff.begin > tempBuff.message.size()/2){
		ByteArray clippedBuffer(tempBuff.message.begin()+static_cast<int64_t>(tempBuff.begin), tempBuff.message.end() );
		tempBuff.message = std::move(clippedBuffer);
		tempBuff.begin = 0;
	}
	size_t messageSize = tempBuff.message.size();
	tempBuff.message.resize(messageSize + in.size());
	memcpy( &tempBuff.message[messageSize], &in[0], in.size() );
}

bool WebsocketWriteBuffers::writeData(int index){
	std::lock_guard<std::mutex> lck(mut);
	pendingMessage& tempBuff = writeBuffer[index];
	size_t  ret = systemWrap->writeFD(index, &tempBuff.message[tempBuff.begin],  tempBuff.message.size()-tempBuff.begin);
	tempBuff.begin += ret;
	if (tempBuff.begin == tempBuff.message.size()){ //all data in buffer is sent
		writeBuffer.erase(index);
		return true;
	}
	else return false;
}

size_t WebsocketWriteBuffers::messageSize(int index) const{
	std::lock_guard<std::mutex> lck(mut);
	if(writeBuffer.count(index) != 0){
		return writeBuffer.at(index).message.size()-writeBuffer.at(index).begin;
	}
	else return 0;
}

void WebsocketWriteBuffers::eraseBuffers(int index){
	std::lock_guard<std::mutex> lck(mut);
	if(writeBuffer.count(index) > 0) writeBuffer.erase(index); //erase buffer if it exists;
}

WebsocketWriteBuffers::WebsocketWriteBuffers(SystemInterface *_systemWrap) : systemWrap(_systemWrap), mut(), writeBuffer(){}
WebsocketWriteBuffers::~WebsocketWriteBuffers(){
	std::lock_guard<std::mutex> lck(mut); //don't destroy while a thread has a lock
}
