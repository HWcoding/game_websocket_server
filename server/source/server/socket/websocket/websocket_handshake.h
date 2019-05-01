#ifndef SERVER_SOCKET_WEBSOCKET_WEBSOCKET_HANDSHAKE_H_
#define SERVER_SOCKET_WEBSOCKET_WEBSOCKET_HANDSHAKE_H_
//#include "source/server/socket/websocket/websocket_handshake.h"

#include "source/server/socket/websocket/websocket_handshake_interface.h"
#include <unistd.h>
#include <vector>
#include <stdint.h>
#include "source/data_types/byte_array.h"

class HandshakeHeaders : public HandshakeHeadersInterface {
public:
	ByteArray getConnection() const override;
	ByteArray getCookie() const override;
	ByteArray getUpgrade() const override;
	ByteArray getSecWebSocketKey() const override;
	ByteArray getSecWebSocketProtocol() const override;
	ByteArray getOrigin() const override;

	bool fillHeaders(const ByteArray &input) override;
	bool areHeadersFilled() const override;
	HandshakeHeaders();
	HandshakeHeaders(const HandshakeHeaders& h) noexcept;
	HandshakeHeaders& operator=(const HandshakeHeaders& h) noexcept;
	HandshakeHeaders& operator=(HandshakeHeaders&& h) noexcept;

	~HandshakeHeaders() override = default;
protected:
	bool checkHeaders() const;
	ByteArray Connection {};
	ByteArray Upgrade {};
	ByteArray SecWebSocketKey {};
	ByteArray SecWebSocketProtocol {};
	ByteArray Cookie {};
	ByteArray Origin {};
private:
	bool filled {false};
};

#endif /* SERVER_SOCKET_WEBSOCKET_WEBSOCKET_HANDSHAKE_H_ */
