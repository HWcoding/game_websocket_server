#include "source/server/socket/socket_reader.h"
#include <signal.h>
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


SocketReader::SocketReader(SystemInterface *_systemWrap, SetOfFileDescriptors *_FDs, std::atomic<bool>* run) : systemWrap(_systemWrap),
																										processor( new WebsocketMessageProcessor(_FDs) ),
																										running(run), fileDescriptors(_FDs), readerQueue(nullptr),
																										waitingFDs(), waitingMut(),
																										epollFD(-1), MAXEVENTS(9999), maxBufferSize(32760)
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


void SocketReader::startPoll(){
	setupEpoll();
	std::vector<epoll_event> events;
	events.resize(static_cast<size_t>(MAXEVENTS) );

	while(running->load()){ // The event loop
		int waitTime = 0;
		{ // lock
		std::lock_guard<std::recursive_mutex> lck(waitingMut);
		if(waitingFDs.empty())waitTime = 1000;	// sleep for awhile during epollWait if no other work is to be done
		} // unlock
		size_t num = systemWrap->epollWait(epollFD, &events[0], MAXEVENTS, waitTime);
		for (size_t i = 0; i < num; ++i){
			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)){ // An error occured
				int error = 0;
				socklen_t errlen = sizeof(error);
				if (systemWrap->getSockOpt(events[i].data.fd, SOL_SOCKET, SO_ERROR, reinterpret_cast<void *>(&error), &errlen) == 0){
					LOG_ERROR("epoll error on FD "<<events[i].data.fd<<". Error: "<<strerror(error));
				}
				else LOG_ERROR("epoll error");
				closeFD(events[i].data.fd);
			}
			else if((events[i].events & EPOLLIN)){	// the socket is ready for reading
				addToWaitingFDs(events[i].data.fd);
			}
			else LOG_ERROR("FD "<<events[i].data.fd<< " was not EPOLLIN "<< events[i].events);
		}
		readChunk();
	}
}

void SocketReader::setupEpoll(){
	epollFD = systemWrap->epollCreate(0);
	LOG_INFO("Reader epollFD is "<<epollFD);
}


void SocketReader::closeFD(int FD){
	int ret = fileDescriptors->removeFD(FD);
	if(ret == -1)LOG_ERROR(" File descriptor "<<FD<<"failed to close properly. ");
}

void SocketReader::closeFDHandler(int FD){
	LOG_INFO("closed connection on FD "<<FD);
	try{ fileDescriptors->stopPollingFD(epollFD, FD); }
	catch(int &ret) {
		BACKTRACE_PRINT();
		writeError("epoll_ctl");
	}
	removeFromWaitingFDs(FD);
	processor->closeFDHandler(FD);
}

void SocketReader::newConnectionHandler(int FD){
	LOG_INFO("New connection on FD "<<FD);
	try{ fileDescriptors->startPollingForRead(epollFD, FD); }
	catch(int &ret) {
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
		catch(int &ret){
			BACKTRACE_PRINT();
			LOG_ERROR("threw while calling readChunkFromFD("<<FD<<")");
			closeList.push_back(FD); // drop connection on failure
		}
		if(waitingFDs.empty()) return;
	}
	for(auto FD : closeList){
		LOG_ERROR("FD "<<FD<<"added to closeList and closed");
		closeFD(FD);
	}
	for(auto FD : removeList){ removeFromWaitingFDs(FD); }
}

bool SocketReader::readChunkFromFD(int FD){
	{ // lock
	std::lock_guard<std::recursive_mutex> lck(waitingMut);
	if(waitingFDs.count(FD) == 0) throwInt("invalid FD "<<FD);
	} // unlock

	ByteArray buffer(maxBufferSize);
	bool done;
	size_t count = systemWrap->readFD(FD, &buffer[0], maxBufferSize, done);
	if(done){ // if we read all data.
		return true; // removeFromWaitingFDs(FD);
	}

	else if (count == 0){ // End of file. Remote closed connection.
		throwInt("client closed connection disconnected");
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
