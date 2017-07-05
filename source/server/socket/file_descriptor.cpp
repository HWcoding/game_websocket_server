#include "source/server/socket/file_descriptor.h"
#include "source/server/socket/system_wrapper.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <string.h>


FileDescriptor::FileDescriptor(SystemInterface *_systemWrap) : systemWrap(_systemWrap), mut(), IP(), port(), CSRFkey(), FD(-1) {}

FileDescriptor::FileDescriptor(SystemInterface *_systemWrap, int _FD): systemWrap(_systemWrap), mut(), IP(), port(), CSRFkey(), FD(_FD) {}

FileDescriptor::FileDescriptor(FileDescriptor&& f) noexcept: systemWrap(), mut(), IP(), port(), CSRFkey(), FD(-1) {	//move constructor
	std::lock_guard<std::recursive_mutex> lck(f.mut);
	FD = std::move(f.FD);
	IP = std::move(f.IP);
	port = std::move(f.port);
	CSRFkey = std::move(f.CSRFkey);
	systemWrap = f.systemWrap;
	f.FD = -1;
}

FileDescriptor::FileDescriptor(const FileDescriptor& f) noexcept : systemWrap(), mut(), IP(), port(), CSRFkey(), FD(-1) { //copy constructor
	std::lock_guard<std::recursive_mutex> lck(f.mut);
	FD = f.FD;
	IP = f.IP;
	port = f.port;
	CSRFkey = f.CSRFkey;
	systemWrap = f.systemWrap;
}

FileDescriptor& FileDescriptor::operator=(FileDescriptor&& f){ //move assignment
	if (this != &f){
		std::unique_lock<std::recursive_mutex> lhs_lk(mut, std::defer_lock);
		std::unique_lock<std::recursive_mutex> rhs_lk(f.mut, std::defer_lock);
		std::lock(lhs_lk, rhs_lk);
		FD = std::move(f.FD);
		IP = std::move(f.IP);
		port = std::move(f.port);
		CSRFkey = std::move(f.CSRFkey);
		systemWrap = f.systemWrap;
		f.FD =-1;
	}
	return *this;
}

FileDescriptor& FileDescriptor::operator=(const FileDescriptor& f){ //copy assignment
	if (this != &f){
		std::unique_lock<std::recursive_mutex> lhs_lk(mut, std::defer_lock);
		std::unique_lock<std::recursive_mutex> rhs_lk(f.mut, std::defer_lock);
		std::lock(lhs_lk, rhs_lk);
		FD = f.FD;
		IP = f.IP;
		port = f.port;
		CSRFkey = f.CSRFkey;
		systemWrap = f.systemWrap;
	}
	return *this;
}

FileDescriptor::~FileDescriptor(){
	std::lock_guard<std::recursive_mutex> lck(mut);
}

int FileDescriptor::getFD(){
	std::lock_guard<std::recursive_mutex> lck(mut);
	return FD;
}

ByteArray FileDescriptor::getIP(){
	std::lock_guard<std::recursive_mutex> lck(mut);
	return IP;
}

ByteArray FileDescriptor::getPort(){
	std::lock_guard<std::recursive_mutex> lck(mut);
	return port;
}

ByteArray FileDescriptor::getCSRFkey(){
	std::lock_guard<std::recursive_mutex> lck(mut);
	return CSRFkey;
}

void FileDescriptor::setIP(ByteArray s){
	std::lock_guard<std::recursive_mutex> lck(mut);
	IP = s;
}

void FileDescriptor::setPort(ByteArray s){
	std::lock_guard<std::recursive_mutex> lck(mut);
	port = s;
}

void FileDescriptor::setCSRFkey(ByteArray s){
	std::lock_guard<std::recursive_mutex> lck(mut);
	CSRFkey = s;
}

void FileDescriptor::setIP(std::string s){
	std::lock_guard<std::recursive_mutex> lck(mut);
	IP.resize(s.size());
	memcpy(&IP[0], &s[0], s.size());
}

void FileDescriptor::setPort(std::string s){
	std::lock_guard<std::recursive_mutex> lck(mut);
	port.resize(s.size());
	memcpy(&port[0], &s[0], s.size());
}

void FileDescriptor::setCSRFkey(std::string s){
	std::lock_guard<std::recursive_mutex> lck(mut);
	CSRFkey.resize(s.size());
	memcpy(&CSRFkey[0], &s[0], s.size());
}

void FileDescriptor::pollForWrite(int epoll){
	struct epoll_event event ={};
	event.data.fd = FD;
	event.events = EPOLLOUT|EPOLLET;
	setFDReadWrite(event, epoll);
}

void FileDescriptor::pollForRead(int epoll){
	struct epoll_event event ={};
	event.data.fd = FD;
	event.events = EPOLLIN|EPOLLET;
	setFDReadWrite(event, epoll);
}

void FileDescriptor::setFDReadWrite(epoll_event event, int epoll){
	std::lock_guard<std::recursive_mutex> lck(mut);
	systemWrap->epollControlMod(epoll, FD, &event);
}

void FileDescriptor::stopPollingFD(int epoll){
	std::lock_guard<std::recursive_mutex> lck(mut);
	systemWrap->epollControlDelete(epoll, FD, nullptr);
}

void FileDescriptor::startPollingForWrite(int epoll){
	struct epoll_event event ={};
	event.data.fd = FD;
	event.events = EPOLLOUT|EPOLLET;
	bool exists = startPollingFD(event, epoll);
	if(exists) pollForWrite(epoll);
	return;
}

void FileDescriptor::startPollingForRead(int epoll){
	struct epoll_event event ={};
	event.data.fd = FD;
	event.events = EPOLLIN|EPOLLET;
	bool exists = startPollingFD(event, epoll);
	if(exists) pollForRead(epoll);
	return;
}

bool FileDescriptor::startPollingFD(epoll_event event, int epoll){
	std::lock_guard<std::recursive_mutex> lck(mut);
	return systemWrap->epollControlAdd(epoll, FD, &event);
}

void FileDescriptor::makeNonblocking (){
	std::lock_guard<std::recursive_mutex> lck(mut);
	int flags = systemWrap->getFlags(FD);
	flags |= O_NONBLOCK;
	systemWrap->setFlags(FD, flags);
}

std::unique_lock<std::recursive_mutex> FileDescriptor::lock(){
	std::unique_lock<std::recursive_mutex> lck(mut);
	return lck;
}
