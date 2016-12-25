#include "source/server/socket/websocket/websocket_message_processor.h"
#include "source/server/socket/websocket/websocket_read_buffer.h"
#include "source/server/socket/message_queue.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "source/data_types/socket_message.h"
#include "source/data_types/message_type.h"
#include <string.h>
#include "source/logging/exception_handler.h"

WebsocketMessageProcessor::WebsocketMessageProcessor(SetOfFileDescriptors *_FDs) : readerQueue(NULL), MaxReadBufferSize(999999999), ReadBuffers( new WebsocketReadBuffers(_FDs, MaxReadBufferSize) ), fileDescriptors(_FDs){}
WebsocketMessageProcessor::~WebsocketMessageProcessor(){}

MessageQueue * WebsocketMessageProcessor::getQueue(){
	return readerQueue;
}
void WebsocketMessageProcessor::setReaderQueue(MessageQueue *_readerQueue){
	readerQueue = _readerQueue;
}

void WebsocketMessageProcessor::closeFDHandler(int FD){
	//add message to queue to indicate client has disconnected
	SocketMessage newMessage;
	ByteArray key = fileDescriptors->getCSRFkey(FD);
	ByteArray IP = fileDescriptors->getIP(FD);
	ByteArray port = fileDescriptors->getPort(FD);
	uint32_t type = MessageType::logout;
	uint32_t priority = 0;
	ByteArray mess;
	newMessage.setMessage( FD, type, priority, IP, port, key, std::move(mess) );
	readerQueue->pushMessage(newMessage); //send logout message
	//clean buffers client was using
	ReadBuffers->eraseBuffers(FD);
}

void WebsocketMessageProcessor::processSockMessage (ByteArray &in,  int FD){
	std::vector< ByteArray > messages;
	std::vector<int> messageTypes;
	size_t r = extractMessage(in, messages, messageTypes, FD);
	if(r != 0){//we got a complete message
		for(size_t i=0; i<r;++i){
			if(messageTypes[i] == 2 || messageTypes[i] == 1){
				SocketMessage newMessage;
				ByteArray key = fileDescriptors->getCSRFkey(FD);
				ByteArray IP = fileDescriptors->getIP(FD);
				ByteArray port = fileDescriptors->getPort(FD);
				uint32_t type = messages[i].getNextUint32();
				uint32_t priority = 0;
				newMessage.setMessage( FD, type, priority, IP, port, key, std::move(messages[i]) );
				readerQueue->pushMessage(newMessage);
			}
			else if (messageTypes[i] == 8){ //client closed connection (close control opcode)
				LOG_INFO("Client closed connection on FD " << FD );
				fileDescriptors->removeFD(FD); 	//remove player and buffers
			}
			else LOG_ERROR("message type was " << messageTypes[i] << " on FD " << FD );
		}
	}
}

