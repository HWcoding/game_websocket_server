#ifndef SERVER_SOCKET_SOCKET_CONNECTOR_H_
#define SERVER_SOCKET_SOCKET_CONNECTOR_H_
//#include "server/socket/socket_connector.h"

#include <string>
#include <vector>
#include <memory>

class SystemInterface;
class SetOfFileDescriptors;
class WebsocketAuthenticator;
class ByteArray;
struct epoll_event;


class SocketServerConnector{
public:
	void startPoll();
	SocketServerConnector(const std::string &_port, SystemInterface *_systemWrap, SetOfFileDescriptors *FDs, bool* run);
	~SocketServerConnector();

private:
	void setupEpoll();
	bool handleEpollErrors(epoll_event &event);
	void handleEpollRead(epoll_event &event);
	void handleEpollWrite(epoll_event &event);
	void createAddressInfoHints(struct addrinfo &hints);
	int getListeningPort();
	bool createAndBindListeningFD(struct addrinfo *addressInfo);
	void openListeningPort();
	void getPortAndIP(int FD, struct sockaddr &in_addr, unsigned int &in_len, ByteArray &PortBuff, ByteArray &IPBuff);
	void newConnection();
	void waitForHandshake(int FD);
	void readHandshake(int FD);
	void processHandshake(ByteArray &in, int FD);
	void sendHandshake(int FD);
	void handshakeComplete(int FD);
	void closeFD(int FD);
	int addFD(int FD);

	SocketServerConnector& operator=(const SocketServerConnector&)=delete;
	SocketServerConnector(const SocketServerConnector&)=delete;

	SystemInterface *systemWrap;
	std::unique_ptr<WebsocketAuthenticator> Authenticator;
	bool *running;
	size_t maxMessageSize;
	SetOfFileDescriptors *fileDescriptors;
	std::string port;
	int MAXEVENTS;
	int epollFD;
	int listeningFD;
};

#endif /* SERVER_SOCKET_SOCKET_CONNECTOR_H_ */
