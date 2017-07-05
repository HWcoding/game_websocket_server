#include "source/server/socket/websocket/websocket_message_sender.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/websocket/websocket_write_buffer.h"
#include "source/data_types/socket_message.h"
#include "source/server/socket/system_wrapper.h"
#include "source/logging/exception_handler.h"

MessageSenderInterface::~MessageSenderInterface() = default;


WebsocketMessageSender::~WebsocketMessageSender() = default;

WebsocketMessageSender::WebsocketMessageSender(SystemInterface *_systemWrap) : writeBuffers( new WebsocketWriteBuffers(_systemWrap) ), MaxWriteBufferSize(1999999999){}

WebsocketMessageSender::WebsocketMessageSender(WebsocketWriteBuffers *_writeBuffers ) :
                                       writeBuffers( _writeBuffers ), MaxWriteBufferSize(1999999999){}

void WebsocketMessageSender::addMessage(SocketMessage &message)
{
	int FD = message.getFD();
	ByteArray messageValue = message.getMessage();
	ByteArray frameHeader = createFrameHeader(messageValue, 2); //2 for binary
	messageValue.insert(messageValue.begin(), frameHeader.begin(), frameHeader.end());
	writeBuffers->addMessage(FD, messageValue);
}

bool WebsocketMessageSender::writeData(int FD)
{
	size_t writeAmount = writeBuffers->messageSize(FD);

	if(writeAmount>MaxWriteBufferSize){
		throwInt("writeAmount > MaxWriteBufferSize on descriptor "<< static_cast<unsigned int>(FD)<< ". MaxWriteBufferSize: "<<MaxWriteBufferSize);
	}
	return writeBuffers->writeData(FD);
}

void WebsocketMessageSender::closeFDHandler(int FD)
{
	writeBuffers->eraseBuffers(FD);
}

ByteArray WebsocketMessageSender::createFrameHeader(const ByteArray &in, uint8_t opcode)
{
	size_t size = 2;

	if(in.size()>65535){
		size = 10;
	}
	else if(in.size()>125){
		size = 4;
	}

	ByteArray buffer;
	buffer.resize(size);
	buffer[0] = static_cast <uint8_t>(128 + opcode);
	if(size == 2){
		buffer[1] = static_cast <uint8_t>(in.size());
	}
	else if(size == 4){
		uint64_t isize = in.size();
		uint64_t itsize;
		buffer[1] = static_cast <uint8_t>(126);
		buffer[2] = static_cast <uint8_t>(isize>>8);
		itsize    = (isize<<8);
		buffer[3] = static_cast <uint8_t>(itsize>>8);
	}
	else if(size == 10){
		uint64_t isize = in.size();
		uint64_t itsize;
		buffer[1] = static_cast <uint8_t>(127);
		buffer[2] = static_cast <uint8_t>(isize>>56);
		for(size_t i=3, j=8; i<10; ++i,j+=8){
			itsize = (isize<<j);
			buffer[i] = static_cast <uint8_t>(itsize>>56);
		}
	}
	return buffer;
}
