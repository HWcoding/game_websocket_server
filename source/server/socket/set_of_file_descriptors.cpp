#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/system_wrapper.h"
#include "source/logging/exception_handler.h"


SetOfFileDescriptors::SetOfFileDescriptors(SystemInterface *_systemWrap) : systemWrap(_systemWrap), openFDs(), FDmut(), closeCallbacks(), newConnectionCallbacks(){}

SetOfFileDescriptors::~SetOfFileDescriptors()
{
	std::lock_guard<std::recursive_mutex> lck(FDmut); //don't destroy while other threads are accessing this
	for (auto& elem: openFDs){ //close remaining FDs
		int FD = elem.second.getFD();
		systemWrap->closeFD(FD);
		LOG_INFO("removed FD "<<FD);
	}
}

ByteArray SetOfFileDescriptors::getIP(int FD)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(openFDs.count(FD)>0){ //check to see if FD is in list
		return openFDs.at(FD).getIP();
	}
	else {
		throwInt(FD<<" is an invalid file descriptor");
		ByteArray dummy;
		return dummy;
	}
}

ByteArray SetOfFileDescriptors::getPort(int FD)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(openFDs.count(FD)>0){ //check to see if FD is in list
		return openFDs.at(FD).getPort();
	}
	else {
		throwInt(FD<<" is an invalid file descriptor");
		ByteArray dummy;
		return dummy;
	}
}

ByteArray SetOfFileDescriptors::getCSRFkey(int FD)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(openFDs.count(FD)>0){ //check to see if FD is in list
		return openFDs.at(FD).getCSRFkey();
	}
	else {
		throwInt(FD<<" is an invalid file descriptor");
		ByteArray dummy;
		return dummy;
	}
}

void SetOfFileDescriptors::setIP(int FD, ByteArray s)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(openFDs.count(FD)>0){ //check to see if FD is in list
		openFDs.at(FD).setIP(s);
	}
	else throwInt(FD<<" is an invalid file descriptor");
}

void SetOfFileDescriptors::setPort(int FD, ByteArray s)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(openFDs.count(FD)>0){ //check to see if FD is in list
		openFDs.at(FD).setPort(s);
	}
	else throwInt(FD<<" is an invalid file descriptor");
}

void SetOfFileDescriptors::setCSRFkey(int FD, ByteArray s)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(openFDs.count(FD)>0){ //check to see if FD is in list
		openFDs.at(FD).setCSRFkey(s);
	}
	else throwInt(FD<<" is an invalid file descriptor");
}

void SetOfFileDescriptors::addCloseFDCallback(std::function<void(int)> callback)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	closeCallbacks.push_back(callback);
}

void SetOfFileDescriptors::addNewConnectionCallback(std::function<void(int)> callback)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	newConnectionCallbacks.push_back(callback);
}

int SetOfFileDescriptors::addFD(int FD)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(FD>0 && openFDs.count(FD)==0){ //a negative number is an error message passed through this function. Don't add to FDs. Just return the error.
		openFDs.emplace(std::piecewise_construct,
						std::forward_as_tuple(FD),
						std::forward_as_tuple(systemWrap,FD));
	}
	else return -1;
	return FD;
}

int SetOfFileDescriptors::tellServerAboutNewConnection(int FD)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(isFDOpen(FD)){ //check to see if FD is in list of open FD's.
		for(unsigned int i =0; i< newConnectionCallbacks.size(); i++){
			newConnectionCallbacks[i](FD);
		}
	}
	return FD;
}

bool SetOfFileDescriptors::callCloseCallbacks(int FD)
{
	bool callbackThrew = false;
	for(unsigned int i =0; i< closeCallbacks.size(); ++i){
		//call all of the callbacks associated with this socket's closure.
		try{ closeCallbacks[i](FD); }
		catch(...){
			BACKTRACE_PRINT();
			callbackThrew = true;
		}
	}
	return callbackThrew;

}

int SetOfFileDescriptors::removeFD(int FD)
{
	int ret = 0;
	bool callbackThrew = false;
	try{
		std::lock_guard<std::recursive_mutex> lock_FD(FDmut);
		if(isFDOpen(FD)){ //check to see if FD is in list of open FD's.
			callbackThrew = callCloseCallbacks(FD);
			systemWrap->closeFD(FD);
			openFDs.erase(FD);
		}
	}
	catch(...){ //this function is called in destructors and should not throw
		BACKTRACE_PRINT();
		ret = -1;
	}
	if(callbackThrew) ret = -1;
	LOG_INFO("removed FD "<<FD);
	return ret;
}

bool SetOfFileDescriptors::isFDOpen(int FD)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(openFDs.count(FD)>0){ //check to see if FD is in list of open FD's.
		return true;
	}
	else return false;
}

void SetOfFileDescriptors::stopPollingFD(int epoll, int FD)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(openFDs.count(FD)>0){ //check to see if FD is in list of open FD's.
		openFDs.at(FD).stopPollingFD(epoll);
	}
	else throwInt(FD<<" is an invalid file descriptor");
}

void SetOfFileDescriptors::startPollingForWrite(int epoll, int FD)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(openFDs.count(FD)>0){ //check to see if FD is in list of open FD's.
		openFDs.at(FD).startPollingForWrite(epoll);
	}
	else throwInt(FD<<" is an invalid file descriptor");
}

void SetOfFileDescriptors::startPollingForRead(int epoll, int FD)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(openFDs.count(FD)>0){ //check to see if FD is in list of open FD's.
		openFDs.at(FD).startPollingForRead(epoll);
	}
	else throwInt(FD<<" is an invalid file descriptor");
}

void SetOfFileDescriptors::makeNonblocking(int FD)
{
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(openFDs.count(FD)>0){ //check to see if FD is in list of open FD's.
		openFDs.at(FD).makeNonblocking();
	}
	else throwInt(FD<<" is an invalid file descriptor");
}

std::unique_lock<std::recursive_mutex> SetOfFileDescriptors::getAndLockFD(FileDescriptor* &FDPointer, int FD)
{
	FDPointer = NULL;
	std::lock_guard<std::recursive_mutex> lck(FDmut);
	if(openFDs.count(FD)>0){ //check to see if FD is in list of open FD's.
		std::unique_lock<std::recursive_mutex> newLock(openFDs.at(FD).lock()); //lock the fd so it won't get destroyed or changed
		FDPointer = &(openFDs.at(FD));
		return newLock;
	}
	else throwInt(FD<<" is an invalid file descriptor");
	return std::unique_lock<std::recursive_mutex>();
}
