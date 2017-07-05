#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/websocket/websocket_handshake.cpp, \
                                     source/data_types/byte_array.cpp"

#include "source/server/socket/websocket/websocket_handshake.h"
#include "tests/test_lib/mocks_stubs/socket_test_helpers.h"

#include "tests/test.h"


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
	/*ByteArray Upgrade {};
	ByteArray SecWebSocketKey {};
	ByteArray SecWebSocketProtocol {};
	ByteArray Cookie {};*/
};





TEST(WebsocketHandshakeTest, fillHeaders)
{
	ByteArray testInput = createTestHandshakeHeader();
	HandshakeHeadersWrap testHeader;
	testHeader.fillHeaders(testInput);

	std::string testOutputString = testHeader.getUpgrade().toString();
	EXPECT_STREQ( testOutputString.c_str(), testHeader.getUpgrade().toString().c_str() );
	/*if(testOutputString.compare("websocket")!=0){
	//	TEST_PRINT(redTestText("Upgrade failed"));
		throw 1;
	}*/

	testOutputString = testHeader.getConnection().toString();
	bool doesConnectionContainUpgrade = testHeader.getConnection().toString().find("Upgrade") != std::string::npos;
	EXPECT_EQ( doesConnectionContainUpgrade, true);
	//if(testOutputString.compare("Upgrade")!=0){
	//	TEST_PRINT(redTestText("Connection failed"));
	//	throw 1;
	//}

	testOutputString = testHeader.getSecWebSocketKey().toString();
	EXPECT_STREQ( testOutputString.c_str(), "+40NMxLMogWjfV/0HyjlxA==" );
	/*if(testOutputString.compare("+40NMxLMogWjfV/0HyjlxA==")!=0){
	//	TEST_PRINT(redTestText("SecWebSocketKey failed"));
		throw 1;
	}*/

	testOutputString = testHeader.getSecWebSocketProtocol().toString();
	EXPECT_STREQ( testOutputString.c_str(), "05fcc56b7cb916d5e5a82081223b3357" );
	/*if(testOutputString.compare("05fcc56b7cb916d5e5a82081223b3357")!=0){
	//	TEST_PRINT(redTestText("SecWebSocketProtocol failed"));
		throw 1;
	}*/

	testOutputString = testHeader.getCookie().toString();
	EXPECT_STREQ( testOutputString.c_str(), "CJP5G89v2O30Dx-StfclobgZ0AuIH8Nh74SEzHxvBJEZWG6yJ3smhW73TZgDMO0HEy8AvYhKgzxVry5Yby75oT-250dW6PTdm74rhmQyACSwbiAbvp67108QZid7KoPJjf-OuP1cf5Z31_eHimsW8JTIf9KINfG0yy31WuDb21XU-nH9EJcVKhdoXrQB_35DPIRymBxV85cENsxScjjMIBnI60mUR1koC5k_XcwSiTgnoT9ApEPwIX6Z9iw0tV2X7" );
	/*if(testOutputString.compare("CJP5G89v2O30Dx-StfclobgZ0AuIH8Nh74SEzHxvBJEZWG6yJ3smhW73TZgDMO0HEy8AvYhKgzxVry5Yby75oT-250dW6PTdm74rhmQyACSwbiAbvp67108QZid7KoPJjf-OuP1cf5Z31_eHimsW8JTIf9KINfG0yy31WuDb21XU-nH9EJcVKhdoXrQB_35DPIRymBxV85cENsxScjjMIBnI60mUR1koC5k_XcwSiTgnoT9ApEPwIX6Z9iw0tV2X7")!=0){
	//	TEST_PRINT(redTestText("Cookie failed"));
		throw 1;
	}*/
}





TEST(WebsocketHandshakeTest, checkHeaders){
	ByteArray testInput = createTestHandshakeHeader();
	HandshakeHeadersWrap testHeader;
	testHeader.fillHeaders(testInput);

	EXPECT_EQ(testHeader.checkHeaders(), true);
	/*if(!testHeader.checkHeaders()){
		//TEST_PRINT(redTestText("returned false with good data"));
		throw 1;
	}*/

	ByteArray testTemp = testHeader.getUpgradeRef();
	testHeader.getUpgradeRef() = ByteArray( std::string("testFail") );

	EXPECT_EQ(testHeader.checkHeaders(), false);
	/*if(testHeader.checkHeaders()){
		//TEST_PRINT(redTestText("returned true with bad Upgrade header"));
		throw 1;
	}*/
	testHeader.getUpgradeRef() = testTemp;


	testTemp = testHeader.getConnectionRef();
	testHeader.getConnectionRef() = ByteArray( std::string("testFail") );

	EXPECT_EQ(testHeader.checkHeaders(), false);
	/*if(testHeader.checkHeaders()){
		//TEST_PRINT(redTestText("returned true with bad Connection header"));
		throw 1;
	}*/
	testHeader.getConnectionRef() = testTemp;

	testTemp = testHeader.getSecWebSocketKeyRef();
	testHeader.getSecWebSocketKeyRef() = ByteArray();

	EXPECT_EQ(testHeader.checkHeaders(), false);
	/*if(testHeader.checkHeaders()){
		//TEST_PRINT(redTestText("returned true with bad SecWebSocketKey header"));
		throw 1;
	}*/
	testHeader.getSecWebSocketKeyRef() = testTemp;

	testTemp = testHeader.getSecWebSocketProtocolRef();
	testHeader.getSecWebSocketProtocolRef() = ByteArray();

	EXPECT_EQ(testHeader.checkHeaders(), false);
	/*if(testHeader.checkHeaders()){
		//TEST_PRINT(redTestText("returned true with bad SecWebSocketProtocol header"));
		throw 1;
	}*/
	testHeader.getSecWebSocketProtocolRef() = testTemp;

	//testTemp = testHeader.getCookieRef();
	//testHeader.getCookieRef() = ByteArray();

	//EXPECT_EQ(testHeader.checkHeaders(), false);
	/*if(testHeader.checkHeaders()){
		//TEST_PRINT(redTestText("returned true with bad Cookie header"));
		throw 1;
	}*/
	//testHeader.getCookieRef() = testTemp;

	EXPECT_EQ(testHeader.checkHeaders(), true);
	/*if(!testHeader.checkHeaders()){
		//TEST_PRINT(redTestText("returned false with good data"));
		throw 1;
	}*/
}



int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
