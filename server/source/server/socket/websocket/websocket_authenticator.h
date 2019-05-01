#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_AUTHENTICATOR_H_
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_AUTHENTICATOR_H_
//#include "source/server/socket/websocket/websocket_authenticator.h"

#include <unistd.h>
#include <unordered_map>
#include <vector>
#include "source/data_types/byte_array.h"
#include "source/server/socket/websocket/websocket_client_validator.h"
#include "source/server/socket/authenticator_interface.h"

class SetOfFileDescriptors;
class HandshakeHeaders;
class HandshakeHeadersInterface;

class WebsocketAuthenticator : public AuthenticatorInterface
{
public:
	WebsocketAuthenticator(SetOfFileDescriptors*FDs);
	~WebsocketAuthenticator() override = default;
	bool processHandshake(const ByteArray &in, int FD) override;
	bool sendHandshake(int FD) override;
	void closeFD(int FD) override;
	bool isNotValidConnection(const ByteArray &IP, const ByteArray &port) const override;
	void setClientValidator(ClientValidatorInterface * validator) override
	{
		ClientValidator = validator;
	}

private:
	void checkForValidHeaders( int FD, const HandshakeHeadersInterface &headers) const;
	static bool isHandshake(const ByteArray &in);
	static bool isCompleteHandshake(const ByteArray &in);
	static HandshakeHeaders getHandshakeHeaders(const ByteArray &in);
	static ByteArray createHandshake(const HandshakeHeadersInterface &headers);
	static ByteArray createSecWebSocketAccept(const ByteArray &SecWebSocketKey);
	static bool isHandshakeInvalid(const ByteArray &handShake);

	WebsocketAuthenticator& operator=(const WebsocketAuthenticator&) = delete;
	WebsocketAuthenticator(const WebsocketAuthenticator&) = delete;
	std::unordered_map<int,ByteArray > handshakeReadBuffer;
	std::unordered_map<int,ByteArray > handshakeWriteBuffer;
	size_t maxHandshakeSize;
	SetOfFileDescriptors *fileDescriptors;

	DefaultClientValidator defaultClientValidator {};
	ClientValidatorInterface * ClientValidator { &defaultClientValidator };
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_AUTHENTICATOR_H_ */
