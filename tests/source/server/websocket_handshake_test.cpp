/*#define TESTING //to activate conditional macros for test logging and access
#include "tests/test.h"

#include <iostream>
#include <string>
#include "server/socket/websocket/websocket_handshake.h"
#include "engine/byte_array.h"

#include "tests/server/mocks_stubs/mock_system_wrapper.h"
#include "tests/server/mocks_stubs/socket_test_helpers.h"
#include "tests/server/websocket_handshake_test/websocket_handshake_test.h"
#include "main/includes.h"


namespace WebsocketHandshake_Test{


void test_FillHeaders(){
	ByteArray testInput = createTestHandshakeHeader();
	HandshakeHeaders testHeader;
	testHeader.fillHeaders(testInput);

	std::string testOutputString = testHeader.Upgrade.toString();
	if(testOutputString.compare("websocket")!=0){
		TEST_PRINT(redTestText("Upgrade failed"));
		throw 1;
	}

	testOutputString = testHeader.Connection.toString();
	if(testOutputString.compare("Upgrade")!=0){
		TEST_PRINT(redTestText("Connection failed"));
		throw 1;
	}

	testOutputString = testHeader.SecWebSocketKey.toString();
	if(testOutputString.compare("+40NMxLMogWjfV/0HyjlxA==")!=0){
		TEST_PRINT(redTestText("SecWebSocketKey failed"));
		throw 1;
	}

	testOutputString = testHeader.SecWebSocketProtocol.toString();
	if(testOutputString.compare("05fcc56b7cb916d5e5a82081223b3357")!=0){
		TEST_PRINT(redTestText("SecWebSocketProtocol failed"));
		throw 1;
	}

	testOutputString = testHeader.Cookie.toString();
	if(testOutputString.compare("CJP5G89v2O30Dx-StfclobgZ0AuIH8Nh74SEzHxvBJEZWG6yJ3smhW73TZgDMO0HEy8AvYhKgzxVry5Yby75oT-250dW6PTdm74rhmQyACSwbiAbvp67108QZid7KoPJjf-OuP1cf5Z31_eHimsW8JTIf9KINfG0yy31WuDb21XU-nH9EJcVKhdoXrQB_35DPIRymBxV85cENsxScjjMIBnI60mUR1koC5k_XcwSiTgnoT9ApEPwIX6Z9iw0tV2X7")!=0){
		TEST_PRINT(redTestText("Cookie failed"));
		throw 1;
	}
}

void test_CheckHeaders(){
	ByteArray testInput = createTestHandshakeHeader();
	HandshakeHeaders testHeader;
	testHeader.fillHeaders(testInput);

	if(!testHeader.checkHeaders()){
		TEST_PRINT(redTestText("returned false with good data"));
		throw 1;
	}

	ByteArray testTemp = testHeader.Upgrade;
	testHeader.Upgrade = ByteArray( std::string("testFail") );
	if(testHeader.checkHeaders()){
		TEST_PRINT(redTestText("returned true with bad Upgrade header"));
		throw 1;
	}
	testHeader.Upgrade = testTemp;


	testTemp = testHeader.Connection;
	testHeader.Connection = ByteArray( std::string("testFail") );
	if(testHeader.checkHeaders()){
		TEST_PRINT(redTestText("returned true with bad Connection header"));
		throw 1;
	}
	testHeader.Connection = testTemp;

	testTemp = testHeader.SecWebSocketKey;
	testHeader.SecWebSocketKey = ByteArray();
	if(testHeader.checkHeaders()){
		TEST_PRINT(redTestText("returned true with bad SecWebSocketKey header"));
		throw 1;
	}
	testHeader.SecWebSocketKey = testTemp;

	testTemp = testHeader.SecWebSocketProtocol;
	testHeader.SecWebSocketProtocol = ByteArray();
	if(testHeader.checkHeaders()){
		TEST_PRINT(redTestText("returned true with bad SecWebSocketProtocol header"));
		throw 1;
	}
	testHeader.SecWebSocketProtocol = testTemp;

	testTemp = testHeader.Cookie;
	testHeader.Cookie = ByteArray();
	if(testHeader.checkHeaders()){
		TEST_PRINT(redTestText("returned true with bad Cookie header"));
		throw 1;
	}
	testHeader.Cookie = testTemp;

	if(!testHeader.checkHeaders()){
		TEST_PRINT(redTestText("returned false with good data"));
		throw 1;
	}
}

void test(){
	test_FillHeaders();
	test_CheckHeaders();
}

}

int main(){
	WebsocketHandshake_Test::test();
	return 0;
}*/
int main(){return 0;}
