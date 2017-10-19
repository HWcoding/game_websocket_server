#include "source/server/socket/socket_node.h"


#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/system_wrapper.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <signal.h>
#include <netdb.h>
#include <atomic>

#include "source/logging/exception_handler.h"


SocketNode::SocketNode(SystemInterface *_systemWrap,
                       SetOfFileDescriptors *FDs,
                       std::atomic<bool>* run) : systemWrap(_systemWrap),
                                                 running(run),
                                                 fileDescriptors(FDs),
                                                 MAXEVENTS(9999),
                                                 epollFD(-1)
{
	signal(SIGPIPE, SIG_IGN); //ignore error when writing to closed sockets to prevent crash on client disconnect
}

SocketNode::~SocketNode(){
	if(epollFD != -1){
		systemWrap->closeFD(epollFD);
		epollFD = -1;
	}
}

void SocketNode::closeFD(int FD){
	int ret= fileDescriptors->removeFD(FD);
	if(ret == -1){
		LOG_ERROR("File descriptor " << FD << " failed to close properly. ");
	}
}

int SocketNode::getWaitTime() {
	return 2000;
}

bool SocketNode::handleEpollErrors(epoll_event &event){
	if ( (event.events & EPOLLERR) || (event.events & EPOLLHUP) ){// An error occured
		LOG_ERROR("epoll error");
		int error = 0;
		socklen_t errlen = sizeof(error);
		if (systemWrap->getSockOpt(event.data.fd, SOL_SOCKET, SO_ERROR, reinterpret_cast<void *>(&error), &errlen) == 0){
			LOG_ERROR("error: " << systemWrap->strError(error) );
		}
		else {
			LOG_ERROR("epoll error");
		}
		closeFD(event.data.fd);
		return true; //there was an error
	}
	return false;
}

void SocketNode::processEvents(std::vector<epoll_event> &events){
	size_t numberOfEvents = systemWrap->epollWait(epollFD, &events[0], MAXEVENTS, getWaitTime());
	for (size_t i = 0; i < numberOfEvents; ++i){
		if(handleEpollErrors(events[i])){
			continue; //an error occured; move to next event
		}
		handleEpollRead(events[i]);
		handleEpollWrite(events[i]);
	}
}

void SocketNode::startPoll(){
	LOG_INFO("poll started");
	setupEpoll();

	std::vector<epoll_event> events;
	events.resize( static_cast<size_t>(MAXEVENTS) );

	while(running->load()){	//The event loop
		processEvents(events);
	}
}

void SocketNode::setupEpoll(){
	epollFD = systemWrap->epollCreate(0);
}

