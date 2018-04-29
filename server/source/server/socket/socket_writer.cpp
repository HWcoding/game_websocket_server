#include "source/server/socket/socket_writer.h"

#include <unistd.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <csignal>
#include <atomic>

#include "source/server/socket/set_of_file_descriptors.h"
#include "source/data_types/socket_message.h"
#include "source/server/socket/message_sender_interface.h"
#include "source/server/socket/websocket/websocket_message_sender.h"
#include "source/server/socket/system_wrapper.h"
#include "source/logging/exception_handler.h"


SocketWriter::SocketWriter(SystemInterface *_systemWrap,
                           SetOfFileDescriptors *FDs,
                           std::atomic<bool>* run) : SocketNode(_systemWrap, FDs, run),
                                                     sender(new WebsocketMessageSender(_systemWrap)),
                                                     writePollingMut()
                                                     {

	fileDescriptors->addNewConnectionCallback(std::bind(&SocketWriter::newConnectionHandler, this, std::placeholders::_1));
	fileDescriptors->addCloseFDCallback(std::bind(&SocketWriter::closeFDHandler, this, std::placeholders::_1));
	signal(SIGPIPE, SIG_IGN); //ignore error when writing to closed sockets to prevent crash on client disconnect
}

// unused
void SocketWriter::handleEpollRead(epoll_event &event){
	(void)event;
}


void SocketWriter::handleEpollWrite(epoll_event &event) {
	try {
		//write data to client
		writeData(event.data.fd);
	}
	catch(std::runtime_error &ret) {
		BACKTRACE_PRINT();
		closeFD(event.data.fd);			//drop connection on failure
	}
}

SocketWriter::~SocketWriter() = default;

void SocketWriter::setupEpoll() {
	SocketNode::setupEpoll();
	LOG_INFO("Writer epollFD is " << epollFD);
}

void SocketWriter::closeFDHandler(int FD) {
	LOG_INFO("closed connection on FD " << FD);
	try {
		fileDescriptors->stopPollingFD(epollFD, FD);
	}
	catch(std::runtime_error &ret) {
		BACKTRACE_PRINT();
		writeError("epoll_ctl");
	}
	sender->closeFDHandler(FD);
}

void SocketWriter::newConnectionHandler(int FD) {
	(void) FD; //stops warnings for unused variable when logging is turned off
	LOG_INFO("New connection on FD "<<FD);
	return;
}

void SocketWriter::writeData(int FD) {
	std::lock_guard<std::mutex> lck(writePollingMut);
	bool done = sender->writeData(FD);
	if(done) {
		fileDescriptors->stopPollingFD(epollFD, FD); //done writing
	}
}

void SocketWriter::sendMessage(SocketMessage &message) {
	int FD = message.getFD();
	sender->addMessage(message);
	fileDescriptors->startPollingForWrite(epollFD, FD);
}
