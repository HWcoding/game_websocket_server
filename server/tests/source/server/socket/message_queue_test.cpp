// list of comma separated files this test needs to be linked with; read by the build script
#define TEST_FILE_LINK_DEPENDENCIES "source/data_types/byte_array.cpp, \
									source/data_types/socket_message.cpp, \
									source/server/socket/message_queue.cpp"

#include "source/data_types/socket_message.h"
#include "source/server/socket/message_queue.h"
#include "source/data_types/byte_array.h"

#include "tests/test.h"


// mock of debug function used by message_queue.cpp
namespace DEBUG_BACKTRACE {
	void debugBackTrace(){}
}


TEST(MessageQueueTest, getNextMessage)
{
	std::atomic<bool> running;
	running.store(true);
	ByteArray IP("testIP");
	ByteArray Port("testPort");
	ByteArray CSRFkey("testCSRFkey");
	ByteArray Message("testMessage");


	SocketMessage message(1, 2, 2, 0, IP, Port, CSRFkey, Message);
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
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
