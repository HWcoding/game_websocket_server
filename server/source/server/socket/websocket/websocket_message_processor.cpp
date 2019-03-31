#include "source/server/socket/websocket/websocket_message_processor.h"
#include "source/server/socket/websocket/websocket_read_buffer.h"
#include "source/server/socket/message_queue.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "source/data_types/socket_message.h"
#include "source/data_types/message_type.h"
#include <cstring>
#include "source/logging/exception_handler.h"


WebsocketMessageProcessor::WebsocketMessageProcessor(SetOfFileDescriptors *_FDs) :
	readerQueue(nullptr),
	MaxReadBufferSize(999999999),
	ReadBuffers( new WebsocketReadBuffers(_FDs, MaxReadBufferSize) ),
	fileDescriptors(_FDs)
{}


WebsocketMessageProcessor::~WebsocketMessageProcessor() = default;


MessageQueue * WebsocketMessageProcessor::getQueue()
{
	return readerQueue;
}


void WebsocketMessageProcessor::setReaderQueue(MessageQueue *_readerQueue)
{
	readerQueue = _readerQueue;
}


void WebsocketMessageProcessor::closeFDHandler(int FD)
{
	//add message to queue to indicate client has disconnected
	SocketMessage newMessage;
	ByteArray key = fileDescriptors->getCSRFkey(FD);
	ByteArray IP = fileDescriptors->getIP(FD);
	ByteArray port = fileDescriptors->getPort(FD);
	uint32_t type = MessageType::logout;
	uint32_t priority = 0;
	ByteArray mess;
	newMessage.setMessage( FD, 8, type, priority, IP, port, key, std::move(mess) );

	//send logout message
	readerQueue->pushMessage(newMessage);
	//clean buffers client was using
	ReadBuffers->eraseBuffers(FD);
}


void WebsocketMessageProcessor::processSockMessage (ByteArray &in,  int FD)
{
	std::vector<SocketMessage> messages = ReadBuffers->extractMessages(in, FD);
	for(auto message : messages) {
		if (message.getOpcode() != 8){
			readerQueue->pushMessage(message);
		} else {
			LOG_INFO("Client closed connection on FD " << FD );
			//remove player and buffers.
			fileDescriptors->removeFD(FD);

		}
	}
}
