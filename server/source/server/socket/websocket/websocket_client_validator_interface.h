#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_CLIENT_VALIDATOR_INTERFACE_H
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_CLIENT_VALIDATOR_INTERFACE_H
//#include "source/server/socket/websocket/websocket_client_validator_interface.h"

#include <string>

struct ConnectionHeaders
{
	std::string ip {};
	std::string port {};
	std::string secWebSocketProtocol {};
	std::string cookie {};
	std::string origin {};
};


/**
 * At bare minimum your implementation of areClientHeadersValid() should return false if
 * headers.origin is not equal to your site domain. Failure to do so will allow a
 * very simple cross site hijacking!!!
 */
class ClientValidatorInterface
{
public:
	virtual bool areClientHeadersValid(ConnectionHeaders &headers) = 0;
	virtual bool isClientIPValid(std::string &ip, std::string &port) = 0;
	virtual ~ClientValidatorInterface();
protected:
	ClientValidatorInterface() = default;
};


class MinimallySecureValidator : public ClientValidatorInterface
{

public:
	std::string websiteOrigin;

	MinimallySecureValidator(const std::string &_websiteOrigin): websiteOrigin(_websiteOrigin) {}

	bool areClientHeadersValid(ConnectionHeaders &headers) override
	{
		return headers.origin.compare(websiteOrigin) == 0;
	}
	bool isClientIPValid(std::string &ip, std::string &port) override
	{
		(void)ip;
		(void)port;
		//accept all traffic
		return true;
	}
	~MinimallySecureValidator() override = default;
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_CLIENT_VALIDATOR_INTERFACE_H */
