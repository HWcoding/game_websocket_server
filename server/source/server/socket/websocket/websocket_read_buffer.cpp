#include "source/server/socket/websocket/websocket_read_buffer.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include <cstring>
#include "source/logging/exception_handler.h"



void WebsocketReadBuffers::addMessage(ByteArray &in, int index ) {
	std::lock_guard<std::mutex> lck(mut);
	PartialMessage& tempBuff = messageBuffer[index];
	if(in.size()+tempBuff.size() <= maxMessageSize) {
		//tempBuff.increaseSize(in.size());
		//tempBuff.buffer.push_back(std::move(in));
		tempBuff.addPartialMessage(ByteArray &in, 0);
	}
	else {
		throw std::runtime_error(LOG_EXCEPTION("Message expected size too large. Expectedsize: "+std::to_string(tempBuff.expectedSize)));
	}
}

bool WebsocketReadBuffers::extractMessage(ByteArray &out, size_t position, int index)
{
	std::lock_guard<std::mutex> lck(mut);
	auto fdBuffer = currentMessage[index];
	ByteArray message= fdBuffer.extractPartialMessage();
	if(message.size() > 0) {
		out.insert( out.begin() + static_cast<int64_t>(position), message.begin(), message.end() );
		return true;
	}
	return false;
}
bool WebsocketReadBuffers::messageIsEmpty(int index)
{
	std::lock_guard<std::mutex> lck(mut);
	auto fdBuffer = currentMessage[index];
	return fdBuffer.buffer.empty() || fdBuffer.buffer.front().empty();
}

void WebsocketReadBuffers::eraseBuffers(int index)
{
	std::lock_guard<std::mutex> lck(mut);
	messageBuffer.erase(index); //erase buffer;
}

WebsocketReadBuffers::WebsocketReadBuffers(SetOfFileDescriptors *FDs, size_t size):

	maxMessageSize(size),
	mut(),
	fileDescriptors(FDs),
	messageBuffer(),
	fractureBuffer(),
	fractureBufferType()

{}
WebsocketReadBuffers::~WebsocketReadBuffers()
{
	std::lock_guard<std::mutex> lck(mut); //don't destroy while a thread has a lock
}

/*void WebsocketReadBuffers::addMessage(int index, ByteArray &in)
{
	std::lock_guard<std::mutex> lck(mut);
	if(fileDescriptors->isFDOpen(index)) {
		PartialMessage& tempBuff = messageBuffer[index];
		if(in.size()+tempBuff.size() <= maxMessageSize) {
			tempBuff.increaseSize(in.size());
			tempBuff.buffer.push_back(std::move(in));
		}
		else {
			throw std::runtime_error(LOG_EXCEPTION("Message expected size too large. Expectedsize: "+std::to_string(tempBuff.expectedSize)));
		}
	}
}


bool WebsocketReadBuffers::messageIsEmpty(int index)
{
	std::lock_guard<std::mutex> lck(mut);
	return (messageBuffer.count(index) == 0);
}


void WebsocketReadBuffers::setMessageSize(int index, size_t _size)
{
	if(messageBuffer.count(index) != 0) {
		messageBuffer[index].expectedSize = static_cast<int64_t>(_size);
	}
}


bool WebsocketReadBuffers::extractMessage(ByteArray &out, size_t position, int index)
{
	std::lock_guard<std::mutex> lck(mut);
	if(messageBuffer.count(index) != 0) {
		PartialMessage& tempBuff = messageBuffer[index];
		if(!tempBuff.buffer.empty()) {
			if(out.size() + tempBuff.size() <= maxMessageSize) {
				if( tempBuff.expectedSize > static_cast<int64_t>(maxMessageSize) ) {
					throw std::runtime_error(LOG_EXCEPTION("Message expected size too large. Expectedsize: " + std::to_string(tempBuff.expectedSize)));
				}
				if( static_cast<int64_t>( out.size() + tempBuff.size() ) >= tempBuff.expectedSize ) {
					ByteArray message;
					if(tempBuff.expectedSize > 0) {
						message.reserve( static_cast<size_t>(tempBuff.expectedSize) );
					}
					for(auto element : tempBuff.buffer) {
						size_t messageSize = message.size();
						message.resize(messageSize + element.size());
						memcpy( &message[messageSize], &element[0], element.size() );
					}
					messageBuffer.erase(index);
					out.insert( out.begin() + static_cast<int64_t>(position), message.begin(), message.end() ); //add message buffer to out;
					return true;
				}
			}
			else {
				throw std::runtime_error(LOG_EXCEPTION("Client sent too much data.  Size: " + std::to_string(out.size() + messageBuffer[index].size()) ));
			}
		}
	}
	return false;
}


void WebsocketReadBuffers::addFracture(int index, ByteArray &in)
{
	std::lock_guard<std::mutex> lck(mut);
	if(fileDescriptors->isFDOpen(index)){
		Fracture& tempBuff = fractureBuffer[index];
		tempBuff.increaseSize(in.size());
		tempBuff.buffer.push_back(std::move(in));
		in.clear();
	}
}


void WebsocketReadBuffers::extractFracture( ByteArray &out, size_t position, int index)
{
	std::lock_guard<std::mutex> lck(mut);
	if(fractureBuffer.count(index) != 0) {
		Fracture& tempBuff = fractureBuffer[index];
		if(tempBuff.size() != 0) {
			if(out.size()+tempBuff.size() <= maxMessageSize) {
				ByteArray fractureContents;
				fractureContents.reserve(tempBuff.size());
				for(auto element : tempBuff.buffer) {
					size_t messageSize = fractureContents.size();
					fractureContents.resize(messageSize + element.size());
					memcpy( &fractureContents[messageSize], &element[0], element.size() );
					element.clear();
				}
				out.insert( out.begin()+static_cast<int64_t>(position), fractureContents.begin(), fractureContents.end() );
				fractureBuffer.erase(index); //erase buffer;
			}
		}
	}
}


void WebsocketReadBuffers::clearFracture(int index)
{
	std::lock_guard<std::mutex> lck(mut);
	if(fractureBuffer.count(index) != 0) {
		for(auto element : fractureBuffer[index].buffer) {
			element.clear();
		}
	}
}


void WebsocketReadBuffers::fractureTypeSet(int index, bool flag)
{
	std::lock_guard<std::mutex> lck(mut);
	if(fileDescriptors->isFDOpen(index)) {
		fractureBufferType[index] = flag;
	}
}


bool WebsocketReadBuffers::getFractureType(int index)
{
	std::lock_guard<std::mutex> lck(mut);
	if(fractureBufferType.count(index) != 0) {
		return fractureBufferType[index];
	}
	else return false;
}


void WebsocketReadBuffers::eraseFractureType(int index)
{
	std::lock_guard<std::mutex> lck(mut);
	if(fractureBufferType.count(index) != 0) {
		fractureBufferType.erase(index); //erase buffer;
	}
}


void WebsocketReadBuffers::eraseBuffers(int index)
{
	std::lock_guard<std::mutex> lck(mut);
	messageBuffer.erase(index); //erase buffer;
	fractureBuffer.erase(index); //erase buffer;
	fractureBufferType.erase(index); //erase buffer;
}


WebsocketReadBuffers::WebsocketReadBuffers(SetOfFileDescriptors *FDs, size_t size) :
	maxMessageSize(size),
	mut(),
	fileDescriptors(FDs),
	messageBuffer(),
	fractureBuffer(),
	fractureBufferType()
{}


WebsocketReadBuffers::~WebsocketReadBuffers()
{
	std::lock_guard<std::mutex> lck(mut); //don't destroy while a thread has a lock
}
*/