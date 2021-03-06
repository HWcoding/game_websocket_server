#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_CLIENT_VALIDATOR_H
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_CLIENT_VALIDATOR_H
//#include "source/server/socket/websocket/websocket_client_validator.h"

#include "source/server/socket/websocket/websocket_client_validator_interface.h"

class DefaultClientValidator : public ClientValidatorInterface
{
public:
	bool areClientHeadersValid(ConnectionHeaders &headers) override
	{
		(void)headers;
		//reject all traffic
		return false;
	}
	bool isClientIPValid(std::string &IP, std::string &port) override
	{
		(void)IP;
		(void)port;
		//reject all traffic
		return false;
	}
	~DefaultClientValidator() override = default;
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_CLIENT_VALIDATOR_H */
