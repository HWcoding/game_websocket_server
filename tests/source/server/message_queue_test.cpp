#define TEST_FILE_LINK_DEPENDENCIES "source/data_types/byte_array.cpp, \
									source/data_types/socket_message.cpp \
									source/server/socket/message_queue.cpp \
									source/logging/exception_handler.cpp"

#include "source/data_types/socket_message.h"
#include "source/server/socket/message_queue.h"
#include "source/data_types/byte_array.h"

#include "tests/test.h"


TEST(MessageQueueTest, getNextMessage)
{
	std::atomic<bool> running;
	running.store(true);
	ByteArray IP( std::string("testIP") );
	ByteArray Port( std::string("testPort") );
	ByteArray CSRFkey( std::string("testCSRFkey") );
	ByteArray Message( std::string("testMessage") );


	SocketMessage message(1, 2, 0, IP, Port, CSRFkey, Message);
	MessageQueue testQueue(&running);
	testQueue.pushMessage(message);

	SocketMessage result = testQueue.getNextMessage();

	EXPECT_EQ(result.getIP().toString(), "testIP");
	EXPECT_EQ(result.getPort().toString(), "testPort");
	EXPECT_EQ(result.getCSRFkey().toString(), "testCSRFkey");
	EXPECT_EQ(result.getMessage().toString(), "testMessage");

	EXPECT_EQ(result.getFD(), 1);
	EXPECT_EQ(result.getType(), 2);
	EXPECT_EQ(result.getPriority(), 0);
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	STAY_SILENT_ON_SUCCESS();
	return RUN_ALL_TESTS();
}
