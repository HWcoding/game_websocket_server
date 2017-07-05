#ifndef SOURCE_SERVER_SOCKET_AUTHENTICATOR_INTERFACE_H_
#define SOURCE_SERVER_SOCKET_AUTHENTICATOR_INTERFACE_H_
//#include "source/server/socket/authenticator_interface.h"

class ByteArray;
class ClientValidatorInterface;

class AuthenticatorInterface
{
public:
	virtual void processHandshake(const ByteArray &in, int FD) = 0;
	virtual bool sendHandshake(int FD) = 0;
	virtual void closeFD(int FD) = 0;
	virtual bool isNotValidConnection(const ByteArray &IP, const ByteArray &port) const = 0;
	virtual void setClientValidator(ClientValidatorInterface * validator) = 0;
	virtual ~AuthenticatorInterface();
protected:
	AuthenticatorInterface() = default;
};

#endif /* SOURCE_SERVER_SOCKET_AUTHENTICATOR_INTERFACE_H_ */