#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_HANDSHAKE_H_
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_HANDSHAKE_H_

#include <unistd.h>
#include <vector>
#include <stdint.h>
#include "source/data_types/byte_array.h"


class HandshakeHeadersInterface {
public:
	virtual bool fillHeaders(const ByteArray &input) =0;
	virtual bool areHeadersFilled() const =0;

	virtual ByteArray getConnection() const =0;
	virtual ByteArray getCookie() const =0;
	virtual ByteArray getUpgrade() const =0;
	virtual ByteArray getSecWebSocketKey() const =0;
	virtual ByteArray getSecWebSocketProtocol() const =0;

	virtual ~HandshakeHeadersInterface();
protected:
	HandshakeHeadersInterface(){}
};

class HandshakeHeaders : public HandshakeHeadersInterface {
public:
	ByteArray getConnection() const;
	ByteArray getCookie() const;
	ByteArray getUpgrade() const;
	ByteArray getSecWebSocketKey() const;
	ByteArray getSecWebSocketProtocol() const;

	bool fillHeaders(const ByteArray &input);
	bool areHeadersFilled() const;
	HandshakeHeaders();
	HandshakeHeaders(const HandshakeHeaders& h) noexcept;
	HandshakeHeaders& operator=(const HandshakeHeaders& h) noexcept;
	HandshakeHeaders& operator=(HandshakeHeaders&& h) noexcept;

	~HandshakeHeaders(){}
protected:
	bool checkHeaders() const;
	ByteArray Connection {};
	ByteArray Upgrade {};
	ByteArray SecWebSocketKey {};
	ByteArray SecWebSocketProtocol {};
	ByteArray Cookie {};
private:
	bool filled {false};
};



#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_HANDSHAKE_H_ */
