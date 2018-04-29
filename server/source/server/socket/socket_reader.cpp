#include "source/server/socket/socket_reader.h"
#include <csignal>
#include <atomic>
#include <sys/epoll.h>
#include <sys/socket.h>
#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/message_queue.h"
#include "source/server/socket/system_wrapper.h"
#include "source/data_types/socket_message.h"
#include "source/server/socket/websocket/websocket_message_processor.h"
#include "source/data_types/byte_array.h"
#include "source/logging/exception_handler.h"


SocketReader::SocketReader(SystemInterface *_systemWrap, SetOfFileDescriptors *_FDs, std::atomic<bool>* run) : SocketNode(_systemWrap, _FDs, run),
																										processor( new WebsocketMessageProcessor(_FDs) ),
																										readerQueue(nullptr),
																										waitingFDs(), waitingMut(),
																										maxBufferSize(32760)
																										{
	readerQueue = new MessageQueue(running);
	processor->setReaderQueue(readerQueue);
	fileDescriptors->addNewConnectionCallback(std::bind(&SocketReader::newConnectionHandler,this, std::placeholders::_1));
	fileDescriptors->addCloseFDCallback(std::bind(&SocketReader::closeFDHandler,this, std::placeholders::_1));
	signal(SIGPIPE, SIG_IGN); // ignore error when writing to closed sockets to prevent crash on client disconnect
}


SocketReader::~SocketReader(){
	if(readerQueue != nullptr ) delete readerQueue;
	readerQueue = nullptr;
}


SocketMessage SocketReader::getNextMessage(){  // blocks thread while queue is empty
	return readerQueue->getNextMessage_Blocking();//getNextMessage();
}


void SocketReader::shutdown(){
	readerQueue->shutdown(); // unblocks getNextMessage() if it's blocking and sets running to false--ending the event loop.
}


void SocketReader::processSockMessage(ByteArray &in,  int FD){
	processor->processSockMessage(in, FD);
}

int SocketReader::getWaitTime() {
	int waitTime = 0;
	{ // lock
	std::lock_guard<std::recursive_mutex> lck(waitingMut);
	if(waitingFDs.empty())waitTime = SocketNode::getWaitTime();	// sleep for awhile during epollWait if no other work is to be done
	} // unlock
	return waitTime;
}

void SocketReader::handleEpollRead(epoll_event &event){
	if((event.events & EPOLLIN)) {			//the socket is ready for reading
		addToWaitingFDs(event.data.fd);
	}
	readChunk();
}

// unused
void SocketReader::handleEpollWrite(epoll_event &event) {
	(void)event;
}


void SocketReader::setupEpoll(){
	SocketNode::setupEpoll();
	LOG_INFO("Reader epollFD is "<<epollFD);
}

void SocketReader::closeFDHandler(int FD){
	LOG_INFO("closed connection on FD "<<FD);
	try{
		fileDescriptors->stopPollingFD(epollFD, FD);
	}
	catch(std::runtime_error &ret) {
		BACKTRACE_PRINT();
		writeError("epoll_ctl");
	}
	removeFromWaitingFDs(FD);
	processor->closeFDHandler(FD);
}

void SocketReader::newConnectionHandler(int FD){
	LOG_INFO("New connection on FD "<<FD);
	try{
		fileDescriptors->startPollingForRead(epollFD, FD);
	}
	catch(std::runtime_error &ret) {
		BACKTRACE_PRINT();
		LOG_ERROR("threw while calling fileDescriptors->startPollingForRead("<<epollFD<<", "<<FD<<")");
		closeFD(FD);
	}
}

void SocketReader::readChunk(){
	std::lock_guard<std::recursive_mutex> lck(waitingMut);
	std::vector<int> closeList;
	std::vector<int> removeList;
	for(auto FD : waitingFDs){
		try{
			bool done = readChunkFromFD(FD);
			if(done) removeList.push_back(FD);
		}
		catch(std::runtime_error &ret){
			BACKTRACE_PRINT();
			LOG_ERROR("threw while calling readChunkFromFD("<<FD<<")");
			closeList.push_back(FD); // drop connection on failure
		}
		if(waitingFDs.empty()){
			return;
		}
	}
	for(auto FD : closeList){
		LOG_ERROR("FD "<<FD<<"added to closeList and closed");
		closeFD(FD);
	}
	for(auto FD : removeList){
		removeFromWaitingFDs(FD);
	}
}

bool SocketReader::readChunkFromFD(int FD){
	{ // lock
	std::lock_guard<std::recursive_mutex> lck(waitingMut);
	if(waitingFDs.count(FD) == 0){
		throw std::runtime_error(LOG_EXCEPTION("invalid FD "+std::to_string(FD)));
	}
	} // unlock

	ByteArray buffer(maxBufferSize);
	bool done;
	size_t count = systemWrap->readFD(FD, &buffer[0], maxBufferSize, done);
	if(done){ // if we read all data.
		return true; // removeFromWaitingFDs(FD);
	}

	else if (count == 0){ // End of file. Remote closed connection.
		throw std::runtime_error(LOG_EXCEPTION("client closed connection disconnected"));
	}
	else {
		buffer.resize(count);
		processor->processSockMessage(buffer,  FD);
	}
	return false;
}

void SocketReader::addToWaitingFDs(int FD){
	std::lock_guard<std::recursive_mutex> lck(waitingMut);
	if(FD != -1 && waitingFDs.count(FD) == 0) {
		waitingFDs.insert(FD);
	}
}

void SocketReader::removeFromWaitingFDs(int FD){
	std::lock_guard<std::recursive_mutex> lck(waitingMut);
	if(waitingFDs.count(FD) != 0){
		waitingFDs.erase(FD);
	}
}
