#include "source/server/socket/websocket/websocket_read_buffer.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include <cstring>
#include "source/logging/exception_handler.h"



/*    websocket packet spec
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                               |Masking-key, if MASK set to 1  |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+*/

void unmask (ByteArray::iterator messageStart, ByteArray::iterator messageEnd, ByteArray & out);
uint16_t getNet16bit (ByteArray::iterator num);
uint64_t getNet64bit (ByteArray::iterator num);
int64_t getMessageSize (ByteArray::iterator &messageStart, ByteArray::iterator bufferEnd);



void unmask (ByteArray::iterator messageStart, ByteArray::iterator messageEnd, ByteArray & out) {
	uint64_t begin = out.size(); //save end position of string to begin writing to
	//size_t capacity = begin + length; //new size needed to hold the concatenated strings
	auto signedSize = std::distance(messageStart, messageEnd);
	if( signedSize <= 0) {
		return;
	}
	auto size = static_cast<uint64_t>(signedSize);
	auto capacity = begin + size;
	out.resize(capacity);

	memcpy(&out[begin], &(*(messageStart)), size); //append a masked &in to the end of &out

	uint8_t mask[4];
	//mask is the 4 bytes before messageStart
	mask[0] = *(messageStart-4);
	mask[1] = *(messageStart-3);
	mask[2] = *(messageStart-2);
	mask[3] = *(messageStart-1);

	uint64_t i;
	//unmask output until output[] is 8byte aligned
	for (i = 0; i < size && reinterpret_cast<size_t>(&out[i+begin])%8 != 0; ++i) {
		out[i+begin] ^= mask[i % 4];	//unmask data by 'XOR'ing 4byte blocks with the mask one byte at a time
	}

	//process the rest 64bits at a time
	if(i < size) {
		uint64_t endBytes = (size-i) % 32; //run until there are less than 4 8byte numbers left
		uint64_t length64 = size - endBytes;
		length64 /= 8; //convert length64 from number of bytes to number of 64bit ints

		auto output64 = reinterpret_cast<uint64_t*>(&out[i+begin]);
		uint64_t offset = i;

		uint64_t mask64 = 0;
		auto tempMask = reinterpret_cast<uint8_t*>(&mask64);
		for(int j = 0; j<8; ++j, ++i) {
			tempMask[j] = mask[i % 4]; //build new 64bit mask starting were the previous loop left off (at i)
		}

		for (i = 0; i < length64; i++) {//reset i to zero and start unmasking at the output64 pointer
			output64[i]   ^= mask64;	//unmask data by 64bit 'XOR'ing
			output64[++i] ^= mask64;
			output64[++i] ^= mask64;
			output64[++i] ^= mask64;
		}

		offset += i * 8; //unmask the last remaining bits
		endBytes += offset;
		for(i = offset; i < endBytes; i++) {
			out[i+begin] ^= mask[i % 4];
		}
	}
}

uint16_t getNet16bit (ByteArray::iterator num)
{
	uint8_t out[2];
	uint16_t rval;

	out[0] = *(num + 1);
	out[1] = *(num + 0);
	memcpy(&rval, &out, sizeof(uint16_t));
	return rval;
}

uint64_t getNet64bit (ByteArray::iterator num)
{
	uint8_t out[8];
	uint64_t rval;

	out[0] = *(num + 7);
	out[1] = *(num + 6);
	out[2] = *(num + 5);
	out[3] = *(num + 4);
	out[4] = *(num + 3);
	out[5] = *(num + 2);
	out[6] = *(num + 1);
	out[7] = *(num + 0);
	memcpy(&rval, &out, sizeof(uint64_t));
	return rval;
}




int64_t getMessageSize (ByteArray::iterator &messageStart, ByteArray::iterator bufferEnd) {
	auto bufferSize = std::distance(messageStart, bufferEnd);

	// header+mask takes 8. Message length is at least 126
	const int64_t minSize16 = 134;
	// header+mask takes 14. Message length is at least 65536
	const int64_t minSize64 = 65550;
	// header+mask takes 6
	const uint64_t messageOffsetSmall = 6;
	// header+mask takes 8
	const uint64_t messageOffset16 = 8;
	// header+mask takes 14
	const uint64_t messageOffset64 = 14;
	// if the size byte is 126 or 127 then size is stored in a 16 or 64 bit value
	const uint64_t sizeFlag16 = 126;
	const uint64_t sizeFlag64 = 127;

	const uint64_t highBitMask = 128;

	if(bufferSize < 1) {
		// message is too small
		return -1;
	}

	// save start position of message, we will update messageStart to a point after the size.
	auto beginOfSize = messageStart;


	// payload size is second byte
	++beginOfSize;
	// xor the size with bitmask to unset the maskbit (maskbit is not part of size).
	uint64_t size = *(beginOfSize)^highBitMask;

	// if mask bit was zero
	if(size >= highBitMask) {
		throw std::runtime_error(LOG_EXCEPTION("maskbit was not set. " + " bufferSize: " + std::to_string(bufferSize) + \
		 ". First bit " + std::to_string(static_cast<unsigned int>( *(messageStart) )) )); //maskbit was not set
	}

	// if small measage
	if(size < sizeFlag16) {
		// move messageStart to begining of data
		std::advance(messageStart, messageOffsetSmall);
	}
	// if 16 bit size
	else if(size == sizeFlag16) {
		if(bufferSize < minSize16) {
			// ByteArray too small to read any data
			return -1;
		}
		// size begins after size byte for 16bit
		++beginOfSize;
		size = getNet16bit(beginOfSize);
		// move messageStart to begining of data
		std::advance(messageStart, messageOffset16);
	}
	// if 64 bit size
	else if (size == sizeFlag64) {
		if(bufferSize < minSize64) {
			// ByteArray too small to read any data
			return -1;
		}
		// size begins after size byte for 64bit
		++beginOfSize;
		size = getNet64bit(beginOfSize);
		// move messageStart to begining of data
		std::advance(messageStart, messageOffset64);
	}
	return static_cast<int64_t>(size);
}

