/*#define TESTING //to activate conditional macros for test logging and access
#include "tests/test.h"
#include "main/includes.h"
#include <iostream>
#include <string>
#include "tests/server/mocks_stubs/mock_system_wrapper.h"
#include "tests/server/mocks_stubs/socket_test_helpers.h"
#include "tests/server/websocket_read_buffer_test/websocket_read_buffer_test.h"
#include "server/socket/set_of_file_descriptors.h"
#include "server/socket/websocket/websocket_read_buffer.h"
#include "engine/byte_array.h"

namespace WebsocketReadBuffer_Test{

void addMessage(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);

	std::string testString = generateTestString(101);
	ByteArray input(testString); //addMessage uses move if messageBuffer is empty. input is used instead of testString because its contents will be destroyed

	{
		WebsocketReadBuffers testBuffer(&FDs,100);
		try{
			testBuffer.addMessage(1, input);
			TEST_PRINT(redTestText("addMessage did not throw with too large of a size"));
			throw; //kill program
		}
		catch(...){}//proper behavior
	}

	WebsocketReadBuffers testBuffer(&FDs,202);
	input = ByteArray(testString);
	testBuffer.addMessage(1, input);
	std::string result = testBuffer.messageBuffer[1].buffer[0].toString();

	if(result.compare(testString) != 0){
		TEST_PRINT(redTestText("Buffer does not match test string"));
		throw 1;
	}

	input = ByteArray(testString);
	testBuffer.addMessage(1, input);
	result = testBuffer.messageBuffer[1].buffer[1].toString();

	if(result.compare(testString) != 0){
		TEST_PRINT(redTestText("Buffer does not match test string"));
		throw 1;
	}
}

void extractMessage(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);

	std::string testString = generateTestString(300);
	ByteArray input(testString);

	WebsocketReadBuffers testBuffer(&FDs,1000);
	testBuffer.addMessage(1, input);
	ByteArray result;

	if( !testBuffer.extractMessage(result, 0, 1) ){
		TEST_PRINT( redTestText("extractMessage did not return a complete message when size was unknown") );
		TEST_PRINT( redTestText("message size = "<<testBuffer.messageBuffer[1].totalSize<<"\nExpected size = "<<testBuffer.messageBuffer[1].expectedSize) );
		throw 1;
	}

	input =ByteArray(testString);
	testBuffer.addMessage(1, input);
	testBuffer.setMessageSize(1, 600);
	result =ByteArray();

	if( testBuffer.extractMessage(result, 0, 1) ){
		TEST_PRINT( redTestText("extractMessage returned a complete message when a partial message was given") );
		TEST_PRINT( redTestText("message size = "<<testBuffer.messageBuffer[1].totalSize<<"\nExpected size = "<<testBuffer.messageBuffer[1].expectedSize) );
		throw 1;
	}

	input = ByteArray(testString);
	testBuffer.addMessage(1, input);
	result =ByteArray();

	if( !testBuffer.extractMessage(result, 0, 1) ){
		TEST_PRINT( redTestText("extractMessage did not return a complete message when a complete message was given") );
		TEST_PRINT( redTestText("message size = "<<testBuffer.messageBuffer[1].totalSize<<"\nExpected size = "<<testBuffer.messageBuffer[1].expectedSize) );
		throw 1;
	}

	std::string stringResult = result.toString();
	testString.append(testString);
	if( stringResult.compare(testString) != 0 ){
		TEST_PRINT( redTestText("result does not equal input") );
		throw 1;
	}
}

void addFracture(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);

	std::string testString = generateTestString(300);
	ByteArray input(testString); //addFracture uses move if messageBuffer is empty. input is used instead of testString because its contents will be destroyed

	WebsocketReadBuffers testBuffer(&FDs,1000);
	input = ByteArray(testString);
	testBuffer.addFracture(1, input);
	std::string stringResult = testBuffer.fractureBuffer[1].buffer[0].toString();

	if(stringResult.compare(testString) != 0){
		TEST_PRINT(redTestText("Buffer does not match test string"));
		throw 1;
	}

	input = ByteArray(testString);
	testBuffer.addFracture(1, input);
	stringResult = testBuffer.fractureBuffer[1].buffer[1].toString();

	if(stringResult.compare(testString) != 0){
		TEST_PRINT(redTestText("Buffer does not match test string"));
		throw 1;
	}

	if(testBuffer.fractureBuffer[1].size() != 600){
		TEST_PRINT(redTestText("Buffer size is not correct"));
		throw 1;
	}
}

void extractFracture(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);

	std::string testString = generateTestString(300);
	ByteArray input(testString); //addFracture uses move if messageBuffer is empty. input is used instead of testString because its contents will be destroyed

	WebsocketReadBuffers testBuffer(&FDs,1000);
	input = ByteArray(testString);
	testBuffer.addFracture(1, input);
	input = ByteArray(testString);
	testBuffer.addFracture(1, input);

	ByteArray result;
	testBuffer.extractFracture(result, 0, 1);

	input = ByteArray(testString);
	testString.append(testString);
	std::string stringResult = result.toString();

	if( stringResult.compare(testString) != 0 ){
		TEST_PRINT( redTestText("result does not equal input") );
		throw 1;
	}
}

void test(){
	addMessage();
	extractMessage();
	addFracture();
	extractFracture();
}

}


int main(){
	WebsocketReadBuffer_Test::test();
	return 0;
}*/
int main(){return 0;}
