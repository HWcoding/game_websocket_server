#include "source/server/socket/websocket/websocket_message_sender.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/websocket/websocket_write_buffer.h"
#include "source/data_types/socket_message.h"
#include "source/server/socket/system_wrapper.h"
#include "source/logging/exception_handler.h"


WebsocketMessageSender::~WebsocketMessageSender() = default;

WebsocketMessageSender::WebsocketMessageSender() :
		writeBuffers( new WebsocketWriteBuffers() ),
		MaxWriteBufferSize(2097152){}


void WebsocketMessageSender::addMessage(SocketMessage &message)
{
	int FD = message.getFD();
	ByteArray messageValue = message.getMessage();
	ByteArray frameHeader = createFrameHeader(messageValue.size(), 2); //2 for binary
	messageValue.insert(messageValue.begin(), frameHeader.begin(), frameHeader.end());
	writeBuffers->addMessage(FD, messageValue);
}



bool WebsocketMessageSender::writeData(int FD)
{
	size_t writeAmount = writeBuffers->messageSize(FD);

	if(writeAmount>MaxWriteBufferSize){
		throw std::runtime_error(LOG_EXCEPTION("writeAmount > MaxWriteBufferSize on descriptor "+ \
			std::to_string(static_cast<unsigned int>(FD))+". MaxWriteBufferSize: "+std::to_string(MaxWriteBufferSize)));
	}
	return writeBuffers->writeData(FD);
}


void WebsocketMessageSender::closeFDHandler(int FD)
{
	writeBuffers->eraseBuffers(FD);
}


ByteArray WebsocketMessageSender::createFrameHeader(size_t frameSize, uint8_t opcode, bool finished)
{
	size_t size;


	uint8_t fin = 0;
	// if this is the last frame in the message the header has a 1 in the high bit
	if(finished) {
		fin = 128; // 1000 0000
	}
	else {
		if(opcode != 0 && opcode != 2 && opcode != 1){
			throw std::runtime_error(LOG_EXCEPTION("message fragmentation only available with opcode 0x1, and 0x2"));
		}
	}

	// the first byte is fin followed by the 4-bit opcode
	auto firstByte = static_cast <uint8_t>(fin + opcode);

	// the second byte is the size of the message if it is less than or equal to
	// 125, or if its larger, a magic number indicating the magnitude of the
	// size. In the second case, the size is stored in the bytes that follow the
	// magic number.
	uint8_t secondByte;

	if(frameSize>65535){
		// too big, the header is going to need 10 bytes to store the magic
		// number, 8-Byte size, and opcode. The magic number is 127
		size = 10;
		secondByte = 127;
	}
	else if(frameSize>125){
		// too big, the header is going to need 4 bytes to store the magic
		// number, 2-Byte size, and opcode. The magic number is 126
		size = 4;
		secondByte = 126;
	}
	else {
		// the size and opcode can fit in just 2 bytes. The second byte is
		// the size.
		size = 2;
		secondByte = static_cast <uint8_t>(frameSize);
	}

	ByteArray buffer(size);
	buffer[0] = firstByte;
	buffer[1] = secondByte;
	// write the size to the remaining bytes in network byte order if size > 2
	for(size_t i = 2, shiftAmount = (size - 3) * 8; i < size; ++i, shiftAmount -= 8){
		buffer[i] = static_cast <uint8_t>(frameSize >> shiftAmount);
	}

	return buffer;
}
