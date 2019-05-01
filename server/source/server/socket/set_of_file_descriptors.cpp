#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/system_wrapper.h"
#include "source/logging/exception_handler.h"



SetOfFileDescriptors::SetOfFileDescriptors() : openFDs(), lockState(), closeCallbacks(), newConnectionCallbacks(){}

// writers
/**
 * @throws std::system_error if lock fails
 */
SetOfFileDescriptors::~SetOfFileDescriptors()
{
	WriterLockGuard lk(lockState);
	// we are shutting down so we can't assume any of the objects
	// the closeFDCallbacks use are still around. Just close the FD's
	// without calling them.
	for (auto& elem: openFDs){ //close remaining FDs
		int FD = elem.second.getFD();
		closeFD(FD);
		LOG_INFO("removed FD "<<FD);
	}
	// clear all buffers incase something tries to read them
	openFDs.clear();
	closeCallbacks.clear();
	newConnectionCallbacks.clear();
}

/**
 * @throws std::system_error if lock fails
 */
void SetOfFileDescriptors::addCloseFDCallback(std::function<void(int)> callback)
{
	WriterLockGuard lk(lockState);
	closeCallbacks.push_back(callback);
}

/**
 * @throws std::system_error if lock fails
 */
void SetOfFileDescriptors::addNewConnectionCallback(std::function<void(int)> callback)
{
	WriterLockGuard lk(lockState);
	newConnectionCallbacks.push_back(callback);
}

/**
 * @throws std::system_error if lock fails
 */
int SetOfFileDescriptors::addFD(int FD)
{
	WriterLockGuard lk(lockState);
	if(FD>0 && openFDs.count(FD)==0){ //a negative number is an error message passed through this function. Don't add to FDs. Just return the error.
		openFDs.emplace(std::piecewise_construct,
						std::forward_as_tuple(FD),
						std::forward_as_tuple(FD));
	} else {
		FD = -1;
	}
	return FD;
}

int SetOfFileDescriptors::removeFD(int FD)
{
	int ret = 0;
	bool callbackThrew = false;
	try{
		callbackThrew = callCloseCallbacks(FD);
	}
	catch(...) { //this function is called in destructors and should not throw
		BACKTRACE_PRINT();
		ret = -1;
	}
	WriterLockGuard lk(lockState);
	if(openFDs.count(FD)>0){
		closeFD(FD);
		openFDs.erase(FD);
	}
	if(callbackThrew) ret = -1;
	LOG_INFO("removed FD "<<FD);
	return ret;
}











// readers
/**
 * @throws std::runtime_error
 * @throws std::system_error if lock fails
 */
ByteArray SetOfFileDescriptors::getIP(int FD)
{
	ReaderLockGuard lk(lockState);
	if(openFDs.count(FD)>0){ //check to see if FD is in list
		return openFDs.at(FD).getIP();
	}

	throw std::runtime_error(LOG_EXCEPTION(std::string()+std::to_string(FD)+" is an invalid file descriptor"));
}

/**
 * @throws std::runtime_error
 * @throws std::system_error if lock fails
 */
ByteArray SetOfFileDescriptors::getPort(int FD)
{
	ReaderLockGuard lk(lockState);
	if(openFDs.count(FD)>0){ //check to see if FD is in list
		return openFDs.at(FD).getPort();
	}
	throw std::runtime_error(LOG_EXCEPTION(std::string()+std::to_string(FD)+" is an invalid file descriptor"));
}

/**
 * @throws std::runtime_error
 * @throws std::system_error if lock fails
 */
ByteArray SetOfFileDescriptors::getCSRFkey(int FD)
{
	ReaderLockGuard lk(lockState);
	if(openFDs.count(FD)>0){ //check to see if FD is in list
		return openFDs.at(FD).getCSRFkey();
	}
	throw std::runtime_error(LOG_EXCEPTION(std::string()+std::to_string(FD)+" is an invalid file descriptor"));
}

/**
 * @throws std::runtime_error
 * @throws std::system_error if lock fails
 */
void SetOfFileDescriptors::setIP(int FD, ByteArray s)
{
	ReaderLockGuard lk(lockState);
	if(openFDs.count(FD)>0){ //check to see if FD is in list
		openFDs.at(FD).setIP(s);
	}
	else {
		throw std::runtime_error(LOG_EXCEPTION(std::string()+std::to_string(FD)+" is an invalid file descriptor"));
	}
}

/**
 * @throws std::runtime_error
 * @throws std::system_error if lock fails
 */
