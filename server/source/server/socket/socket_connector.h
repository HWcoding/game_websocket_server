#ifndef SERVER_SOCKET_SOCKET_CONNECTOR_H_
#define SERVER_SOCKET_SOCKET_CONNECTOR_H_
//#include "server/socket/socket_connector.h"

#include "source/server/socket/socket_node.h"
#include <string>
#include <vector>
#include <memory>

class SystemInterface;
class SetOfFileDescriptors;
class AuthenticatorInterface;
class ByteArray;
class ClientValidatorInterface;
struct epoll_event;

class SocketServerConnector : public SocketNode {
public:
	SocketServerConnector(std::string _port, SystemInterface *_systemWrap, SetOfFileDescriptors *FDs, std::atomic<bool>* run);
	void setClientValidator(ClientValidatorInterface * validator);
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

	SocketServerConnector& operator=(const SocketServerConnector&) = delete;
	SocketServerConnector(const SocketServerConnector&) = delete;
	std::unique_ptr<AuthenticatorInterface> Authenticator;
	size_t maxMessageSize;
	std::string port;
	int listeningFD;
};

#endif /* SERVER_SOCKET_SOCKET_CONNECTOR_H_ */
