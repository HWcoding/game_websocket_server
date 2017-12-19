#include "source/server/socket/websocket/websocket_message_sender.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/websocket/websocket_write_buffer.h"
#include "source/data_types/socket_message.h"
#include "source/server/socket/system_wrapper.h"
#include "source/logging/exception_handler.h"

MessageSenderInterface::~MessageSenderInterface() = default;


WebsocketMessageSender::~WebsocketMessageSender() = default;

WebsocketMessageSender::WebsocketMessageSender(SystemInterface *_systemWrap) :
		writeBuffers( new WebsocketWriteBuffers(_systemWrap) ),
		MaxWriteBufferSize(2097152){}

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
		throw std::runtime_error(LOG_EXCEPTION("writeAmount > MaxWriteBufferSize on descriptor "+ \
			std::to_string(static_cast<unsigned int>(FD))+". MaxWriteBufferSize: "+std::to_string(MaxWriteBufferSize)));
	}
	return writeBuffers->writeData(FD);
}

void WebsocketMessageSender::closeFDHandler(int FD)
{
	writeBuffers->eraseBuffers(FD);
}

ByteArray WebsocketMessageSender::createFrameHeader(const ByteArray &in, uint8_t opcode)
{
	size_t size;

	// the first byte has a 1 in the high bit followed by the opcode
	uint8_t firstByte = static_cast <uint8_t>(128 + opcode);

	// the second byte is the size of the message if it is less than or equal to
	// 125, or if it larger, a magic number indicating the magnitude of the
	// size. In the second case, the size is stored in the bytes that follow.
	uint8_t secondByte;

	if(in.size()>65535){
		// too big, the header is going to need 10 bytes to store the magic
		// number, 8-Byte size, and opcode. The magic number is 127
		size = 10;
		secondByte = 127;
	}
	else if(in.size()>125){
		// too big, the header is going to need 4 bytes to store the magic
		// number, 2-Byte size, and opcode. The magic number is 126
		size = 4;
		secondByte = 126;
	}
	else {
		// the size and opcode can fit in just 2 bytes. The second byte is
		// the size.
		size = 2;
		secondByte = static_cast <uint8_t>(in.size());
	}

	ByteArray buffer;
	buffer.resize(size);
	buffer[0] = firstByte;
	buffer[1] = secondByte;

	// write the size to the remaining bytes in network byte order if size > 2
	uint64_t currentByte;
	size_t shiftAmount = (size - 3)*8;
	for(size_t i=2, j=0; i<size; ++i, j += 8){
		currentByte = (in.size()<<j)>>shiftAmount;
		buffer[i] = static_cast <uint8_t>(currentByte);
	}

	return buffer;
}
