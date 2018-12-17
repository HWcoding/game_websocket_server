#include "source/server/socket/file_descriptor.h"
#include "source/server/socket/system_wrapper.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>


FileDescriptor::FileDescriptor() {}

FileDescriptor::FileDescriptor(int _FD): FD(_FD) {}

/**
 * @throws std::system_error if lock fails followed by abort since it's declared noexcept
 */
FileDescriptor::FileDescriptor(FileDescriptor&& f) noexcept {	//move constructor
	std::lock_guard<std::mutex> lck(f.mut);
	FD = std::move(f.FD);
	IP = std::move(f.IP);
	port = std::move(f.port);
	CSRFkey = std::move(f.CSRFkey);
	f.FD = -1;
}

/**
 * @throws std::system_error if lock fails followed by abort since it's declared noexcept
 */
FileDescriptor::FileDescriptor(const FileDescriptor& f) noexcept { //copy constructor
	std::lock_guard<std::mutex> lck(f.mut);
	FD = f.FD;
	IP = f.IP;
	port = f.port;
	CSRFkey = f.CSRFkey;
}

/**
 * @throws std::system_error if lock fails
 */
FileDescriptor& FileDescriptor::operator=(FileDescriptor&& f) { //move assignment
	if (this != &f){
		std::unique_lock<std::mutex> lhs_lk(mut, std::defer_lock);
		std::unique_lock<std::mutex> rhs_lk(f.mut, std::defer_lock);
		std::lock(lhs_lk, rhs_lk);
		FD = std::move(f.FD);
		IP = std::move(f.IP);
		port = std::move(f.port);
		CSRFkey = std::move(f.CSRFkey);
		f.FD =-1;
	}
	return *this;
}

/**
 * @throws std::system_error if lock fails
 */
FileDescriptor& FileDescriptor::operator=(const FileDescriptor& f) { //copy assignment
	if (this != &f){
		std::unique_lock<std::mutex> lhs_lk(mut, std::defer_lock);
		std::unique_lock<std::mutex> rhs_lk(f.mut, std::defer_lock);
		std::lock(lhs_lk, rhs_lk);
		FD = f.FD;
		IP = f.IP;
		port = f.port;
		CSRFkey = f.CSRFkey;
	}
	return *this;
}

FileDescriptor::~FileDescriptor(){
	try{
		std::lock_guard<std::mutex> lck(mut);
	}
	catch(...){}
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
 * @throws std::system_error if lock fails
 */
void FileDescriptor::setFDReadWrite(epoll_event event, int epoll){
	std::lock_guard<std::mutex> lck(mut);
	epollControlMod(epoll, FD, &event);
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
	struct epoll_event event ={};
	event.data.fd = FD;
	event.events = EPOLLIN|EPOLLET;
	bool exists = startPollingFD(event, epoll);
	if(exists) pollForRead(epoll);
	return;
}

/**
 * @throws std::runtime_error if epollControlAdd returns an error
 * @throws std::system_error if lock fails
 */
bool FileDescriptor::startPollingFD(epoll_event event, int epoll){
	std::lock_guard<std::mutex> lck(mut);
	return epollControlAdd(epoll, FD, &event);
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