size_t WebsocketMessageProcessor::extractMessage (ByteArray &in, std::vector< ByteArray > &out, std::vector<int> &types, int FD){
	types.reserve(10);
	types.push_back(-1);
	out.reserve(10);			//we could calculate how many messages are in 'in' here and reserve the correct number but it would be slow. 10 should cover most cases.
	out.push_back( ByteArray() ); //create first element.

	size_t currentM =0;
	uint64_t start =0;
	uint64_t size =0;
	uint64_t messageStart =0;

	while(size<(in.size()-(messageStart))){
		size_t outStart = out[currentM].size();

		if(!ReadBuffers->messageIsEmpty(FD)){
			bool wholeMessage = ReadBuffers->extractMessage(in, start, FD); //if we aready have part of the message in buffer, prepend it to in
			if(!wholeMessage){
				ByteArray message;
				size_t messageSize = in.size()-start;
				message.resize(messageSize);
				memcpy(&message[0], &in[start], messageSize);
				ReadBuffers->addMessage(FD, message);
				return currentM;
			}
		}

		if(in.size()-start<6){ //string too small to read any data (header+mask takes a min size of 6. Control frame message may be of length 0)
			ByteArray message;
			size_t messageSize = in.size()-start;
			message.resize(messageSize);
			memcpy(&message[0], &in[start], messageSize);
			ReadBuffers->addMessage(FD, message); //add to buffer for processing
			return currentM;
		}

		size = getMessageSize(in, messageStart, start, FD);
		if(size == 0)return currentM;

		if(size>(in.size()-(messageStart))){ //only received partial message;
			ByteArray message;
			size_t messageSize = in.size()-start;
			message.resize(messageSize);
			memcpy(&message[0], &in[start], messageSize);
			ReadBuffers->addMessage(FD, message);
			ReadBuffers->setMessageSize(FD, size);
			return currentM;
		}

		uint8_t opcode = static_cast<uint8_t>(in[start] & 0x0F); //take low 4 bits of first byte
		types[currentM] = opcode;

		unmask(in, out[currentM], messageStart, size);
		if(in[start]<128){ //Fragmented message
			handleFragment(out[currentM], opcode, FD);
		}

		else{ //complete
			if(opcode == 0){
				completeFracture(out[currentM], types[currentM], outStart, FD);
			}
			types.push_back(-1);
			out.push_back(ByteArray() );
			currentM++;
			if(types[currentM-1] == 8){ //close control frame
				return currentM; //don't process further messages
			}
		}
		start = messageStart+size;
	}
	return currentM;
}

uint64_t WebsocketMessageProcessor::getMessageSize (ByteArray &in, uint64_t &messageStart, const uint64_t &start, int FD){
	uint64_t size = in[start+1]^128;
	if(size >= 128){
		throwInt("maskbit was not set on descriptor "<<FD<<" start: "<<start<<" in.size(): "<<in.size()<<". First bit "<<  static_cast<unsigned int>( in[start] ) ); //maskbit was not set
	}
	messageStart = start+6; //start of mask start
	if(size == 126){
		if(in.size()-start<134){//string too small to read any data (header+mask takes a min of 8. Message length is at least 126)
			ByteArray message;
			size_t messageSize = in.size()-start;
			message.resize(messageSize);
			memcpy(&message[0], &in[start], messageSize);
			ReadBuffers->addMessage(FD,message); //add to buffer for processing
			return 0;
		}
		messageStart = start+8;
		size = getNet16bit(&in[start+2]);
	}
	else if (size == 127){
		if(in.size()-start<65550){//string too small to read any data (header+mask takes a min of 14. Message length is at least 65536)
			ByteArray message;
			size_t messageSize = in.size()-start;
			message.resize(messageSize);
			memcpy(&message[0], &in[start], messageSize);
			ReadBuffers->addMessage(FD,message); //add to buffer for processing
			return 0;
		}
		messageStart = start+14;
		size = getNet64bit(&in[start+2]);
	}
	if(size>MaxReadBufferSize){
		throwInt("Message too large on on descriptor "<< FD <<".  Size: "<<size );
	}
	return size;
}

void WebsocketMessageProcessor::completeFracture (ByteArray &out, int &types, size_t position, int FD){
	ReadBuffers->extractFracture(out, position, FD); //if we aready have part of the message in buffer, prepend it to end
	types = ReadBuffers->getFractureType(FD);
	ReadBuffers->eraseFractureType(FD);
}

void WebsocketMessageProcessor::handleFragment (ByteArray &in, uint8_t opcode, int FD){
	if(opcode <= 2){
		if(opcode != 0){ 		//first fragment of message
			if(opcode == 1){	//text
				ReadBuffers->fractureTypeSet(FD, true);
			}
			else if(opcode == 2){ 	//binary
				ReadBuffers->fractureTypeSet(FD, false);
			}
			ReadBuffers->clearFracture(FD);
		}
		ReadBuffers->addFracture(FD,in);
	}
	else{
		throwInt("bad Opcode "<< static_cast<int>(opcode) <<" on descriptor "<<FD);
	}
}

