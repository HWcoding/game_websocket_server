#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_HANDSHAKE_INTERFACE_H_
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_HANDSHAKE_INTERFACE_H_

class ByteArray;

class HandshakeHeadersInterface {
public:
	virtual bool fillHeaders(const ByteArray &input) = 0;
	virtual bool areHeadersFilled() const = 0;

	virtual ByteArray getConnection() const = 0;
	virtual ByteArray getCookie() const = 0;
	virtual ByteArray getUpgrade() const = 0;
	virtual ByteArray getSecWebSocketKey() const = 0;
	virtual ByteArray getSecWebSocketProtocol() const = 0;
	virtual ByteArray getOrigin() const = 0;

	virtual ~HandshakeHeadersInterface();
protected:
	HandshakeHeadersInterface() = default;
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_HANDSHAKE_INTERFACE_H_ */
