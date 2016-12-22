/*#define TESTING //to activate conditional macros for test logging
#include "tests/test.h"
#include <iostream>
#include <string>
#include "tests/server/mocks_stubs/mock_system_wrapper.h"
#include "tests/server/mocks_stubs/socket_test_helpers.h"
#include "tests/server/message_queue_test/message_queue_test.h"
#include "server/socket/message_queue.h"
#include "server/socket/socket_message.h"
#include "engine/byte_array.h"
#include "main/includes.h"

#include <cgreen/cgreen.h>
using namespace cgreen;

Describe(MessageQueueTest)
BeforeEach(MessageQueueTest) {}
AfterEach(MessageQueueTest) {}




namespace MessageQueue_Test{





Ensure(MessageQueueTest, getNextMessage_test) {
	bool running = true;
	ByteArray IP( std::string("testIP") );
	ByteArray Port( std::string("testPort") );
	ByteArray CSRFkey( std::string("testCSRFkey") );
	ByteArray Message( std::string("testMessage") );

	SocketMessage message(1, 2, IP, Port, CSRFkey, Message);
	MessageQueue testQueue(&running);
	testQueue.pushMessage(message);

	SocketMessage result = testQueue.getNextMessage();

	assert_that(result.getIP().toString().c_str(), is_equal_to_string("testIP") );
	assert_that(result.getPort().toString().c_str(), is_equal_to_string("testPort") );
	assert_that(result.getCSRFkey().toString().c_str(), is_equal_to_string("testCSRFkey") );
	assert_that(result.getMessage().toString().c_str(), is_equal_to_string("testMessage") );

	assert_that(result.getFD(), is_equal_to(1));
	assert_that(result.getType(), is_equal_to(2));
}


int test(){
	TestSuite *suite = create_named_test_suite("Message Queue");
	add_test_with_context(suite, MessageQueueTest, getNextMessage_test);
	return run_test_suite(suite, create_text_reporter());
}


}


int main(){
	return MessageQueue_Test::test();
}*/
int main(){return 0;}
