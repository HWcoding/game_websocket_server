#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_CLIENT_VALIDATOR_INTERFACE_H
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_CLIENT_VALIDATOR_INTERFACE_H
//#include "source/server/socket/websocket/websocket_client_validator_interface.h"

#include <string>

struct ConnectionHeaders
{
	std::string IP {};
	std::string port {};
	std::string SecWebSocketProtocol {};
	std::string Cookie {};
};

class ClientValidatorInterface
{
public:
	virtual bool areClientHeadersValid(ConnectionHeaders &headers) = 0;
	virtual bool isClientIPValid(std::string &IP, std::string &port) = 0;
	virtual ~ClientValidatorInterface();
protected:
	ClientValidatorInterface() = default;
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_CLIENT_VALIDATOR_INTERFACE_H */
