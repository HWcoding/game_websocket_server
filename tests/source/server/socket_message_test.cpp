/*#define TESTING //to activate conditional macros for test logging and access
#include "tests/test.h"
#include "main/includes.h"
//#include <iostream>
#include <string>
#include "tests/server/mocks_stubs/mock_system_wrapper.h"
#include "tests/server/mocks_stubs/socket_test_helpers.h"
#include "tests/server/socket_message_test/socket_message_test.h"
//#include "server/socket/set_of_file_descriptors.h"
#include "server/socket/socket_message.h"
#include "engine/byte_array.h"

namespace SocketMessage_Test{

ByteArray serializeToBytes(const void * bytes, size_t size);

ByteArray serializeToBytes(const void * bytes, size_t size){
	ByteArray output;
	output.resize(size);
	memcpy(&output[0], bytes, size);
	return output;
}


void test_GetNextFloat(){
	float testBuffer[8] = {0.0, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7};
	ByteArray testMessage = serializeToBytes(&testBuffer[0], sizeof(float)*8 );

	SocketMessage message(1, 2,
	ByteArray( std::string("IP") ),
	ByteArray( std::string("Port") ),
	ByteArray( std::string("CSRFkey") ),
	testMessage );

	for(int i=0; i<8; ++i){
		float nextValue =  message.getNextFloat();
		if(testBuffer[i] != nextValue){
			TEST_PRINT(redTestText("Input does not match output. Input: "<<testBuffer[i]<<" Output: "<<nextValue));
			throw 1;
		}
	}
	try{
		message.getNextFloat(); //should throw
		TEST_PRINT(redTestText("did not throw when trying to read past end of buffer"));
		throw; //kill test
	}
	catch(...){}//proper behavior
}

void test_GetNextUint32(){
	uint32_t testBuffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	ByteArray testMessage = serializeToBytes(&testBuffer[0], sizeof(uint32_t)*8 );

	SocketMessage message(1, 2,
	ByteArray( std::string("IP") ),
	ByteArray( std::string("Port") ),
	ByteArray( std::string("CSRFkey") ),
	testMessage );

	for(int i=0; i<8; ++i){
		uint32_t nextValue =  message.getNextUint32();
		if(testBuffer[i] != nextValue){
			TEST_PRINT(redTestText("Input does not match output. Input: "<<testBuffer[i]<<" Output: "<<nextValue));
			throw 1;
		}
	}
	try{
		message.getNextUint32(); //should throw
		TEST_PRINT(redTestText("did not throw when trying to read past end of buffer"));
		throw; //kill test
	}
	catch(...){}//proper behavior
}

void test_GetNextInt32(){
	int32_t testBuffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	ByteArray testMessage = serializeToBytes(&testBuffer[0], sizeof(int32_t)*8 );

	SocketMessage message(1, 2,
	ByteArray( std::string("IP") ),
	ByteArray( std::string("Port") ),
	ByteArray( std::string("CSRFkey") ),
	testMessage );

	for(int i=0; i<8; ++i){
		int32_t nextValue =  message.getNextInt32();
		if(testBuffer[i] != nextValue){
			TEST_PRINT(redTestText("Input does not match output. Input: "<<testBuffer[i]<<" Output: "<<nextValue));
			throw 1;
		}
	}
	try{
		message.getNextInt32(); //should throw
		TEST_PRINT(redTestText("did not throw when trying to read past end of buffer"));
		throw; //kill test
	}
	catch(...){}//proper behavior
}

void test_GetNextUint16(){
	uint16_t testBuffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	ByteArray testMessage = serializeToBytes(&testBuffer[0], sizeof(uint16_t)*8 );

	SocketMessage message(1, 2,
	ByteArray( std::string("IP") ),
	ByteArray( std::string("Port") ),
	ByteArray( std::string("CSRFkey") ),
	testMessage );

	for(int i=0; i<8; ++i){
		uint16_t nextValue =  message.getNextUint16();
		if(testBuffer[i] != nextValue){
			TEST_PRINT(redTestText("Input does not match output. Input: "<<testBuffer[i]<<" Output: "<<nextValue));
			throw 1;
		}
	}
	try{
		message.getNextUint16(); //should throw
		TEST_PRINT(redTestText("did not throw when trying to read past end of buffer"));
		throw; //kill test
	}
	catch(...){}//proper behavior
}

void test_GetNextInt16(){
	int16_t testBuffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	ByteArray testMessage = serializeToBytes(&testBuffer[0], sizeof(int16_t)*8 );

	SocketMessage message(1, 2,
	ByteArray( std::string("IP") ),
	ByteArray( std::string("Port") ),
	ByteArray( std::string("CSRFkey") ),
	testMessage );

	for(int i=0; i<8; ++i){
		int16_t nextValue =  message.getNextInt16();
		if(testBuffer[i] != nextValue){
			TEST_PRINT(redTestText("Input does not match output. Input: "<<testBuffer[i]<<" Output: "<<nextValue));
			throw 1;
		}
	}
	try{
		message.getNextInt16(); //should throw
		TEST_PRINT(redTestText("did not throw when trying to read past end of buffer"));
		throw; //kill test
	}
	catch(...){}//proper behavior
}

void test_GetNextUint64(){
	uint64_t testBuffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	ByteArray testMessage = serializeToBytes(&testBuffer[0], sizeof(uint64_t)*8 );

	SocketMessage message(1, 2,
	ByteArray( std::string("IP") ),
	ByteArray( std::string("Port") ),
	ByteArray( std::string("CSRFkey") ),
	testMessage );

	for(int i=0; i<8; ++i){
		uint64_t nextValue =  message.getNextUint64();
		if(testBuffer[i] != nextValue){
			TEST_PRINT(redTestText("Input does not match output. Input: "<<testBuffer[i]<<" Output: "<<nextValue));
			throw 1;
		}
	}
	try{
		message.getNextUint64(); //should throw
		TEST_PRINT(redTestText("did not throw when trying to read past end of buffer"));
		throw; //kill test
	}
	catch(...){}//proper behavior
}

void test_GetNextInt64(){
	int64_t testBuffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	ByteArray testMessage = serializeToBytes(&testBuffer[0], sizeof(int64_t)*8 );

	SocketMessage message(1, 2,
	ByteArray( std::string("IP") ),
	ByteArray( std::string("Port") ),
	ByteArray( std::string("CSRFkey") ),
	testMessage );

	for(int i=0; i<8; ++i){
		int64_t nextValue =  message.getNextInt64();
		if(testBuffer[i] != nextValue){
			TEST_PRINT(redTestText("Input does not match output. Input: "<<testBuffer[i]<<" Output: "<<nextValue));
			throw 1;
		}
	}
	try{
		message.getNextInt64(); //should throw
		TEST_PRINT(redTestText("did not throw when trying to read past end of buffer"));
		throw; //kill test
	}
	catch(...){}//proper behavior
}

void test_GetNextDouble(){
	double testBuffer[8] = {0.0, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7};
	ByteArray testMessage = serializeToBytes(&testBuffer[0], sizeof(double)*8 );

	SocketMessage message(1, 2,
	ByteArray( std::string("IP") ),
	ByteArray( std::string("Port") ),
	ByteArray( std::string("CSRFkey") ),
	testMessage );

	for(int i=0; i<8; ++i){
		double nextValue =  message.getNextDouble();
		if(testBuffer[i] != nextValue){
			TEST_PRINT(redTestText("Input does not match output. Input: "<<testBuffer[i]<<" Output: "<<nextValue));
			throw 1;
		}
	}
	try{
		message.getNextDouble(); //should throw
		TEST_PRINT(redTestText("did not throw when trying to read past end of buffer"));
		throw; //kill test
	}
	catch(...){}//proper behavior
}

void test_GetNextChar(){
	char testBuffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	ByteArray testMessage = serializeToBytes(&testBuffer[0], sizeof(char)*8 );

	SocketMessage message(1, 2,
	ByteArray( std::string("IP") ),
	ByteArray( std::string("Port") ),
	ByteArray( std::string("CSRFkey") ),
	testMessage );

	for(int i=0; i<8; ++i){
		char nextValue =  message.getNextChar();
		if(testBuffer[i] != nextValue){
			TEST_PRINT(redTestText("Input does not match output. Input: "<<testBuffer[i]<<" Output: "<<nextValue));
			throw 1;
		}
	}
	try{
		message.getNextChar(); //should throw
		TEST_PRINT(redTestText("did not throw when trying to read past end of buffer"));
		throw; //kill test
	}
	catch(...){}//proper behavior
}


void test(){
	test_GetNextFloat();
	test_GetNextUint32();
	test_GetNextInt32();
	test_GetNextUint16();
	test_GetNextInt16();
	test_GetNextUint64();
	test_GetNextInt64();
	test_GetNextDouble();
	test_GetNextChar();
}

}


int main(){
	SocketMessage_Test::test();
	return 0;
}*/
int main(){return 0;}
