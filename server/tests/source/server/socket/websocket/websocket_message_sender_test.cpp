#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/websocket/websocket_write_buffer.cpp, \
                                     source/server/socket/message_sender_interface.h, \
                                     source/server/socket/websocket/websocket_message_sender.cpp, \
                                     source/data_types/byte_array.cpp, \
                                     source/data_types/socket_message.cpp"

#include "source/server/socket/websocket/websocket_write_buffer.h"
#include "source/server/socket/websocket/websocket_message_sender.h"
#include "tests/test_lib/mocks_stubs/mock_system_wrapper.h"
#include "tests/test_lib/mocks_stubs/socket_test_helpers.h"
#include "source/data_types/socket_message.h"

#include "tests/test.h"


std::string testCreateFrameHeader(size_t messageSize, uint8_t opcode);


class WebsocketMessageSenderWrap : public WebsocketMessageSender
{
public:
	WebsocketMessageSenderWrap() : WebsocketMessageSender(){}

	ByteArray createFrameHeader(size_t messageSize, uint8_t opcode)
	{
		return WebsocketMessageSender::createFrameHeader(messageSize, opcode);
	}

};

class MessageSenderTestFactory{
public:
	MockSystemWrapperState &systemWrap;
	WebsocketMessageSenderWrap MS;
	MessageSenderTestFactory():
		systemWrap(MockSystemWrapperState::getMockSystemInstance(true)), MS()
	{}
};

// returns correct FrameHeader for the given ByteArray and opcode

std::string testCreateFrameHeader(size_t messageSize, uint8_t opcode){
	size_t size;

	if(messageSize>65535){
		size = 10;
	}
	else if(messageSize>125){
		size = 4;
	}
	else {
		size = 2;
	}

	std::string buffer;
	buffer.resize(size);
	buffer[0] = static_cast <char>(128 + opcode);
	if(size == 2){
		// first byte is the size
		buffer[1] = static_cast <char>(messageSize);
	}
	else if(size == 4){
		// first byte is magic number 126
		buffer[1] = static_cast <char>(126);
		// the following bytes are the message size in network byte order
		buffer[2] = static_cast <char>((messageSize&0x000000000000FF00)>>8);
		buffer[3] = static_cast <char>((messageSize&0x00000000000000FF));
	}
	else if(size == 10){
		// first byte is magic number 127
		buffer[1] = static_cast <char>(127);
		// the following bytes are the message size in network byte order
		buffer[2] = static_cast <char>((messageSize & 0xFF00000000000000)>>(8*7));
		buffer[3] = static_cast <char>((messageSize & 0x00FF000000000000)>>(8*6));
		buffer[4] = static_cast <char>((messageSize & 0x0000FF0000000000)>>(8*5));
		buffer[5] = static_cast <char>((messageSize & 0x000000FF00000000)>>(8*4));
		buffer[6] = static_cast <char>((messageSize & 0x00000000FF000000)>>(8*3));
		buffer[7] = static_cast <char>((messageSize & 0x0000000000FF0000)>>(8*2));
		buffer[8] = static_cast <char>((messageSize & 0x000000000000FF00)>>(8*1));
		buffer[9] = static_cast <char>((messageSize & 0x00000000000000FF)>>(8*0));
	}
	return buffer;
}


TEST(WebsocketMessageSenderTest, createFrameHeader)
{
	//MockSystemWrapper systemWrap;
	//WebsocketMessageSenderWrap sender(&systemWrap);

	MessageSenderTestFactory sender;

	std::string smallTestStringResult = sender.MS.createFrameHeader(0, 2).toString();
	EXPECT_STREQ( smallTestStringResult.c_str(), testCreateFrameHeader(0,2).c_str() );

	smallTestStringResult = sender.MS.createFrameHeader(125, 2).toString();
	EXPECT_STREQ( smallTestStringResult.c_str(), testCreateFrameHeader(125,2).c_str() );

	std::string mediumTestStringResult = sender.MS.createFrameHeader(126, 2).toString();
	EXPECT_STREQ( mediumTestStringResult.c_str(), testCreateFrameHeader(126,2).c_str() );

	mediumTestStringResult = sender.MS.createFrameHeader(65535, 2).toString();
	EXPECT_STREQ( mediumTestStringResult.c_str(), testCreateFrameHeader(65535,2).c_str() );

	std::string largeTestStringResult = sender.MS.createFrameHeader(65536, 2).toString();
	EXPECT_STREQ( largeTestStringResult.c_str(), testCreateFrameHeader(65536,2).c_str() );

	largeTestStringResult = sender.MS.createFrameHeader(18446744073709551615U, 2).toString();
	EXPECT_STREQ( largeTestStringResult.c_str(), testCreateFrameHeader(18446744073709551615U,2).c_str() );
}

TEST(WebsocketMessageSenderTest, writeData)
{
	//MockSystemWrapper systemWrap;
	//WebsocketMessageSenderWrap sender(&systemWrap);

	MessageSenderTestFactory sender;

	std::string testString("testing");
	ByteArray IP;
	IP.appendWithNoSize("IP");
	ByteArray Port;
	Port.appendWithNoSize("Port");
	ByteArray KEY;
	KEY.appendWithNoSize("KEY");
	ByteArray mess;
	mess.appendWithNoSize("testing");

	// create a message to add
	SocketMessage testMessage(1, 1, 0, IP, Port, KEY, mess);

	// create the expected buffer contents of the message.
	// it should be a Frame Header followed by the message
	ByteArray expected(testCreateFrameHeader(testMessage.getMessage().size(),2));
	expected.append(testMessage.getMessage());

	// add the message
	sender.MS.addMessage(testMessage);

	// write the data
	sender.MS.writeData(1);

	// check the data to make sure it is correct
	std::string data = sender.systemWrap.getWriteBuffer(1);
	EXPECT_STREQ( data.c_str(), expected.toString().c_str());
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
