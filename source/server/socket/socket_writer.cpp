#include "source/server/socket/socket_writer.h"

#include <unistd.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <signal.h>

#include "source/server/socket/set_of_file_descriptors.h"
#include "source/data_types/socket_message.h"
#include "source/server/socket/websocket/websocket_message_sender.h"
#include "source/server/socket/system_wrapper.h"


SocketWriterInterface::~SocketWriterInterface(){}

SocketWriter::SocketWriter(SystemInterface *_systemWrap, SetOfFileDescriptors *FDs, bool* run) : 	systemWrap(_systemWrap), sender( new WebsocketMessageSender(_systemWrap) ), writePollingMut(),
																					fileDescriptors(FDs), running(run), MAXEVENTS(9999), epollFD(-1) {

	fileDescriptors->addNewConnectionCallback(std::bind(&SocketWriter::newConnectionHandler,this, std::placeholders::_1));
	fileDescriptors->addCloseFDCallback(std::bind(&SocketWriter::closeFDHandler,this, std::placeholders::_1));
	signal(SIGPIPE, SIG_IGN); //ignore error when writing to closed sockets to prevent crash on client disconnect
}


void SocketWriter::startPoll(){
	setupEpoll();
	std::vector<epoll_event> events;
	events.resize( static_cast<size_t>(MAXEVENTS) );

	while(*running){	//The event loop
		size_t num = systemWrap->epollWait(epollFD, &events[0], MAXEVENTS, 500);
		for (size_t i = 0; i < num; ++i){
			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)){// An error occured
				//LOG_ERROR("SocketWriter::startPoll", "epoll error");
				int error = 0;
				socklen_t errlen = sizeof(error);
				if (systemWrap->getSockOpt(events[i].data.fd, SOL_SOCKET, SO_ERROR, reinterpret_cast<void *>(&error), &errlen) == 0){
					//LOG_ERROR("SocketWriter::startPoll epoll error", "error: "<<systemWrap->strError(error) );
				}
				closeFD(events[i].data.fd);
				continue; //go to next FD
			}
			if((events[i].events & EPOLLOUT)){			//the socket is ready for writing
				try{ writeData(events[i].data.fd); }	//write data to client
				catch( int &ret){
					closeFD(events[i].data.fd);			//drop connection on failure
					//BACKTRACE_PRINT();
				}
			}
		}
	}
}

SocketWriter::~SocketWriter(){}

void SocketWriter::setupEpoll(){
	epollFD = systemWrap->epollCreate(0);
	//LOG_INFO("SocketWriter::setupEpoll", "Writer epollFD is "<<epollFD);
}


void SocketWriter::closeFD(int FD){
	if(fileDescriptors->removeFD(FD) == -1){
		//LOG_ERROR("SocketWriter::closeFD", "FD "<<FD<<" failed to close correctly");
	}
}

void SocketWriter::closeFDHandler(int FD){
	//LOG_INFO("SocketWriter::closeFDHandler", "closed connection on FD "<<FD);
	try{ fileDescriptors->stopPollingFD(epollFD, FD); }
	catch(int &ret) {
		//writeError("epoll_ctl"); BACKTRACE_PRINT();
	}
	sender->closeFDHandler(FD);
}

void SocketWriter::newConnectionHandler(int FD){
	(void) FD; //stops warnings for unused variable when logging is turned off
	//LOG_INFO("SocketWriter::newConnectionHandler", "New connection on FD "<<FD);
	return;
}

void SocketWriter::writeData(int FD){
	std::lock_guard<std::mutex> lck(writePollingMut);
	bool done = sender->writeData(FD);
	if(done){
		fileDescriptors->stopPollingFD(epollFD, FD); //done writing
	}
}

void SocketWriter::sendMessage(SocketMessage &message){
	int FD = message.getFD();
	sender->addMessage(message);
	fileDescriptors->startPollingForWrite(epollFD, FD);
}
