/*#define TESTING //to activate conditional macros for test logging and access
#include "tests/test.h"

#include <iostream>
#include <string>

#include "main/includes.h"
#include "server/socket/websocket/websocket_authenticator.h"
#include "server/socket/set_of_file_descriptors.h"
#include "server/socket/websocket/websocket_authenticator.h"
#include "server/socket/websocket/websocket_handshake.h"
#include "engine/byte_array.h"

#include "tests/server/mocks_stubs/mock_system_wrapper.h"
#include "tests/server/mocks_stubs/socket_test_helpers.h"
#include "tests/server/websocket_authenticator_test/websocket_authenticator_test.h"
#include "tests/server/websocket_handshake_test/websocket_handshake_test.h"


namespace WebsocketAuthenticator_Test{

void test_IsNotValidConnection(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);
*/

/*	char nullChar = '\0';
	char *hbuf = &nullChar;
	char *sbuf = &nullChar;

	if(authenticator.isNotValidConnection(1,hbuf,sbuf)){
		TEST_PRINT(redTestText("validate returned true with good data"));
		throw 1;
	}

	if(!authenticator.isNotValidConnection(0,hbuf,sbuf)){
		TEST_PRINT(redTestText("validate returned false with bad connection"));
		throw 1;
	}

	hbuf =NULL;
	if(!authenticator.isNotValidConnection(1,hbuf,sbuf)){
		TEST_PRINT(redTestText("validate returned false with bad hbuf"));
		throw 1;
	}
	hbuf =&nullChar;

	sbuf =NULL;
	if(!authenticator.isNotValidConnection(1,hbuf,sbuf)){
		TEST_PRINT(redTestText("validate returned false with bad sbuf"));
		throw 1;
	}
	sbuf =&nullChar;

	if(authenticator.isNotValidConnection(1,hbuf,sbuf)){
		TEST_PRINT(redTestText("validate returned true with good data"));
		throw 1;
	}*//*
}

void test_IsHandshake(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);

	//std::string testString("GET /socket HTTP/1.1");
	ByteArray testString( std::string("GET /socket HTTP/1.1") );
	if(!authenticator.isHandshake(testString)){
		TEST_PRINT(redTestText("returned false on good handshake"));
		throw 1;
	}

	//testString = std::string("POST /socket HTTP/1.1");
	testString = ByteArray( std::string("POST /socket HTTP/1.1") );
	if(authenticator.isHandshake(testString)){
		TEST_PRINT(redTestText("returned true on bad handshake"));
		throw 1;
	}
}

void test_IsCompleteHandshake(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);

	ByteArray testString = createTestHandshakeHeader();

	if(!authenticator.isCompleteHandshake(testString)){
		TEST_PRINT(redTestText("returned false on good handshake"));
		throw 1;
	}

	testString[testString.size()-1] = '\0';
	testString[testString.size()-2] = '\0';
	if(authenticator.isCompleteHandshake(testString)){
		TEST_PRINT(redTestText("returned true on bad handshake"));
		throw 1;
	}
}

void test_CloseFD(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);

	authenticator.handshakeReadBuffer[1]= ByteArray( std::string("test") );
	authenticator.handshakeWriteBuffer[1]= ByteArray( std::string("test") );

	authenticator.closeFD(1);

	if(authenticator.handshakeReadBuffer.count(1) != 0){
		TEST_PRINT(redTestText("handshakeReadBuffer not cleared"));
		throw 1;
	}

	if(authenticator.handshakeWriteBuffer.count(1) != 0){
		TEST_PRINT(redTestText("handshakeWriteBuffer not cleared"));
		throw 1;
	}
}

void test_GetHandshakeHeaders(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);

	ByteArray IP( std::string("IP") );
	ByteArray PORT( std::string("PORT") );
	ByteArray KEY( std::string("KEY") );

	FDs.addFD(1);
	FDs.setIP(1,IP);
	FDs.setPort(1,PORT);
	FDs.setCSRFkey(1,KEY );
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);

	ByteArray testString = createTestHandshakeHeader();
	HandshakeHeaders testHeader = authenticator.getHandshakeHeaders(testString);

	std::string testUpgrade = testHeader.Upgrade.toString();

	if(testUpgrade.compare("websocket")!=0){
		TEST_PRINT(redTestText("failed"));
		throw 1;
	}
}

void test_CreateHandshake(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);

	ByteArray IP( std::string("IP") );
	ByteArray PORT( std::string("PORT") );
	ByteArray KEY( std::string("KEY") );

	FDs.addFD(1);
	FDs.setIP(1,IP);
	FDs.setPort(1,PORT);
	FDs.setCSRFkey(1,KEY );
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);

	ByteArray testString = createTestHandshakeHeader();
	HandshakeHeaders testHeader = authenticator.getHandshakeHeaders(testString);

	std::string result = authenticator.createHandshake(testHeader).toString();

	std::string expectedOutput("HTTP/1.1 101 Switching Protocols\r\n");
	expectedOutput.append("Upgrade: websocket\r\n");
	expectedOutput.append("Connection: Upgrade\r\n");
	expectedOutput.append("Sec-WebSocket-Accept: XvS4xrxcXUWz3C5CU/McPLRYBFY=\r\n");
	expectedOutput.append("Sec-WebSocket-Protocol: 05fcc56b7cb916d5e5a82081223b3357\r\n\r\n");

	if( expectedOutput.compare(result) != 0){
		TEST_PRINT(redTestText("failed"));
		throw 1;
	}
}

void test_CreateSecWebSocketAccept(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);

	ByteArray input( std::string("+40NMxLMogWjfV/0HyjlxA==") );

	std::string output = authenticator.createSecWebSocketAccept(input).toString();

	if( output.compare(std::string("XvS4xrxcXUWz3C5CU/McPLRYBFY=")) != 0){
		TEST_PRINT(redTestText("failed"));
		throw 1;
	}
}

void test_IsHandshakeInvalid(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);

	ByteArray IP( std::string("IP") );
	ByteArray PORT( std::string("PORT") );
	ByteArray KEY( std::string("KEY") );

	FDs.addFD(1);
	FDs.setIP(1,IP);
	FDs.setPort(1,PORT);
	FDs.setCSRFkey(1,KEY );

	WebsocketAuthenticator authenticator(&systemWrap, &FDs);

	ByteArray testString = createTestHandshakeHeader();
	ByteArray largeString = testString;
	//ByteArray appendString(generateTestString(2050));
	//size_t end = largeString.size();
	//largeString.resize(end+appendString.size());
	//memcpy(&largeString[end], &appendString[0], appendString.size());
	largeString.appendNoNull(generateTestString(2050));

	if(!authenticator.isHandshakeInvalid(largeString)){
		TEST_PRINT(redTestText("authenticated too large of a handshake"));
		throw 1;
	}

	if(authenticator.isHandshakeInvalid(testString)){
		TEST_PRINT(redTestText("did not authenticated a valid handshake"));
		throw 1;
	}

	testString = ByteArray( std::string("POST /socket HTTP/1.1") );
	if(!authenticator.isHandshakeInvalid(testString)){
		TEST_PRINT(redTestText("authenticated an invalid handshake"));
		throw 1;
	}

	testString = ByteArray( std::string("GE") );
	if(!authenticator.isHandshakeInvalid(testString)){
		TEST_PRINT(redTestText("authenticated an incomplete handshake"));
		throw 1;
	}
}

void test_ConvertTo64(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);

	if(authenticator.convertTo64(25) != 90){
		TEST_PRINT(redTestText("failed"));
		throw 1;
	}

	if(authenticator.convertTo64(51) != 122){
		TEST_PRINT(redTestText("failed"));
		throw 1;
	}

	if(authenticator.convertTo64(61) != 57){
		TEST_PRINT(redTestText("failed"));
		throw 1;
	}

	if(authenticator.convertTo64(62) != 43){
		TEST_PRINT(redTestText("failed"));
		throw 1;
	}

	if(authenticator.convertTo64(63) != 47){
		TEST_PRINT(redTestText("failed"));
		throw 1;
	}

	if(authenticator.convertTo64(64) != '\\'){
		TEST_PRINT(redTestText("failed"));
		throw 1;
	}
}

void test_ToBase64(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);

	ByteArray overTwenty( std::string("123456789012345678901") );
	ByteArray output;

	authenticator.toBase64(overTwenty, output);
	std::string outputString = output.toString();
	if(outputString.compare("")!=0){
		TEST_PRINT(redTestText("processed too large of a string"));
		throw 1;
	}

	ByteArray twenty( std::string("12345678901234567890") );
	output = ByteArray();
	authenticator.toBase64(twenty, output);
	outputString = output.toString();
	if(outputString.compare("MTIzNDU2Nzg5MDEyMzQ1Njc4OTA=")!=0){
		TEST_PRINT(redTestText("wrong output"));
		throw 1;
	}
}

void test_ProcessHandshake(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);

	{
		//std::string largeString = createTestHandshakeHeader();
		//largeString.append(generateTestString(authenticator.maxHandshakeSize+1));

		ByteArray largeString = createTestHandshakeHeader();
		//ByteArray appendString(generateTestString(authenticator.maxHandshakeSize+1));
		//size_t end = largeString.size();
		//largeString.resize(end+appendString.size());
		//memcpy(&largeString[end], &appendString[0], appendString.size());
		largeString.appendNoNull(generateTestString(authenticator.maxHandshakeSize+1));

		try{
			authenticator.processHandshake(largeString,1);
			TEST_PRINT(redTestText("processHandshake did not throw with too large a string size"));
			throw; //kill test
		}
		catch(...){}//correct behavior
	}

	ByteArray testString = createTestHandshakeHeader();
	try{
		authenticator.processHandshake(testString,1);
	}
	catch(...){
		TEST_PRINT(redTestText("processHandshake threw with valid data"));
		throw;
	}
}

void test_SendHandshake(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);

	ByteArray IP( std::string("IP") );
	ByteArray PORT( std::string("PORT") );
	ByteArray KEY( std::string("KEY") );

	FDs.addFD(1);
	FDs.setIP(1,IP);
	FDs.setPort(1,PORT);
	FDs.setCSRFkey(1,KEY );

	WebsocketAuthenticator authenticator(&systemWrap, &FDs);

	ByteArray testString = createTestHandshakeHeader();
	authenticator.processHandshake(testString,1);

	std::string expectedOutput("HTTP/1.1 101 Switching Protocols\r\n");
	expectedOutput.append("Upgrade: websocket\r\n");
	expectedOutput.append("Connection: Upgrade\r\n");
	expectedOutput.append("Sec-WebSocket-Accept: XvS4xrxcXUWz3C5CU/McPLRYBFY=\r\n");
	expectedOutput.append("Sec-WebSocket-Protocol: 05fcc56b7cb916d5e5a82081223b3357\r\n\r\n");

	if(!authenticator.sendHandshake(1)){
		TEST_PRINT(redTestText("reported did not send entire handshake when it did"));
		throw 1;
	}

	systemWrap.SetBytesTillWriteFail(1, 9);
	authenticator.processHandshake(testString,1);
	if(authenticator.sendHandshake(1)){
		TEST_PRINT(redTestText("reported it sent entire handshake when it did not"));
		throw 1;
	}

	if(authenticator.handshakeWriteBuffer[1].size() != expectedOutput.size()-9){
		TEST_PRINT(redTestText("buffer not properly saved after write"));
		throw 1;
	}

	systemWrap.SetBytesTillWriteFail(1, -1);
	authenticator.processHandshake(testString,1);
	if(!authenticator.sendHandshake(1)){
		TEST_PRINT(redTestText("final part of buffer not sent properly"));
		throw 1;
	}

	if(authenticator.handshakeWriteBuffer.count(1) != 0){
		TEST_PRINT(redTestText("buffer not cleared after empty"));
		throw 1;
	}
}

void test(){
	test_IsNotValidConnection();
	test_IsHandshake();
	test_IsCompleteHandshake();
	test_CloseFD();
	test_GetHandshakeHeaders();
	test_CreateSecWebSocketAccept();
	test_IsHandshakeInvalid();
	test_ConvertTo64();
	test_ToBase64();
	test_CreateHandshake();
	test_ProcessHandshake();
	test_SendHandshake();
}

}

int main(){
	WebsocketAuthenticator_Test::test();
	return 0;
}*/
int main(){return 0;}
