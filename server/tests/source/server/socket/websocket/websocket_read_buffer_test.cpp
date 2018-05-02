#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/websocket/websocket_read_buffer.cpp, \
                                     source/server/socket/set_of_file_descriptors.cpp, \
                                     source/server/socket/file_descriptor.cpp, \
                                     source/logging/exception_handler.cpp, \
                                     source/data_types/byte_array.cpp"

#include "source/server/socket/websocket/websocket_read_buffer.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "tests/test_lib/mocks_stubs/mock_system_wrapper.h"
#include "tests/test_lib/mocks_stubs/socket_test_helpers.h"

#include "tests/test.h"



class WebsocketReadBuffersWrap : public WebsocketReadBuffers
{
public:
	WebsocketReadBuffersWrap(SetOfFileDescriptors *FDs, size_t size) : WebsocketReadBuffers(FDs, size){}

	std::string getFracBufferMessage(size_t i, size_t j)
	{
		return fractureBuffer[i].buffer[j].toString();
	}

	size_t getFracBufferSize(size_t i)
	{
		return fractureBuffer[1].size();
	}

	std::string getMessageBufferMessage(size_t i, size_t j)
	{
		return messageBuffer[i].buffer[j].toString();
	}
};


class ReadBufferTestFactory{
public:
	MockSystemWrapper &systemWrap;
	WebsocketReadBuffersWrap RB;
	ReadBufferTestFactory(SetOfFileDescriptors *FDs, size_t size):
		systemWrap(MockSystemWrapper::getMockSystemInstance(true)),
		RB(FDs, size)
	{}
};


TEST(WebsocketReadBufferTest, addMessage)
{
	SetOfFileDescriptors FDs;
	FDs.addFD(1);

	std::string testString = generateTestString(101);
	ByteArray input;
	input.appendWithNoSize(testString); //addMessage uses move if messageBuffer is empty. input is used instead of testString because its contents will be destroyed

	{
		ReadBufferTestFactory testBuffer(&FDs,100);
		EXPECT_ANY_THROW(
			testBuffer.RB.addMessage(1, input);
		);
	}

	ReadBufferTestFactory testBuffer(&FDs,202);
	input = ByteArray();
	input.appendWithNoSize(testString);
	testBuffer.RB.addMessage(1, input);
	std::string result = testBuffer.RB.getMessageBufferMessage(1,0);

	EXPECT_STREQ( result.c_str(), testString.c_str());

	input = ByteArray();
	input.appendWithNoSize(testString);
	testBuffer.RB.addMessage(1, input);
	result = testBuffer.RB.getMessageBufferMessage(1,1);

	EXPECT_STREQ( result.c_str(), testString.c_str());
}



TEST(WebsocketReadBufferTest, extractMessage)
{
	SetOfFileDescriptors FDs;
	FDs.addFD(1);

	std::string testString = generateTestString(300);
	ByteArray input;
	input.appendWithNoSize(testString);

	ReadBufferTestFactory testBuffer(&FDs,1000);

	testBuffer.RB.addMessage(1, input);
	ByteArray result;

	EXPECT_EQ( testBuffer.RB.extractMessage(result, 0, 1), true);

	input =ByteArray();
	input.appendWithNoSize(testString);
	testBuffer.RB.addMessage(1, input);
	testBuffer.RB.setMessageSize(1, 600);
	result =ByteArray();

	EXPECT_EQ( testBuffer.RB.extractMessage(result, 0, 1), false);

	input = ByteArray();
	input.appendWithNoSize(testString);
	testBuffer.RB.addMessage(1, input);
	result =ByteArray();

	EXPECT_EQ( testBuffer.RB.extractMessage(result, 0, 1), true);

	std::string stringResult = result.toString();
	testString.append(testString);

	EXPECT_EQ( testBuffer.RB.extractMessage(result, 0, 1), 0);
}



TEST(WebsocketReadBufferTest, addFracture)
{
	SetOfFileDescriptors FDs;
	FDs.addFD(1);

	std::string testString = generateTestString(300);
	ByteArray input; //addFracture uses move if messageBuffer is empty. input is used instead of testString because its contents will be destroyed
	input.appendWithNoSize(testString);

	//WebsocketReadBuffersWrap testBuffer(&FDs,1000);
	ReadBufferTestFactory testBuffer(&FDs,1000);
	input = ByteArray();
	input.appendWithNoSize(testString);
	testBuffer.RB.addFracture(1, input);
	std::string stringResult = testBuffer.RB.getFracBufferMessage(1,0);

	EXPECT_STREQ( stringResult.c_str(), testString.c_str() );

	input = ByteArray();
	input.appendWithNoSize(testString);

	testBuffer.RB.addFracture(1, input);
	stringResult = testBuffer.RB.getFracBufferMessage(1,1);

	EXPECT_STREQ( stringResult.c_str(), testString.c_str() );
	EXPECT_EQ( testBuffer.RB.getFracBufferSize(1), 600 );
}



TEST(WebsocketReadBufferTest, extractFracture)
{
	//MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs;
	FDs.addFD(1);

	std::string testString = generateTestString(300);
	ByteArray input;
	input.appendWithNoSize(testString); //addFracture uses move if messageBuffer is empty. input is used instead of testString because its contents will be destroyed

	ReadBufferTestFactory testBuffer(&FDs,1000);
	input = ByteArray();
	input.appendWithNoSize(testString);

	testBuffer.RB.addFracture(1, input);
	input = ByteArray();
	input.appendWithNoSize(testString);

	testBuffer.RB.addFracture(1, input);

	ByteArray result;
	testBuffer.RB.extractFracture(result, 0, 1);

	input = ByteArray(testString);
	testString.append(testString);
	std::string stringResult = result.toString();

	EXPECT_STREQ( stringResult.c_str(), testString.c_str() );
}



int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
