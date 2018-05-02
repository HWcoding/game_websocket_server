#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/websocket/websocket_write_buffer.cpp, \
                                     source/data_types/byte_array.cpp"

#include "source/server/socket/websocket/websocket_write_buffer.h"
#include "tests/test_lib/mocks_stubs/mock_system_wrapper.h"
#include "tests/test_lib/mocks_stubs/socket_test_helpers.h"

#include "tests/test.h"



class WebsocketWriteBuffersWrap : public WebsocketWriteBuffers
{
public:
	WebsocketWriteBuffersWrap() = default;

	ByteArray getMessageFromBuffer(int i)
	{
		return writeBuffer[i].message;
	}

	void setBufferBegin(int i, size_t value)
	{
		writeBuffer[i].begin = value;
	}
	size_t getBufferBegin(int i)
	{
		return writeBuffer[i].begin;
	}
	size_t getBufferCount(int i)
	{
		return writeBuffer.count(i);
	}
};


class WriteBufferTestFactory{
public:
	MockSystemWrapper &systemWrap;
	WebsocketWriteBuffersWrap WB;
	WriteBufferTestFactory(): systemWrap(MockSystemWrapper::getMockSystemInstance(true)), WB(){}
};


TEST(WebsocketWriteBufferTest, addMessageProperlyAddsTwoMessages)
{
	WriteBufferTestFactory testBuffer;

	std::string testString = generateTestString(300);
	std::string twoStringsAppended = testString;
	twoStringsAppended.append(testString);
	ByteArray input;
	input.appendWithNoSize(testString);

	testBuffer.WB.addMessage(1, input);
	testBuffer.WB.addMessage(1, input);

	testBuffer.WB.writeData(1);
	std::string result = testBuffer.systemWrap.GetWriteBuffer(1);

	EXPECT_STREQ(result.c_str(), twoStringsAppended.c_str());
}





TEST(WebsocketWriteBufferTest, addMessageWorksAfterWriteDataFailedToWriteWholeBuffer)
{
	WriteBufferTestFactory testBuffer;

	std::string testStringLarge = generateTestString(199000);
	std::string appendedTestStringValue = testStringLarge.substr(100001);
	ByteArray input;
	input.appendWithNoSize(testStringLarge);

	//write the first 100001 bytes to buffer then clear it
	testBuffer.WB.addMessage(1, input);

	testBuffer.systemWrap.SetBytesTillWriteFail(1,100001);
	testBuffer.WB.writeData(1); //should stop afer writing 100001 bytes
	testBuffer.systemWrap.ClearWriteBuffer(1);
	testBuffer.systemWrap.SetBytesTillWriteFail(1,-1);// set write to not fail

	//write another message to buffer
	ByteArray a;
	a.appendWithNoSize(std::string("A"));
	testBuffer.WB.addMessage(1, a );
	appendedTestStringValue.append("A");
	testBuffer.WB.writeData(1);

	//check buffer to see if the remaining part of the first message is still in buffer
	std::string result = testBuffer.systemWrap.GetWriteBuffer(1);

	EXPECT_STREQ(result.c_str(), appendedTestStringValue.c_str());
}



TEST(WebsocketWriteBufferTest, MessageSizeTest)
{
	WriteBufferTestFactory testBuffer;

	ByteArray input;
	input.appendWithNoSize( generateTestString(300) );
	testBuffer.WB.addMessage(1, input);
	size_t testSize = testBuffer.WB.messageSize(1);

	EXPECT_EQ( testSize, 300);

	testBuffer.WB.setBufferBegin(1, 100);
	testSize = testBuffer.WB.messageSize(1);
	EXPECT_EQ( testSize, 200);
}



TEST(WebsocketWriteBufferTest, WriteDataTest)
{
	WriteBufferTestFactory testBuffer;

	std::string testString = generateTestString(300);
	ByteArray input;
	input.appendWithNoSize(testString);

	testBuffer.WB.addMessage(1, input);
	testString = testString.substr(5);
	testBuffer.WB.setBufferBegin(1, 5);

	testBuffer.WB.writeData(1);

	std::string result = testBuffer.systemWrap.GetWriteBuffer(1);

	EXPECT_STREQ( result.c_str(), testString.c_str() );
	EXPECT_EQ( testBuffer.WB.messageSize(1), 0 );		//Error is buffer was not cleared after writing
	EXPECT_EQ( testBuffer.WB.getBufferBegin(1), 0 );	//Error is begin was not reset
}


TEST(WebsocketWriteBufferTest, EraseBuffersTest)
{
	WriteBufferTestFactory testBuffer;

	ByteArray input;
	input.appendWithNoSize( generateTestString(300) );
	testBuffer.WB.addMessage(1, input);

	testBuffer.WB.eraseBuffers(1);

	//the buffer for FD 1 should be gone
	EXPECT_EQ( testBuffer.WB.getBufferCount(1), 0);
}



int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