void WebsocketMessageProcessor::unmask (ByteArray &in, ByteArray &out, uint64_t messageStart, uint64_t length){
	//this function is nasty because it represented 97% of execution time. Its now optimised, and ugly, and illegal (strict-aliasing rules are broken)
	if(messageStart < 4) throw -1; //throwInt("Message start position too low to allow for mask");
	if(in.size() < messageStart+length) throw -1; //throwInt("Message size is smaller than reported length");

	uint64_t begin  = out.size(); //save end position of string to begin writing to
	size_t capacity = begin+length; //new size needed to hold the concatenated strings
	out.resize(capacity);

	memcpy(&out[begin], &in[messageStart], length); //append a masked &in to the end of &out

	//volatile qualifiers are needed to prevent the compiler from making wrong assumptions because we are breaking strict-aliasing rules.
	//const volatile	uint8_t	* input  = reinterpret_cast<const volatile uint8_t*>(in.c_str());
	volatile uint8_t * output = reinterpret_cast<volatile uint8_t*>( &out[0]);

	uint8_t mask[4];
	mask[0] = in[messageStart-4];//mask is the 4 bytes before messageStart
	mask[1] = in[messageStart-3];
	mask[2] = in[messageStart-2];
	mask[3] = in[messageStart-1];

	uint64_t i;
	for (i = 0; i < length && reinterpret_cast<size_t>(&output[i+begin])%8 != 0; ++i){ //unmask output until output[] is 8byte aligned
		output[i+begin] ^= mask[i % 4];	//unmask data by 'XOR'ing 4byte blocks with the mask one byte at a time
	}

	if(i < length){ //process the rest 64bits at a time
		uint64_t endBytes = (length-i) % 32; //run untill there are less than 4 8byte numbers left
		uint64_t length64 = length-endBytes;
		length64 /= 8; //convert length64 from number of bytes to number of 64bit ints

		volatile uint64_t *output64 = reinterpret_cast<volatile uint64_t*>(&output[i+begin]);
		uint64_t offset= i;

		uint64_t mask64 = 0;
		volatile uint8_t *tempMask = reinterpret_cast<volatile uint8_t*>(&mask64);
		for(int j = 0; j<8; ++j, ++i){
			tempMask[j] = mask[i % 4]; //build new 64bit mask starting were the previous loop left off (at i)
		}

		for (i = 0; i < length64; i++){//reset i to zero and start unmasking at the output64 pointer
			output64[i]   ^= mask64;	//unmask data by 64bit 'XOR'ing
			output64[++i] ^= mask64;
			output64[++i] ^= mask64;
			output64[++i] ^= mask64;
		}

		offset += i*8; //unmask the last remaining bits
		endBytes += offset;
		for(i = offset; i<endBytes; i++){
			output[i+begin] ^= mask[i % 4];
		}
	}
}

uint64_t WebsocketMessageProcessor::getNet64bit (uint8_t *in){ //converts a 64bit array in network byte order into a little endian unsigned 64bit int
	uint8_t out[8];
	uint64_t rval;

	out[0]= in[7];
	out[1]= in[6];
	out[2]= in[5];
	out[3]= in[4];
	out[4]= in[3];
	out[5]= in[2];
	out[6]= in[1];
	out[7]= in[0];

	uint64_t *temp = reinterpret_cast<uint64_t *>(out);
	rval = *temp;
	return rval;
}

uint16_t WebsocketMessageProcessor::getNet16bit (uint8_t *in){ //converts a 16bit array in network byte order into a little endian unsigned 16bit int
	uint8_t out[2];
	uint16_t rval;

	out[0]= in[1];
	out[1]= in[0];

	uint16_t *temp = reinterpret_cast<uint16_t *>(out);
	rval = *temp;
	return rval;
}
