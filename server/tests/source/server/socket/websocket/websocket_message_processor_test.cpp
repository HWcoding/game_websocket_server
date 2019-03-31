#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/websocket/websocket_message_processor.cpp, \
                                     source/data_types/byte_array.cpp, \
                                     source/server/socket/websocket/websocket_read_buffer.cpp, \
                                     source/data_types/socket_message.cpp, \
                                     source/server/socket/set_of_file_descriptors.cpp, \
                                     source/server/socket/message_queue.cpp, \
                                     source/server/socket/file_descriptor.cpp, \
                                     source/logging/exception_handler.cpp"

#include "source/server/socket/websocket/websocket_message_processor.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/message_queue.h"
#include "source/data_types/socket_message.h"
#include "source/data_types/message_type.h"
#include "tests/test_lib/mocks_stubs/mock_system_wrapper.h"
#include "tests/test_lib/mocks_stubs/socket_test_helpers.h"
#include "tests/test_lib/clock_cycle_counter.h"
#include <atomic>

#include "tests/test.h"


TEST(WebsocketMessageProcessorTest, testCloseFDHandler)
{
	// reset system state
	MockSystemWrapperState::resetState();
	//create processor for testing and set its state
	SetOfFileDescriptors FDs;

	ByteArray IP;
	IP.appendWithNoSize("IP");
	ByteArray PORT;
	PORT.appendWithNoSize("PORT");
	ByteArray KEY;
	KEY.appendWithNoSize("KEY");

	FDs.addFD(1);
	FDs.setIP(1,IP);
	FDs.setPort(1,PORT);
	FDs.setCSRFkey(1,KEY );

	WebsocketMessageProcessor processor(&FDs);
	std::atomic<bool> running;
	running.store(true);
	std::unique_ptr<MessageQueue> testReaderQueue(new MessageQueue(&running));
	processor.setReaderQueue(testReaderQueue.get());

	// should generage a logout message and place it in the message queue
	processor.closeFDHandler(1);

	// check that message queue contains a message
	EXPECT_FALSE( testReaderQueue->isEmpty() );

	SocketMessage temp = testReaderQueue->getNextMessage();
	// check that the message is logout
	EXPECT_EQ(temp.getType(), MessageType::logout);

	// check that the logout message has the right credentials
	EXPECT_STREQ( temp.getIP().toString().c_str(), "IP" );
	EXPECT_STREQ( temp.getPort().toString().c_str(), "PORT" );
	EXPECT_STREQ( temp.getCSRFkey().toString().c_str(), "KEY" );

}



TEST(WebsocketMessageProcessorTest, testProcessSockMessageWorks)
{
	// reset system state
	MockSystemWrapperState::resetState();

	uint32_t mask = 3893384930;
	uint32_t messageCount = 10;
	size_t messageSize = 200;

	//create a number of strings and encode them with a mask the same way a browser would before sending them to this server
	std::vector<std::string> messages;
	std::string maskedTestString;
	for(uint32_t i = 0; i <messageCount; ++i){
		messages.push_back( generateTestString(messageSize,i) );
		maskedTestString.append(maskMessageForTesting(messages[i], mask+static_cast<uint32_t>(i), true, 130));
	}

	ByteArray input;
	input.appendWithNoSize(maskedTestString);

	//create processor for testing and set its state
	SetOfFileDescriptors FDs;

	ByteArray IP;
	IP.appendWithNoSize("IP");
	ByteArray PORT;
	PORT.appendWithNoSize("PORT");
	ByteArray KEY;
	KEY.appendWithNoSize("KEY");

	FDs.addFD(1);
	FDs.setIP(1,IP);
	FDs.setPort(1,PORT);
	FDs.setCSRFkey(1,KEY );

	WebsocketMessageProcessor processor(&FDs);

	std::atomic<bool> running;
	running.store(true);
	std::unique_ptr<MessageQueue> testReaderQueue(new MessageQueue(&running));

	processor.setReaderQueue(testReaderQueue.get());

	//process messages.  The function stores the output in a queue (testReaderQueue)
	processor.processSockMessage(input, 1);

	//move processed messages from queue into decoded
	std::vector< ByteArray > decoded;
	ByteArray temp = testReaderQueue->getNextMessage().getMessage();
	while(!temp.empty()){
		decoded.push_back(temp);
		temp = testReaderQueue->getNextMessage().getMessage();
	}

	//test to see if the number of messages output are the same as the number of messages input
	EXPECT_EQ( decoded.size(), messageCount );

	//test to see if the output messages match the input messages
	for(size_t i = 0; i< decoded.size(); ++i){
		EXPECT_STREQ( messages[i].c_str(), decoded[i].toString().c_str() );
	}

	// check that client is still connected
	EXPECT_TRUE(FDs.isFDOpen(1));

	ByteArray closeMessage;
	closeMessage.appendWithNoSize( createCloseControlMessage() );
	processor.processSockMessage(closeMessage, 1);
	// check that close message disconnected client
	EXPECT_FALSE(FDs.isFDOpen(1));
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
