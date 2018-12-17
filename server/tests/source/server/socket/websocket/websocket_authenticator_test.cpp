#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/websocket/websocket_authenticator.cpp, \
                                     source/server/socket/websocket/websocket_handshake.cpp, \
                                     source/data_types/byte_array.cpp, \
                                     source/server/socket/file_descriptor.cpp, \
                                     source/logging/exception_handler.cpp, \
                                     source/server/socket/set_of_file_descriptors.cpp"

#include "source/server/socket/websocket/websocket_authenticator.h"
#include "source/server/socket/websocket/websocket_client_validator_interface.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "source/data_types/byte_array.h"
#include "tests/test_lib/mocks_stubs/mock_system_wrapper.h"
#include "tests/test_lib/mocks_stubs/socket_test_helpers.h"
#include "tests/test.h"

ByteArray createTestHandshakeHeader(){
	ByteArray output;

	output.appendWithNoSize(std::string("GET /socket HTTP/1.1\r\n") );
	output.appendWithNoSize(std::string("Host: localhost\r\n") );
	output.appendWithNoSize(std::string("Connection: keep-alive, Upgrade\r\n") );
	output.appendWithNoSize(std::string("Pragma: no-cache\r\n") );
	output.appendWithNoSize(std::string("Cache-Control: no-cache\r\n") );
	output.appendWithNoSize(std::string("Upgrade: websocket\r\n") );
	output.appendWithNoSize(std::string("Origin: http://localhost:8080\r\n") );
	output.appendWithNoSize(std::string("Sec-WebSocket-Version: 13\r\n") );
	output.appendWithNoSize(std::string("User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.94 Safari/537.36\r\n") );
	output.appendWithNoSize(std::string("Accept-Encoding: gzip, deflate, sdch\r\n") );
	output.appendWithNoSize(std::string("Accept-Language: en-US,en;q=0.8\r\n") );
	output.appendWithNoSize(std::string("Cookie: GameServer=CJP5G89v2O30Dx-StfclobgZ0AuIH8Nh74SEzHxvBJEZWG6yJ3smhW73TZgDMO0HEy8AvYhKgzxVry5Yby75oT-250dW6PTdm74rhmQyACSwbiAbvp67108QZid7KoPJjf-OuP1cf5Z31_eHimsW8JTIf9KINfG0yy31WuDb21XU-nH9EJcVKhdoXrQB_35DPIRymBxV85cENsxScjjMIBnI60mUR1koC5k_XcwSiTgnoT9ApEPwIX6Z9iw0tV2X7\r\n") );
	output.appendWithNoSize(std::string("Sec-WebSocket-Key: +40NMxLMogWjfV/0HyjlxA==\r\n") );
	output.appendWithNoSize(std::string("Sec-WebSocket-Extensions: permessage-deflate; client_max_window_bits\r\n") );
	output.appendWithNoSize(std::string("Sec-WebSocket-Protocol: 05fcc56b7cb916d5e5a82081223b3357\r\n\r\n") );
	return output;
}

ByteArray createTestResponseHandshakeHeader(){
	ByteArray output;

	output.appendWithNoSize(std::string("HTTP/1.1 101 Switching Protocols\r\n") );
	output.appendWithNoSize(std::string("Upgrade: websocket\r\n") );
	output.appendWithNoSize(std::string("Connection: Upgrade\r\n") );
	output.appendWithNoSize(std::string("Sec-WebSocket-Accept: XvS4xrxcXUWz3C5CU/McPLRYBFY=\r\n") );
	output.appendWithNoSize(std::string("Sec-WebSocket-Protocol: 05fcc56b7cb916d5e5a82081223b3357\r\n\r\n") );

	return output;
}

class AuthenticatorTestFactory{
public:
	MockSystemWrapperState &systemWrap;
	WebsocketAuthenticator WA;


	class MyClientValidator : public ClientValidatorInterface
	{
	public:

		bool areClientHeadersValid(ConnectionHeaders &headers) override
		{
			(void)headers;
			//accept all connections
			return true;
		}
		bool isClientIPValid(std::string &IP, std::string &port) override
		{
			(void)IP;
			(void)port;
			//accept all connections
			return true;
		}
		~MyClientValidator() override = default;
	};

	MyClientValidator validator;
	AuthenticatorTestFactory(SetOfFileDescriptors*FDs ):
		systemWrap(MockSystemWrapperState::getMockSystemInstance(true)), WA(FDs)
	{
		WA.setClientValidator(&validator);
	}
};

TEST(WebsocketAuthenticatorTest, WebsocketAuthenticator)
{
	SetOfFileDescriptors FDs;
	ASSERT_THROW(WebsocketAuthenticator  failed(nullptr), std::runtime_error);
	WebsocketAuthenticator authenticator(&FDs);
}