void WebsocketReadBuffers::eraseBuffers(int index)
{
	std::lock_guard<std::mutex> lck(mut);
	messages.erase(index); //erase buffer;
}

WebsocketReadBuffers::WebsocketReadBuffers(SetOfFileDescriptors *FDs, size_t size):

	maxMessageSize(size),
	mut(),
	fileDescriptors(FDs),
	messages()
{}
WebsocketReadBuffers::~WebsocketReadBuffers()
{
	std::lock_guard<std::mutex> lck(mut); //don't destroy while a thread has a lock
}


std::vector<SocketMessage> WebsocketReadBuffers::extractMessages(ByteArray &in, int FD) {
	std::lock_guard<std::mutex> lck(mut);
	if(! fileDescriptors->isFDOpen(FD)){ //check to see if FD is in list
		return std::vector<SocketMessage>();
	}
	return messages[FD].extractCompletedMessages(in, FD, maxMessageSize, fileDescriptors);
}


// change to only use one buffer for whole message.
std::vector<SocketMessage> WebsocketReadBuffers::MessageBuffer::extractCompletedMessages(ByteArray &in, int FD, size_t maxMessageSize, SetOfFileDescriptors *fileDescriptors) {
	std::vector<SocketMessage> current_messages;

	workingMessage.append(in);
	auto fractureOffset = workingMessage.begin();
	auto startOfPayload = workingMessage.begin();

	if(expectedSize == -1) {
		expectedSize = getMessageSize (startOfPayload, workingMessage.end());
	}
	// if complete fracture
	while(expectedSize > -1 && std::distance(startOfPayload, workingMessage.end()) >= expectedSize) {

		if( static_cast<uint64_t>(expectedSize) + completeFragments.size() > maxMessageSize) {
			throw std::runtime_error(LOG_EXCEPTION("Message too large.  Size: " + std::to_string(expectedSize) ));
		}

		::unmask (startOfPayload, (startOfPayload + expectedSize), completeFragments);
		auto opcode = static_cast<uint8_t>(*(fractureOffset) & 0x0F); //take low 4 bits of first byte
		if(opcode != 0) {
			currentOpcode = opcode;
		}

		// if FIN
		if(*(fractureOffset) >= 128) {
			current_messages.emplace_back(SocketMessage());
			ByteArray key = fileDescriptors->getCSRFkey(FD);
			ByteArray IP = fileDescriptors->getIP(FD);
			ByteArray port = fileDescriptors->getPort(FD);
			uint32_t type = currentOpcode;
			if( currentOpcode == 2 ) {
				type = completeFragments.getNextUint32();
			}
			uint32_t priority = 0;
			current_messages[current_messages.size()-1].setMessage( FD, currentOpcode, type, priority, IP, port, key, std::move(completeFragments) );
			completeFragments = ByteArray();

			if (currentOpcode == 8){
				// client closed connection (close control opcode)
				// don't process more messeges
				return current_messages;
			} else if(currentOpcode != 1 && currentOpcode != 2 ){
				LOG_ERROR("message type was " << currentOpcode << " on FD " << FD );
			}
		}

		// set offset to next message
		fractureOffset = (startOfPayload + expectedSize);
		startOfPayload = fractureOffset;

		expectedSize = getMessageSize (startOfPayload, workingMessage.end());

	}

	// remove processed messages from workingMessage buffer
	auto leftOver = std::distance(fractureOffset, workingMessage.end());
	if( leftOver > 0) {
		auto bytesLeft = static_cast<size_t>(leftOver);
		memmove(&(*(workingMessage.begin())),&(*(fractureOffset)), bytesLeft);
		workingMessage.resize(bytesLeft);
	} else {
		workingMessage.clear();
	}
	expectedSize = -1;

	return current_messages;
}

