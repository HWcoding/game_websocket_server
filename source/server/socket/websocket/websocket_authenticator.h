#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_AUTHENTICATOR_H_
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_AUTHENTICATOR_H_
//#include "source/server/socket/websocket/websocket_authenticator.h"

#include <unistd.h>
#include <unordered_map>
#include <vector>
#include "source/data_types/byte_array.h"
#include "source/server/socket/websocket/websocket_client_validator.h"
#include "source/server/socket/authenticator_interface.h"

class SystemInterface;
class SetOfFileDescriptors;
class HandshakeHeaders;


class WebsocketAuthenticator : public AuthenticatorInterface
{
public:
	WebsocketAuthenticator(SystemInterface *_systemWrap, SetOfFileDescriptors*FDs);
	~WebsocketAuthenticator(){}
	void processHandshake(const ByteArray &in, int FD);
	bool sendHandshake(int FD);
	void closeFD(int FD);
	bool isNotValidConnection(const ByteArray &IP, const ByteArray &port) const;
	void setClientValidator(ClientValidatorInterface * validator)
	{
		ClientValidator = validator;
	}

private:
	void checkForValidHeaders( int FD, HandshakeHeaders &headers) const;
	bool isHandshake(const ByteArray &in) const;
	bool isCompleteHandshake(const ByteArray &in) const;
	HandshakeHeaders getHandshakeHeaders(const ByteArray &handShake) const;
	ByteArray createHandshake(const HandshakeHeaders &headers) const;
	ByteArray createSecWebSocketAccept(const ByteArray &SecWebSocketKey) const;
	bool isHandshakeInvalid(const ByteArray &handShake) const;
	void toBase64(const ByteArray &in, ByteArray &out) const;
	uint8_t convertTo64(uint8_t in) const;

	WebsocketAuthenticator& operator=(const WebsocketAuthenticator&)=delete;
	WebsocketAuthenticator(const WebsocketAuthenticator&)=delete;
	SystemInterface *systemWrap;
	std::unordered_map<int,ByteArray > handshakeReadBuffer;
	std::unordered_map<int,ByteArray > handshakeWriteBuffer;
	size_t maxHandshakeSize;
	SetOfFileDescriptors *fileDescriptors;

	DefaultClientValidator defaultClientValidator {};
	ClientValidatorInterface * ClientValidator { &defaultClientValidator };
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_AUTHENTICATOR_H_ */
