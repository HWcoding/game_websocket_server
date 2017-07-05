#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/websocket/websocket_write_buffer.cpp, \
                                     source/server/socket/websocket/websocket_message_sender.cpp, \
                                     source/data_types/byte_array.cpp, \
                                     source/data_types/socket_message.cpp, \
                                     source/server/socket/system_wrapper.cpp"

#include "source/server/socket/websocket/websocket_write_buffer.h"
#include "source/server/socket/websocket/websocket_message_sender.h"
#include "tests/test_lib/mocks_stubs/mock_system_wrapper.h"
#include "tests/test_lib/mocks_stubs/socket_test_helpers.h"
#include "source/data_types/socket_message.h"

#include "tests/test.h"





std::string testCreateFrameHeader(const ByteArray &in, uint8_t opcode);


class WebsocketWriteBuffersWrap : public WebsocketWriteBuffers
{
public:
	WebsocketWriteBuffersWrap(MockSystemWrapper* sys): WebsocketWriteBuffers(sys){}

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

class WebsocketMessageSenderWrap : public WebsocketMessageSender
{
public:
	WebsocketMessageSenderWrap(WebsocketWriteBuffers *_writeBuffers ) : WebsocketMessageSender(_writeBuffers){}
	WebsocketMessageSenderWrap(SystemInterface *_systemWrap) : WebsocketMessageSender(_systemWrap){}

	ByteArray getMessageFromBuffer(int i)
	{
		WebsocketWriteBuffersWrap *wrap = dynamic_cast<WebsocketWriteBuffersWrap*>(writeBuffers.get());
		return wrap->getMessageFromBuffer(i);
	}
	ByteArray createFrameHeader(const ByteArray &in, uint8_t opcode)
	{
		return WebsocketMessageSender::createFrameHeader(in, opcode);
	}

};


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




TEST(WebsocketMessageSenderTest, createFrameHeader)
{
	MockSystemWrapper systemWrap;
	//SetOfFileDescriptors FDs(&systemWrap);
	//FDs.addFD(1);
	WebsocketMessageSenderWrap sender(&systemWrap);



	ByteArray testString;
	testString.appendWithNoSize( generateTestString(125) );
	ByteArray mediumTestString;
	mediumTestString.appendWithNoSize( generateTestString(65535) );
	ByteArray largeTestString;
	largeTestString.appendWithNoSize( generateTestString(65536) );


	std::string testStringResult = sender.createFrameHeader(testString, 2).toString();
	std::string mediumTestStringResult = sender.createFrameHeader(mediumTestString, 2).toString();
	std::string largeTestStringResult = sender.createFrameHeader(largeTestString, 2).toString();


	EXPECT_STREQ( testStringResult.c_str(), testCreateFrameHeader(testString,2).c_str() );

	EXPECT_STREQ( mediumTestStringResult.c_str(), testCreateFrameHeader(mediumTestString,2).c_str() );

	EXPECT_STREQ( largeTestStringResult.c_str(), testCreateFrameHeader(largeTestString,2).c_str() );


	/*if(testStringResult.compare(testCreateFrameHeader(testString,2)) !=0){
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
	}*/
}



TEST(WebsocketMessageSenderTest, addMessage)
{
	MockSystemWrapper systemWrap;
	WebsocketWriteBuffersWrap * writeBuffer = new WebsocketWriteBuffersWrap(&systemWrap);
	WebsocketMessageSenderWrap sender(writeBuffer);

	std::string testString("testing");
	ByteArray IP;
	IP.appendWithNoSize("IP");
	ByteArray Port;
	Port.appendWithNoSize("Port");
	ByteArray KEY;
	KEY.appendWithNoSize("KEY");
	ByteArray mess;
	mess.appendWithNoSize("testing");

	SocketMessage testMessage(1, 1, 0, IP, Port, KEY, mess);

	ByteArray expectedOutput = sender.createFrameHeader(testMessage.getMessage(), 2);
	expectedOutput.append(testMessage.getMessage());

	sender.addMessage(testMessage);

	std::string result = sender.getMessageFromBuffer(1).toString();
	EXPECT_STREQ( result.c_str(), expectedOutput.toString().c_str() );
	/*if(sender.writeBuffers.writeBuffer[1].message.compare(expectedOutput)!= 0){
		TEST_PRINT(redTestText("write buffer does not match input message"));
		TEST_PRINT(redTestText("Expected:\n"<<expectedOutput<<"\nHas size: "<<expectedOutput.size() ));
		TEST_PRINT(redTestText("In buffer:\n"<<sender.writeBuffers.writeBuffer[1].message<<"\nHas size: "<<sender.writeBuffers.writeBuffer[1].message.size() ));
		throw 1;
	}*/
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
