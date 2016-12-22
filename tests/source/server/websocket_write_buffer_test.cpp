/*#define TESTING //to activate conditional macros for test logging and access
#include "tests/test.h"
#include "main/includes.h"
#include <iostream>
#include <string>
#include "tests/server/mocks_stubs/mock_system_wrapper.h"
#include "tests/server/mocks_stubs/socket_test_helpers.h"
#include "tests/server/websocket_write_buffer_test/websocket_write_buffer_test.h"
#include "server/socket/set_of_file_descriptors.h"
#include "server/socket/websocket/websocket_write_buffer.h"
#include "engine/byte_array.h"

#include <cgreen/cgreen.h>
using namespace cgreen;

Describe(WriteBufferTest)
BeforeEach(WriteBufferTest) {}
AfterEach(WriteBufferTest) {}

namespace WebsocketWriteBuffer_Test{


Ensure(WriteBufferTest, addMessage_properly_adds_two_messages) {
	MockSystemWrapper systemWrap;
	WebsocketWriteBuffers testBuffer(&systemWrap);

	std::string testString = generateTestString(300);
	std::string twoStringsAppended = testString;
	twoStringsAppended.append(testString);
	ByteArray input(testString);

	testBuffer.addMessage(1, input);
	testBuffer.addMessage(1, input);

	testBuffer.writeData(1);
	std::string result = systemWrap.GetWriteBuffer(1);

	assert_that( result.c_str(), is_equal_to_string( twoStringsAppended.c_str() ) );
}

Ensure(WriteBufferTest, addMessage_properly_adds_messages_after_writeData_failed_to_write_whole_buffer) {
	MockSystemWrapper systemWrap;
	WebsocketWriteBuffers testBuffer(&systemWrap);
	std::string testStringLarge = generateTestString(199000);
	std::string appendedTestStringValue = testStringLarge.substr(100001);
	ByteArray input(testStringLarge);

	//write the first 100001 bytes to buffer then clear it
	testBuffer.addMessage(1, input);

	systemWrap.SetBytesTillWriteFail(1,100001);
	testBuffer.writeData(1); //should stop afer writing 100001 bytes
	systemWrap.ClearWriteBuffer(1);
	systemWrap.SetBytesTillWriteFail(1,-1);// set write to not fail

	//write another message to buffer
	testBuffer.addMessage(1, ByteArray(std::string("A")) );
	appendedTestStringValue.append("A");
	testBuffer.writeData(1);

	//check buffer to see if the remaining part of the first message is still in buffer
	std::string result = systemWrap.GetWriteBuffer(1);
	assert_that( result.c_str(),is_equal_to_string(appendedTestStringValue.c_str()) );
}


Ensure(WriteBufferTest, WriteData_test) {
	MockSystemWrapper systemWrap;
	WebsocketWriteBuffers testBuffer(&systemWrap);

	std::string testString = generateTestString(300);
	ByteArray input(testString);

	testBuffer.addMessage(1, input);
	testString = testString.substr(5);
	testBuffer.writeBuffer[1].begin = 5;

	testBuffer.writeData(1);

	std::string result = systemWrap.GetWriteBuffer(1);

	assert_that( result.c_str(),is_equal_to_string(testString.c_str()) );
	assert_that( testBuffer.messageSize(1),is_equal_to(0) );		//Error is buffer was not cleared after writing
	assert_that( testBuffer.writeBuffer[1].begin,is_equal_to(0) );	//Error is begin was not reset
}


Ensure(WriteBufferTest, MessageSize_test) {
	MockSystemWrapper systemWrap;
	WebsocketWriteBuffers testBuffer(&systemWrap);
	ByteArray input( generateTestString(300) );
	testBuffer.addMessage(1, input);
	size_t testSize = testBuffer.messageSize(1);

	assert_that( testSize ,is_equal_to(300) );

	testBuffer.writeBuffer[1].begin =100;
	testSize = testBuffer.messageSize(1);

	assert_that( testSize ,is_equal_to(200) );
}


Ensure(WriteBufferTest, EraseBuffers_test) {
	MockSystemWrapper systemWrap;
	WebsocketWriteBuffers testBuffer(&systemWrap);

	ByteArray input( generateTestString(300) );
	testBuffer.addMessage(1, input);

	testBuffer.eraseBuffers(1);

	assert_that( testBuffer.writeBuffer.count(1), is_equal_to(0) );
}


int test(){
	TestSuite *suite = create_named_test_suite("Websocket Write Buffer");
	add_test_with_context(suite, WriteBufferTest, addMessage_properly_adds_two_messages);
	add_test_with_context(suite, WriteBufferTest, addMessage_properly_adds_messages_after_writeData_failed_to_write_whole_buffer);
	add_test_with_context(suite, WriteBufferTest, WriteData_test);
	add_test_with_context(suite, WriteBufferTest, MessageSize_test);
	add_test_with_context(suite, WriteBufferTest, EraseBuffers_test);
	return run_test_suite(suite, create_text_reporter());
}


}


int main(){
	return WebsocketWriteBuffer_Test::test();
}
*/
int main(){return 0;}
