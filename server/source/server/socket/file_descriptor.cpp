#include "source/server/socket/file_descriptor.h"
#include "source/server/socket/system_wrapper.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>

FileDescriptor::FileDescriptor() = default;

FileDescriptor::FileDescriptor(int _FD): FD(_FD) {}

FileDescriptor::~FileDescriptor(){
	try{
		std::lock_guard<std::mutex> lck(mut);
	}
	catch(...){}
}



/**
 * @throws std::system_error if lock fails
 */
void FileDescriptor::stopPollingFD(int epoll){
	std::lock_guard<std::mutex> lck(mut);
	epollControlDelete(epoll, FD, nullptr);
}


/**
 * @throws std::runtime_error
 */
void FileDescriptor::startPollingForWrite(int epoll){
	std::lock_guard<std::mutex> lck(mut);
	struct epoll_event event ={};
	event.data.fd = FD;
	event.events = EPOLLOUT|EPOLLET;
	bool exists = startPollingFD(event, epoll);
	if(exists) pollForWrite(epoll);
	return;
}


/**
 * @throws std::runtime_error
 */
void FileDescriptor::startPollingForRead(int epoll){
	std::lock_guard<std::mutex> lck(mut);
	struct epoll_event event ={};
	event.data.fd = FD;
	event.events = EPOLLIN|EPOLLET;
	bool exists = startPollingFD(event, epoll);
	if(exists) pollForRead(epoll);
	return;
}


/**
 * @throws std::runtime_error
 * @throws std::system_error if lock fails
 */
void FileDescriptor::makeNonblocking (){
	std::lock_guard<std::mutex> lck(mut);
	int flags = getFlags(FD);
	flags |= O_NONBLOCK;
	setFlags(FD, flags);
}


/**
 * @throws std::system_error if lock fails
 */
int FileDescriptor::getFD(){
	std::lock_guard<std::mutex> lck(mut);
	return FD;
}

/**
 * @throws std::system_error if lock fails
 */
ByteArray FileDescriptor::getIP(){
	std::lock_guard<std::mutex> lck(mut);
	return IP;
}


/**
 * @throws std::system_error if lock fails
 */
ByteArray FileDescriptor::getPort(){
	std::lock_guard<std::mutex> lck(mut);
	return port;
}


/**
 * @throws std::system_error if lock fails
 */
ByteArray FileDescriptor::getCSRFkey(){
	std::lock_guard<std::mutex> lck(mut);
	return CSRFkey;
}


/**
 * @throws std::system_error if lock fails
 */
void FileDescriptor::setIP(ByteArray s){
	std::lock_guard<std::mutex> lck(mut);
	IP = s;
}


/**
 * @throws std::system_error if lock fails
 */
void FileDescriptor::setPort(ByteArray s){
	std::lock_guard<std::mutex> lck(mut);
	port = s;
}


/**
 * @throws std::system_error if lock fails
 */
void FileDescriptor::setCSRFkey(ByteArray s){
	std::lock_guard<std::mutex> lck(mut);
	CSRFkey = s;
}


/**
 * @throws std::system_error if lock fails
 */
void FileDescriptor::setIP(std::string s){
	std::lock_guard<std::mutex> lck(mut);
	IP.resize(s.size());
	memcpy(&IP[0], &s[0], s.size());
}


/**
 * @throws std::system_error if lock fails
 */
void FileDescriptor::setPort(std::string s){
	std::lock_guard<std::mutex> lck(mut);
	port.resize(s.size());
	memcpy(&port[0], &s[0], s.size());
}


/**
 * @throws std::system_error if lock fails
 */
void FileDescriptor::setCSRFkey(std::string s){
	std::lock_guard<std::mutex> lck(mut);
	CSRFkey.resize(s.size());
	memcpy(&CSRFkey[0], &s[0], s.size());
}











////////////////////////////////////////
//------------private-----------------//
////////////////////////////////////////

/**
 * @throws std::runtime_error
 */
void FileDescriptor::pollForWrite(int epoll){
	struct epoll_event event ={};
	event.data.fd = FD;
	event.events = EPOLLOUT|EPOLLET;
	setFDReadWrite(event, epoll);
}

/**
 * @throws std::runtime_error
 */
void FileDescriptor::pollForRead(int epoll){
	struct epoll_event event ={};
	event.data.fd = FD;
	event.events = EPOLLIN|EPOLLET;
	setFDReadWrite(event, epoll);
}

/**
 * @throws std::runtime_error
 */
void FileDescriptor::setFDReadWrite(epoll_event event, int epoll){
	epollControlMod(epoll, FD, &event);
}

/**
 * @throws std::runtime_error if epollControlAdd returns an error
 */
bool FileDescriptor::startPollingFD(epoll_event event, int epoll){
	return epollControlAdd(epoll, FD, &event);
}