TEST(WebsocketAuthenticatorTest, processHandshake)
{
	SetOfFileDescriptors FDs;
	FDs.addFD(1);
	AuthenticatorTestFactory authenticator(&FDs);

	// test with too much data
	ByteArray testString = createTestHandshakeHeader();
	testString.appendWithNoSize(generateTestString(2049));
	ASSERT_THROW(authenticator.WA.processHandshake(testString,1), std::runtime_error);

	// test with wrong request type
	ByteArray validHeader = createTestHandshakeHeader();
	testString = ByteArray();
	testString.appendWithNoSize("POST ");
	testString.resize(validHeader.size() + 1);
	memcpy(&testString[5],&validHeader[4], validHeader.size() - 4);
	ASSERT_THROW(authenticator.WA.processHandshake(testString,1), std::runtime_error);

	// test with garbage data
	testString = ByteArray();
	testString.appendWithNoSize("GET ");
	testString.appendWithNoSize(generateTestString(50));
	testString.appendWithNoSize("\r\n\r\n");
	ASSERT_THROW(authenticator.WA.processHandshake(testString,1), std::runtime_error);

	// test with data split in two
	ByteArray validHeaderPart1(5);
	memcpy(&validHeaderPart1[0], &validHeader[0], 5);
	ByteArray validHeaderPart2(validHeader.size()-5);
	memcpy(&validHeaderPart2[0], &validHeader[5], validHeader.size()-5);
	EXPECT_NO_THROW(authenticator.WA.processHandshake(validHeaderPart1,1));
	EXPECT_NO_THROW(authenticator.WA.processHandshake(validHeaderPart2,1));

	// test with correct data
	EXPECT_NO_THROW(authenticator.WA.processHandshake(validHeader,1));
}

TEST(WebsocketAuthenticatorTest, sendHandshake)
{
	SetOfFileDescriptors FDs;
	AuthenticatorTestFactory authenticator(&FDs);
	FDs.addFD(1);

	ByteArray validHeader = createTestHandshakeHeader();
	authenticator.WA.processHandshake(validHeader,1);

	// set the system to simulate a full buffer on socket 1 after 10 bytes are written
	authenticator.systemWrap.setBytesTillWriteFail(1, 10);
	// should return false with partial write
	EXPECT_EQ(false, authenticator.WA.sendHandshake(1));
	EXPECT_EQ(false, authenticator.WA.sendHandshake(1));

	// set system to allow all bytes to be written
	authenticator.systemWrap.setBytesTillWriteFail(1, -1);
	// should return true after writing all bytes
	EXPECT_EQ(true, authenticator.WA.sendHandshake(1));

	// get the written data from the system
	std::string data = authenticator.systemWrap.getWriteBuffer(1);

	// check the data to make sure it is correct
	std::string expected = createTestResponseHandshakeHeader().toString();
	EXPECT_STREQ( data.c_str(), expected.c_str());
}

TEST(WebsocketAuthenticatorTest, closeFD)
{
	SetOfFileDescriptors FDs;
	AuthenticatorTestFactory authenticator(&FDs);
	FDs.addFD(1);

	ByteArray validHeader = createTestHandshakeHeader();

	// split validHeader into two parts
	ByteArray validHeaderPart1(5);
	memcpy(&validHeaderPart1[0], &validHeader[0], 5);
	ByteArray validHeaderPart2(validHeader.size()-5);
	memcpy(&validHeaderPart2[0], &validHeader[5], validHeader.size()-5);

	// fill write buffer
	authenticator.WA.processHandshake(validHeader,1);
	// partial fill the read buffer
	authenticator.WA.processHandshake(validHeaderPart1,1);

	// should erase the write and read buffers of FD 1
	authenticator.WA.closeFD(1);

	// should throw because we erased the first half when the read buffer was erased
	ASSERT_THROW(authenticator.WA.processHandshake(validHeaderPart2,1);, std::runtime_error);

	// should throw when called without a write buffer
	ASSERT_THROW(authenticator.WA.sendHandshake(1), std::runtime_error);
}


class TestClientValidator : public ClientValidatorInterface
{
public:
	bool areClientHeadersValid(ConnectionHeaders &headers) override
	{
		(void)headers;
		//accept all traffic
		return true;
	}
	bool isClientIPValid(std::string &IP, std::string &port) override
	{
		if(IP.compare("IPFail") == 0) {
			return false;
		}
		if(port.compare("PortFail") == 0) {
			return false;
		}
		//accept all traffic
		return true;
	}
	~TestClientValidator() override = default;
};

TEST(WebsocketAuthenticatorTest, isNotValidConnection)
{
	TestClientValidator validator;

	SetOfFileDescriptors FDs;
	AuthenticatorTestFactory authenticator(&FDs);
	FDs.addFD(1);

	authenticator.WA.setClientValidator(&validator);

	EXPECT_EQ(true,authenticator.WA.isNotValidConnection(ByteArray("IPFail"), ByteArray("port")));
	EXPECT_EQ(true,authenticator.WA.isNotValidConnection(ByteArray("IP"), ByteArray("PortFail")));
	EXPECT_EQ(false,authenticator.WA.isNotValidConnection(ByteArray("IP"), ByteArray("port")));
}

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