void SetOfFileDescriptors::setPort(int FD, ByteArray s)
{
	ReaderLockGuard lk(lockState);
	if(openFDs.count(FD)>0){ //check to see if FD is in list
		openFDs.at(FD).setPort(s);
	}
	else {
		throw std::runtime_error(LOG_EXCEPTION(std::string()+std::to_string(FD)+" is an invalid file descriptor"));
	}
}

/**
 * @throws std::runtime_error
 * @throws std::system_error if lock fails
 */
void SetOfFileDescriptors::setCSRFkey(int FD, ByteArray s)
{
	ReaderLockGuard lk(lockState);
	if(openFDs.count(FD)>0){ //check to see if FD is in list
		openFDs.at(FD).setCSRFkey(s);
	}
	else {
		throw std::runtime_error(LOG_EXCEPTION(std::string()+std::to_string(FD)+" is an invalid file descriptor"));
	}
}



/**
 * @throws std::system_error if lock fails
 */
bool SetOfFileDescriptors::tellServerAboutNewConnection(int FD)
{
	bool callbackThrew = false;
	std::vector<std::function<void(int)>> callbacks;
	{//lock
		ReaderLockGuard lk(lockState);
		if(openFDs.count(FD)>0){ //check to see if FD is in list of open FD's.
			callbacks = newConnectionCallbacks;
		}
	}//unlock

	for(auto callback : callbacks) {
		//call all of the callbacks that handle a new connection.
		try{ callback(FD); }
		catch(...){
			BACKTRACE_PRINT();
			callbackThrew = true;
		}
	}

	return callbackThrew;
}

/**
 * @throws std::system_error if lock fails
 */
bool SetOfFileDescriptors::callCloseCallbacks(int FD)
{
	std::vector<std::function<void(int)>> callbacks;
	bool callbackThrew = false;
	{//lock
		ReaderLockGuard lk(lockState);
		if(openFDs.count(FD)>0){
			callbacks = closeCallbacks;
		}
		else {
			return false;
		}
	}//unlock

	for(auto callback : callbacks){
		//call all of the callbacks associated with this socket's closure.
		try{ callback(FD); }
		catch(...){
			BACKTRACE_PRINT();
			callbackThrew = true;
		}
	}
	return callbackThrew;

}

/**
 * @throws std::system_error if lock fails
 */
bool SetOfFileDescriptors::isFDOpen(int FD)
{
	ReaderLockGuard lk(lockState);
	return openFDs.count(FD)>0;
}

/**
 * @throws std::runtime_error
 * @throws std::system_error if lock fails
 */
void SetOfFileDescriptors::stopPollingFD(int epoll, int FD)
{
	ReaderLockGuard lk(lockState);
	if(openFDs.count(FD)>0){ //check to see if FD is in list of open FD's.
		openFDs.at(FD).stopPollingFD(epoll);
	}
	else {
		throw std::runtime_error(LOG_EXCEPTION(std::string()+std::to_string(FD)+" is an invalid file descriptor"));
	}
}

/**
 * @throws std::runtime_error
 * @throws std::system_error if lock fails
 */
void SetOfFileDescriptors::startPollingForWrite(int epoll, int FD)
{
	ReaderLockGuard lk(lockState);
	if(openFDs.count(FD)>0){ //check to see if FD is in list of open FD's.
		openFDs.at(FD).startPollingForWrite(epoll);
	}
	else {
		throw std::runtime_error(LOG_EXCEPTION(std::string()+std::to_string(FD)+" is an invalid file descriptor"));
	}
}

/**
 * @throws std::runtime_error if FD is not an open file descriptor
 * @throws std::system_error if lock fails
 */
void SetOfFileDescriptors::startPollingForRead(int epoll, int FD)
{
	ReaderLockGuard lk(lockState);
	if(openFDs.count(FD)>0){ //check to see if FD is in list of open FD's.
		openFDs.at(FD).startPollingForRead(epoll);
	}
	else {
		throw std::runtime_error(LOG_EXCEPTION(std::string()+std::to_string(FD)+" is an invalid file descriptor"));
	}
}

/**
 * @throws std::runtime_error if FD is not an open file descriptor
 * @throws std::system_error if lock fails
 */
void SetOfFileDescriptors::makeNonblocking(int FD)
{
	ReaderLockGuard lk(lockState);
	if(openFDs.count(FD)>0){ //check to see if FD is in list of open FD's.
		openFDs.at(FD).makeNonblocking();
	}
	else {
		throw std::runtime_error(LOG_EXCEPTION(std::string()+std::to_string(FD)+" is an invalid file descriptor"));
	}
}
