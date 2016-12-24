#ifndef SERVER_SOCKET_SOCKET_WRITER_H_
#define SERVER_SOCKET_SOCKET_WRITER_H_
//#include "source/server/socket/socket_writer.h"

#include <mutex>
#include <memory>

class SystemInterface;
class SocketMessage;
class SetOfFileDescriptors;
class MessageSenderInterface;

class SocketWriterInterface{
public:
	virtual void startPoll() =0;
	virtual void closeFDHandler(int FD) =0;
	virtual void newConnectionHandler(int FD) =0;
	virtual void sendMessage(SocketMessage &message) =0;
	virtual ~SocketWriterInterface();
protected:
	SocketWriterInterface(){}
};

class SocketWriter : public SocketWriterInterface {
public:
	void startPoll();
	void closeFDHandler(int FD);
	void newConnectionHandler(int FD);
	void sendMessage(SocketMessage &message);

	SocketWriter(SystemInterface *_systemWrap, SetOfFileDescriptors *FDs, std::atomic<bool>* run);
	~SocketWriter();
private:
	void setupEpoll();
	void closeFD(int FD);
	void writeData(int FD);

	SocketWriter& operator=(const SocketWriter&)=delete;
	SocketWriter(const SocketWriter&)=delete;

	SystemInterface *systemWrap;
	std::unique_ptr<MessageSenderInterface> sender;
	std::mutex writePollingMut;
	SetOfFileDescriptors *fileDescriptors;
	std::atomic<bool> *running;
	int MAXEVENTS;
	int epollFD;
};

#endif /* SERVER_SOCKET_SOCKET_WRITER_H_ */
