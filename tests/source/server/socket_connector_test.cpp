/*#define TESTING //to activate conditional macros for test logging and access
#include "tests/test.h"
#include "main/includes.h"
#include <string>
#include "tests/server/mocks_stubs/mock_system_wrapper.h"
#include "tests/server/mocks_stubs/socket_test_helpers.h"
#include "tests/server/socket_connector_test/socket_connector_test.h"
//#include "server/socket/set_of_file_descriptors.h"
#include "server/socket/socket_connector.h"

namespace SocketConnector_Test{

void test_CreateAndBindListeningFD(){

}

void test_NewConnection(){

}

void test_ReadHandshake(){

}



void test(){
	test_CreateAndBindListeningFD();
	test_NewConnection();
	test_ReadHandshake();
}

}


int main(){
	SocketConnector_Test::test();
	return 0;
}*/
//int main(){return 0;}

#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/socket_connector.cpp, \
                                    source/server/socket/file_descriptor.cpp, \
                                    source/logging/exception_handler.cpp, \
                                    source/data_types/byte_array.cpp, \
                                    source/server/socket/websocket/websocket_authenticator.cpp, \
                                    source/server/socket/websocket/websocket_handshake.cpp, \
                                    source/server/socket/set_of_file_descriptors.cpp, \
                                    source/server/socket/socket_node.cpp"




/*#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/socket_connector.cpp, \
                                    source/server/socket/system_wrapper.cpp, \
                                    source/server/socket/set_of_file_descriptors.cpp, \
                                    source/server/socket/websocket/websocket_authenticator.cpp, \
                                    source/logging/exception_handler.cpp, \
                                    source/data_types/byte_array.cpp"*/

#include "tests/test.h"
#include "source/server/socket/socket_connector.h"

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
