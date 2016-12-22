/*#define TESTING //to activate conditional macros for test logging
#include "tests/test.h"
//#include <iostream>
#include <string>
#include "main/includes.h"
#include "server/socket/websocket/websocket_message_sender.h"
#include "server/socket/socket_message.h"
#include "server/socket/set_of_file_descriptors.h"
#include "server/socket/websocket/websocket_message_sender.h"
#include "engine/byte_array.h"

#include "tests/server/mocks_stubs/mock_system_wrapper.h"
#include "tests/server/mocks_stubs/socket_test_helpers.h"
#include "tests/server/websocket_message_sender_test/websocket_message_sender_test.h"

namespace WebsocketMessageSender_Test{

std::string testCreateFrameHeader(const ByteArray &in, uint8_t opcode);

std::string testCreateFrameHeader(const ByteArray &in, uint8_t opcode){
	size_t size = 2;

	if(in.size()>65535){
		size = 10;
	}
	else if(in.size()>125){
		size = 4;
	}

	std::string buffer;
	buffer.resize(size);
	buffer[0] = static_cast <char>(128 + opcode);
	if(size == 2){
		buffer[1] = static_cast <char>(in.size());
	}
	else if(size == 4){
		uint64_t isize = in.size();
		uint64_t itsize;
		buffer[1] = static_cast <char>(126);
		buffer[2] = static_cast <char>(isize>>8);
		itsize    = (isize<<8);
		buffer[3] = static_cast <char>(itsize>>8);
	}
	else if(size == 10){
		uint64_t isize = in.size();
		uint64_t itsize;
		buffer[1] = static_cast <char>(127);
		buffer[2] = static_cast <char>(isize>>56);
		for(size_t i=3, j=8; i<10; ++i,j+=8){
			itsize = (isize<<j);
			buffer[i] = static_cast <char>(itsize>>56);
		}
	}
	return buffer;
}

void test_CreateFrameHeader(){
	MockSystemWrapper systemWrap;
	//SetOfFileDescriptors FDs(&systemWrap);
	//FDs.addFD(1);
	WebsocketMessageSender sender(&systemWrap);



	ByteArray testString( generateTestString(125) );
	ByteArray mediumTestString( generateTestString(65535) );
	ByteArray largeTestString( generateTestString(65536) );


	std::string testStringResult = sender.createFrameHeader(testString, 2).toString();
	std::string mediumTestStringResult = sender.createFrameHeader(mediumTestString, 2).toString();
	std::string largeTestStringResult = sender.createFrameHeader(largeTestString, 2).toString();

	if(testStringResult.compare(testCreateFrameHeader(testString,2)) !=0){
		TEST_PRINT(redTestText("small string output is wrong"));
		throw 1;
	}

	if(mediumTestStringResult.compare(testCreateFrameHeader(mediumTestString,2)) !=0){
		TEST_PRINT(redTestText("medium string output is wrong"));
		throw 1;
	}

	if(largeTestStringResult.compare(testCreateFrameHeader(largeTestString,2)) !=0){
		TEST_PRINT(redTestText("large string output is wrong"));
		throw 1;
	}
}
*/
/*void test_AddMessage(){
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	FDs.setIP(1,std::string("IP") );
	FDs.setPort(1,std::string("PORT") );
	FDs.setCSRFkey(1,std::string("KEY") );
	WebsocketMessageSender sender(&systemWrap, &FDs);

	//SocketMessage SocketMessage(int _FD, uint32_t _type, const secString &_IP, const secString &_port, const secString &_CSRFkey, secString &_message);
	std::string testString("testing");
	SocketMessage testMessage(1, 1, std::string("IP"), std::string("PORT"), std::string("KEY"), testString);

	std::string expectedOutput = sender.createFrameHeader(testMessage.getMessage(), 2);
	expectedOutput.append(testMessage.getMessage());

	sender.addMessage(testMessage);

	if(sender.writeBuffers.writeBuffer[1].message.compare(expectedOutput)!= 0){
		TEST_PRINT(redTestText("write buffer does not match input message"));
		TEST_PRINT(redTestText("Expected:\n"<<expectedOutput<<"\nHas size: "<<expectedOutput.size() ));
		TEST_PRINT(redTestText("In buffer:\n"<<sender.writeBuffers.writeBuffer[1].message<<"\nHas size: "<<sender.writeBuffers.writeBuffer[1].message.size() ));
		throw 1;
	}
}*/
/*
void test(){
	test_CreateFrameHeader();
	//test_AddMessage();

}

}

int main(){
	WebsocketMessageSender_Test::test();
	return 0;
}*/
int main(){return 0;}
