#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/websocket/websocket_handshake.cpp, \
                                     source/data_types/byte_array.cpp"

#include "source/server/socket/websocket/websocket_handshake.h"
#include "source/data_types/byte_array.h"
#include "tests/test_lib/mocks_stubs/socket_test_helpers.h"

#include "tests/test.h"

ByteArray createTestHandshakeHeader(){
	ByteArray output;

	output.appendWithNoSize(std::string("GET /socket HTTP/1.1\r\n") );
	output.appendWithNoSize(std::string("Host: localhost\r\n") );
	output.appendWithNoSize(std::string("Connection: keep-alive, Upgrade\r\n") );
	output.appendWithNoSize(std::string("Pragma: no-cache\r\n") );
	output.appendWithNoSize(std::string("Cache-Control: no-cache\r\n") );
	output.appendWithNoSize(std::string("Upgrade: websocket\r\n") );
	output.appendWithNoSize(std::string("Origin: http://localhost:8080\r\n") );
	output.appendWithNoSize(std::string("Sec-WebSocket-Version: 13\r\n") );
	output.appendWithNoSize(std::string("User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.94 Safari/537.36\r\n") );
	output.appendWithNoSize(std::string("Accept-Encoding: gzip, deflate, sdch\r\n") );
	output.appendWithNoSize(std::string("Accept-Language: en-US,en;q=0.8\r\n") );
	output.appendWithNoSize(std::string("Cookie: GameServer=CJP5G89v2O30Dx-StfclobgZ0AuIH8Nh74SEzHxvBJEZWG6yJ3smhW73TZgDMO0HEy8AvYhKgzxVry5Yby75oT-250dW6PTdm74rhmQyACSwbiAbvp67108QZid7KoPJjf-OuP1cf5Z31_eHimsW8JTIf9KINfG0yy31WuDb21XU-nH9EJcVKhdoXrQB_35DPIRymBxV85cENsxScjjMIBnI60mUR1koC5k_XcwSiTgnoT9ApEPwIX6Z9iw0tV2X7\r\n") );
	output.appendWithNoSize(std::string("Sec-WebSocket-Key: +40NMxLMogWjfV/0HyjlxA==\r\n") );
	output.appendWithNoSize(std::string("Sec-WebSocket-Extensions: permessage-deflate; client_max_window_bits\r\n") );
	output.appendWithNoSize(std::string("Sec-WebSocket-Protocol: 05fcc56b7cb916d5e5a82081223b3357\r\n\r\n") );
	return output;
}

class HandshakeHeadersWrap : public HandshakeHeaders
{
public:
	HandshakeHeadersWrap(): HandshakeHeaders() {}
	HandshakeHeadersWrap(const HandshakeHeaders& h) noexcept : HandshakeHeaders(h){}
	bool checkHeaders() const
	{
		return HandshakeHeaders::checkHeaders();
	}

	ByteArray & getUpgradeRef()
	{
		return Upgrade;
	}
	ByteArray & getConnectionRef()
	{
		return Connection;
	}
	ByteArray & getSecWebSocketKeyRef()
	{
		return SecWebSocketKey;
	}
	ByteArray & getSecWebSocketProtocolRef()
	{
		return SecWebSocketProtocol;
	}
	ByteArray & getCookieRef()
	{
		return Cookie;
	}
};




TEST(WebsocketHandshakeTest, fillHeaders)
{
	ByteArray testInput = createTestHandshakeHeader();
	HandshakeHeadersWrap testHeader;
	testHeader.fillHeaders(testInput);

	std::string testOutputString = testHeader.getUpgrade().toString();
	EXPECT_STREQ( testOutputString.c_str(), "websocket");

	testOutputString = testHeader.getConnection().toString();
	EXPECT_STREQ( testOutputString.c_str(), "keep-alive, Upgrade");

	testOutputString = testHeader.getSecWebSocketKey().toString();
	EXPECT_STREQ( testOutputString.c_str(), "+40NMxLMogWjfV/0HyjlxA==" );

	testOutputString = testHeader.getSecWebSocketProtocol().toString();
	EXPECT_STREQ( testOutputString.c_str(), "05fcc56b7cb916d5e5a82081223b3357" );

	testOutputString = testHeader.getCookie().toString();
	EXPECT_STREQ( testOutputString.c_str(),
		"CJP5G89v2O30Dx-StfclobgZ0AuIH8Nh74SEzHxvBJEZWG6yJ3smhW73TZgDMO0HEy"
		"8AvYhKgzxVry5Yby75oT-250dW6PTdm74rhmQyACSwbiAbvp67108QZid7KoPJjf-O"
		"uP1cf5Z31_eHimsW8JTIf9KINfG0yy31WuDb21XU-nH9EJcVKhdoXrQB_35DPIRymB"
		"xV85cENsxScjjMIBnI60mUR1koC5k_XcwSiTgnoT9ApEPwIX6Z9iw0tV2X7");
}

TEST(WebsocketHandshakeTest, checkHeaders){
	ByteArray testInput = createTestHandshakeHeader();
	HandshakeHeadersWrap testHeader;
	testHeader.fillHeaders(testInput);

	// check returns true with good data
	EXPECT_EQ(testHeader.checkHeaders(), true);


	ByteArray testTemp = testHeader.getUpgradeRef();
	testHeader.getUpgradeRef() = ByteArray( std::string("testFail") );

	// check returns false with empty Upgrade header
	EXPECT_EQ(testHeader.checkHeaders(), false);


	testHeader.getUpgradeRef() = testTemp;
	testTemp = testHeader.getConnectionRef();
	testHeader.getConnectionRef() = ByteArray( std::string("testFail") );

	// check returns false with empty Connection header
	EXPECT_EQ(testHeader.checkHeaders(), false);

	testHeader.getConnectionRef() = testTemp;
	testTemp = testHeader.getSecWebSocketKeyRef();
	testHeader.getSecWebSocketKeyRef() = ByteArray();

	// check returns false with empty SecWebSocketKey header
	EXPECT_EQ(testHeader.checkHeaders(), false);

	testHeader.getSecWebSocketKeyRef() = testTemp;
	testTemp = testHeader.getSecWebSocketProtocolRef();
	testHeader.getSecWebSocketProtocolRef() = ByteArray();

	// check returns false with empty SecWebSocketProtocol header
	EXPECT_EQ(testHeader.checkHeaders(), false);

	testHeader.getSecWebSocketProtocolRef() = testTemp;
	testTemp = testHeader.getCookieRef();
	testHeader.getCookieRef() = ByteArray(generateTestString(513));


	// check returns false with large Cookie header
	EXPECT_EQ(testHeader.checkHeaders(), false);
	testHeader.getCookieRef() = testTemp;

	// check still returns true with good data
	EXPECT_EQ(testHeader.checkHeaders(), true);
}



int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
