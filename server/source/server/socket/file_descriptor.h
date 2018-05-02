#ifndef SOURCE_SERVER_SOCKET_FILE_DESCRIPTOR_H_
#define SOURCE_SERVER_SOCKET_FILE_DESCRIPTOR_H_
//#include "source/server/socket/file_descriptor.h"

#include <mutex>
#include <vector>
#include <string>
#include "source/data_types/byte_array.h"

class SystemWrapper;
struct epoll_event;

class FileDescriptor{
public:
	FileDescriptor();
	FileDescriptor(int _FD);
	FileDescriptor(FileDescriptor&& f) noexcept;
	FileDescriptor(const FileDescriptor& f) noexcept;
	FileDescriptor& operator=(FileDescriptor&& f);
	FileDescriptor& operator=(const FileDescriptor& f);
	~FileDescriptor();

	void stopPollingFD(int epoll);
	void startPollingForWrite(int epoll);
	void startPollingForRead(int epoll);
	void makeNonblocking();
	int getFD();
	ByteArray getIP();
	ByteArray getPort();
	ByteArray getCSRFkey();
	void setIP(ByteArray s);
	void setPort(ByteArray s);
	void setCSRFkey(ByteArray s);
	void setIP(std::string s);
	void setPort(std::string s);
	void setCSRFkey(std::string s);

private:
	void pollForWrite(int epoll);
	void pollForRead(int epoll);
	bool startPollingFD(epoll_event event, int epoll);
	void setFDReadWrite(epoll_event event, int epoll);

	SystemWrapper &systemWrap;
	mutable std::mutex mut {};
	ByteArray IP {};
	ByteArray port {};
	ByteArray CSRFkey {};
	int FD {-1};
};

#endif /* SOURCE_SERVER_SOCKET_FILE_DESCRIPTOR_H_ */
