#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/websocket/websocket_read_buffer.cpp, \
                                     source/server/socket/set_of_file_descriptors.cpp, \
                                     source/server/socket/file_descriptor.cpp, \
                                     source/logging/exception_handler.cpp, \
                                     source/data_types/byte_array.cpp, \
                                     source/server/socket/system_wrapper.cpp"

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


TEST(WebsocketReadBufferTest, addMessage)
{
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);

	std::string testString = generateTestString(101);
	ByteArray input;
	input.appendWithNoSize(testString); //addMessage uses move if messageBuffer is empty. input is used instead of testString because its contents will be destroyed

	{
		WebsocketReadBuffers testBuffer(&FDs,100);
		EXPECT_ANY_THROW(
			testBuffer.addMessage(1, input);
			//TEST_PRINT(redTestText("addMessage did not throw with too large of a size"));
		);
		/*try{
			testBuffer.addMessage(1, input);
			TEST_PRINT(redTestText("addMessage did not throw with too large of a size"));
			throw; //kill program
		}
		catch(...){}//proper behavior*/
	}

	WebsocketReadBuffersWrap testBuffer(&FDs,202);
	input = ByteArray();
	input.appendWithNoSize(testString);
	testBuffer.addMessage(1, input);
	//std::string result = testBuffer.messageBuffer[1].buffer[0].toString();
	std::string result = testBuffer.getMessageBufferMessage(1,0);

	EXPECT_STREQ( result.c_str(), testString.c_str());
	/*if(result.compare(testString) != 0){
		TEST_PRINT(redTestText("Buffer does not match test string"));
		throw 1;
	}*/

	input = ByteArray();
	input.appendWithNoSize(testString);
	testBuffer.addMessage(1, input);
	//result = testBuffer.messageBuffer[1].buffer[1].toString();
	result = testBuffer.getMessageBufferMessage(1,1);

	EXPECT_STREQ( result.c_str(), testString.c_str());
	/*if(result.compare(testString) != 0){
		TEST_PRINT(redTestText("Buffer does not match test string"));
		throw 1;
	}*/
}



TEST(WebsocketReadBufferTest, extractMessage)
{
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);

	std::string testString = generateTestString(300);
	ByteArray input;
	input.appendWithNoSize(testString);

	WebsocketReadBuffers testBuffer(&FDs,1000);
	testBuffer.addMessage(1, input);
	ByteArray result;

	EXPECT_EQ( testBuffer.extractMessage(result, 0, 1), true);
	/*if( !testBuffer.extractMessage(result, 0, 1) ){
		TEST_PRINT( redTestText("extractMessage did not return a complete message when size was unknown") );
		TEST_PRINT( redTestText("message size = "<<testBuffer.messageBuffer[1].totalSize<<"\nExpected size = "<<testBuffer.messageBuffer[1].expectedSize) );
		throw 1;
	}*/

	input =ByteArray();
	input.appendWithNoSize(testString);
	testBuffer.addMessage(1, input);
	testBuffer.setMessageSize(1, 600);
	result =ByteArray();

	EXPECT_EQ( testBuffer.extractMessage(result, 0, 1), false);
	/*if( testBuffer.extractMessage(result, 0, 1) ){
		TEST_PRINT( redTestText("extractMessage returned a complete message when a partial message was given") );
		TEST_PRINT( redTestText("message size = "<<testBuffer.messageBuffer[1].totalSize<<"\nExpected size = "<<testBuffer.messageBuffer[1].expectedSize) );
		throw 1;
	}*/

	input = ByteArray();
	input.appendWithNoSize(testString);
	testBuffer.addMessage(1, input);
	result =ByteArray();

	EXPECT_EQ( testBuffer.extractMessage(result, 0, 1), true);
	/*if( !testBuffer.extractMessage(result, 0, 1) ){
		TEST_PRINT( redTestText("extractMessage did not return a complete message when a complete message was given") );
		TEST_PRINT( redTestText("message size = "<<testBuffer.messageBuffer[1].totalSize<<"\nExpected size = "<<testBuffer.messageBuffer[1].expectedSize) );
		throw 1;
	}*/

	std::string stringResult = result.toString();
	testString.append(testString);

	EXPECT_EQ( testBuffer.extractMessage(result, 0, 1), 0);
	/*if( stringResult.compare(testString) != 0 ){
		TEST_PRINT( redTestText("result does not equal input") );
		throw 1;
	}*/
}



TEST(WebsocketReadBufferTest, addFracture)
{
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);

	std::string testString = generateTestString(300);
	ByteArray input; //addFracture uses move if messageBuffer is empty. input is used instead of testString because its contents will be destroyed
	input.appendWithNoSize(testString);

	WebsocketReadBuffersWrap testBuffer(&FDs,1000);
	input = ByteArray();
	input.appendWithNoSize(testString);
	testBuffer.addFracture(1, input);
	//std::string stringResult = testBuffer.fractureBuffer[1].buffer[0].toString();
	std::string stringResult = testBuffer.getFracBufferMessage(1,0);

	EXPECT_STREQ( stringResult.c_str(), testString.c_str() );

	input = ByteArray();
	input.appendWithNoSize(testString);

	testBuffer.addFracture(1, input);
	//stringResult = testBuffer.fractureBuffer[1].buffer[1].toString();
	stringResult = testBuffer.getFracBufferMessage(1,1);

	EXPECT_STREQ( stringResult.c_str(), testString.c_str() );
	EXPECT_EQ( testBuffer.getFracBufferSize(1), 600 );
}



TEST(WebsocketReadBufferTest, extractFracture)
{
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);

	std::string testString = generateTestString(300);
	ByteArray input;
	input.appendWithNoSize(testString); //addFracture uses move if messageBuffer is empty. input is used instead of testString because its contents will be destroyed

	WebsocketReadBuffers testBuffer(&FDs,1000);
	input = ByteArray();
	input.appendWithNoSize(testString);

	testBuffer.addFracture(1, input);
	input = ByteArray();
	input.appendWithNoSize(testString);

	testBuffer.addFracture(1, input);

	ByteArray result;
	testBuffer.extractFracture(result, 0, 1);

	input = ByteArray(testString);
	testString.append(testString);
	std::string stringResult = result.toString();

	EXPECT_STREQ( stringResult.c_str(), testString.c_str() );
}



int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	STAY_SILENT_ON_SUCCESS;
	return RUN_ALL_TESTS();
}
