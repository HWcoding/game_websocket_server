#ifndef SERVER_SOCKET_SET_OF_FILE_DESCRIPTORS_H_
#define SERVER_SOCKET_SET_OF_FILE_DESCRIPTORS_H_
//#include "source/server/socket/set_of_file_descriptors.h"

#include <vector>
#include <unordered_map>
#include <mutex>

#include "source/server/socket/file_descriptor.h"
#include "source/data_types/byte_array.h"

class SystemInterface;

class SetOfFileDescriptors{
public:
	void addCloseFDCallback(std::function<void(int)> callback);
	void addNewConnectionCallback(std::function<void(int)> callback);
	int  addFD(int FD);
	int  tellServerAboutNewConnection(int FD);
	int  removeFD(int FD);
	bool isFDOpen(int FD);
	void stopPollingFD(int epoll, int FD);
	void startPollingForWrite(int epoll, int FD);
	void startPollingForRead(int epoll, int FD);
	void makeNonblocking (int FD);

	SetOfFileDescriptors(SystemInterface *_systemWrap);
	~SetOfFileDescriptors();
	ByteArray getIP(int FD);
	ByteArray getPort(int FD);
	ByteArray getCSRFkey(int FD);
	void setIP(int FD, ByteArray s);
	void setPort(int FD, ByteArray s);
	void setCSRFkey(int FD, ByteArray s);

private:
	SetOfFileDescriptors& operator=(const SetOfFileDescriptors&)=delete;
	SetOfFileDescriptors(const SetOfFileDescriptors&)=delete;

	std::unique_lock<std::recursive_mutex> getAndLockFD(FileDescriptor* &FDpointer, int FD);

	SystemInterface *systemWrap;

	std::unordered_map<int, FileDescriptor> openFDs;	//holds list of File Descriptors for all socket connections
	std::recursive_mutex FDmut; //OpenFD mutex

	std::vector<std::function<void(int)>> closeCallbacks; //array of functions to call when an FD is closed.  Each thread should add a callback so it can properly clean up when a connection is closed.

	std::vector<std::function<void(int)>> newConnectionCallbacks; //array of functions to call when a new connection is made.
};



#endif /* SERVER_SOCKET_SET_OF_FILE_DESCRIPTORS_H_ */
