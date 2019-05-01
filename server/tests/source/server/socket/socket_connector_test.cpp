// list of comma separated files this test needs to be linked with; read by the build script
#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/socket_connector.cpp, \
                                    source/server/socket/file_descriptor.cpp, \
                                    source/data_types/byte_array.cpp, \
                                    source/server/socket/websocket/websocket_authenticator.cpp, \
                                    source/server/socket/websocket/websocket_handshake.cpp, \
                                    source/server/socket/set_of_file_descriptors.cpp, \
                                    source/data_types/reader_writer_lockguard.cpp, \
                                    source/server/socket/socket_node.cpp"

#include "tests/test.h"
#include "source/server/socket/socket_connector.h"

// mock of debug function used by socket_connector.cpp and set_of_file_descriptors.cpp
namespace DEBUG_BACKTRACE {
      void debugBackTrace(){}
}

TEST(SocketConnectorTest, CreateAndBindListeningFD) {

}

TEST(SocketConnectorTest, NewConnection) {

}

TEST(SocketConnectorTest, ReadHandshake) {

}

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
