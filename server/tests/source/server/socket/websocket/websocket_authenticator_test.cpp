#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/websocket/websocket_authenticator.cpp, \
                                     source/server/socket/websocket/websocket_handshake.cpp, \
                                     source/data_types/byte_array.cpp, \
                                     source/server/socket/system_wrapper.cpp, \
                                     source/server/socket/file_descriptor.cpp, \
                                     source/logging/exception_handler.cpp, \
                                     source/server/socket/set_of_file_descriptors.cpp"

#include "source/server/socket/websocket/websocket_authenticator.h"
#include "source/server/socket/websocket/websocket_client_validator_interface.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "tests/test_lib/mocks_stubs/mock_system_wrapper.h"
#include "tests/test_lib/mocks_stubs/socket_test_helpers.h"
#include "tests/test.h"

TEST(WebsocketAuthenticatorTest, WebsocketAuthenticator)
{
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	ASSERT_THROW(WebsocketAuthenticator  failed(nullptr, &FDs), std::runtime_error);
	ASSERT_THROW(WebsocketAuthenticator  failed(&systemWrap, nullptr), std::runtime_error);

	WebsocketAuthenticator authenticator(&systemWrap, &FDs);
}

TEST(WebsocketAuthenticatorTest, processHandshake)
{
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);

	// test with too much data
	ByteArray testString = createTestHandshakeHeader();
	testString.appendWithNoSize(generateTestString(2049));
	ASSERT_THROW(authenticator.processHandshake(testString,1), std::runtime_error);

	// test with wrong request type
	ByteArray validHeader = createTestHandshakeHeader();
	testString = ByteArray();
	testString.appendWithNoSize("POST ");
	testString.resize(validHeader.size() + 1);
	memcpy(&testString[5],&validHeader[4], validHeader.size() - 4);
	ASSERT_THROW(authenticator.processHandshake(testString,1), std::runtime_error);

	// test with garbage data
	testString = ByteArray();
	testString.appendWithNoSize("GET ");
	testString.appendWithNoSize(generateTestString(50));
	testString.appendWithNoSize("\r\n\r\n");
	ASSERT_THROW(authenticator.processHandshake(testString,1), std::runtime_error);

	// test with data split in two
	ByteArray validHeaderPart1(5);
	memcpy(&validHeaderPart1[0], &validHeader[0], 5);
	ByteArray validHeaderPart2(validHeader.size()-5);
	memcpy(&validHeaderPart2[0], &validHeader[5], validHeader.size()-5);
	EXPECT_NO_THROW(authenticator.processHandshake(validHeaderPart1,1));
	EXPECT_NO_THROW(authenticator.processHandshake(validHeaderPart2,1));

	// test with correct data
	EXPECT_NO_THROW(authenticator.processHandshake(validHeader,1));
}

TEST(WebsocketAuthenticatorTest, sendHandshake)
{
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);
	FDs.addFD(1);

	ByteArray validHeader = createTestHandshakeHeader();
	authenticator.processHandshake(validHeader,1);

	// set the system to simulate a full buffer on socket 1 after 10 bytes are written
	systemWrap.SetBytesTillWriteFail(1, 10);
	// should return false with partial write
	EXPECT_EQ(false, authenticator.sendHandshake(1));
	EXPECT_EQ(false, authenticator.sendHandshake(1));

	// set system to allow all bytes to be written
	systemWrap.SetBytesTillWriteFail(1, -1);
	// should return true after writing all bytes
	EXPECT_EQ(true, authenticator.sendHandshake(1));

	// get the written data from the system
	std::string data = systemWrap.GetWriteBuffer(1);

	// check the data to make sure it is correct
	std::string expected = createTestResponseHandshakeHeader().toString();
	EXPECT_STREQ( data.c_str(), expected.c_str());
}

TEST(WebsocketAuthenticatorTest, closeFD)
{
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);
	FDs.addFD(1);

	ByteArray validHeader = createTestHandshakeHeader();

	// split validHeader into two parts
	ByteArray validHeaderPart1(5);
	memcpy(&validHeaderPart1[0], &validHeader[0], 5);
	ByteArray validHeaderPart2(validHeader.size()-5);
	memcpy(&validHeaderPart2[0], &validHeader[5], validHeader.size()-5);

	// fill write buffer
	authenticator.processHandshake(validHeader,1);
	// partial fill the read buffer
	authenticator.processHandshake(validHeaderPart1,1);

	// should erase the write and read buffers of FD 1
	authenticator.closeFD(1);

	// should throw because we erased the first half when the read buffer was erased
	ASSERT_THROW(authenticator.processHandshake(validHeaderPart2,1);, std::runtime_error);

	// should throw when called without a write buffer
	ASSERT_THROW(authenticator.sendHandshake(1), std::runtime_error);
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

	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	WebsocketAuthenticator authenticator(&systemWrap, &FDs);
	FDs.addFD(1);

	authenticator.setClientValidator(&validator);

	EXPECT_EQ(true,authenticator.isNotValidConnection(ByteArray("IPFail"), ByteArray("port")));
	EXPECT_EQ(true,authenticator.isNotValidConnection(ByteArray("IP"), ByteArray("PortFail")));
	EXPECT_EQ(false,authenticator.isNotValidConnection(ByteArray("IP"), ByteArray("port")));
}

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
