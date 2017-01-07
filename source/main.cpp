
#include "source/server/socket/socket_node.h"
#include "source/message_handlers/message_dispatcher.h"
#include "source/data_types/socket_message.h"
#include "source/logging/exception_handler.h"

int main()
{
	try {
		std::cout<<"Work in progress"<<std::endl;
		std::cout<<"Navigate a browser to localhost to test"<<std::endl;
		std::cout<<"You can move the green circle using w, a, s, and d"<<std::endl;
		std::cout<<"The circle's position will be printed in this console"<<std::endl;
		std::cout<<"\033[31mExit using ctl-c\033[0m"<<std::endl;

		ServerConfig config;
		config.port = std::string("5600");

		Socket gameSocket(config);

		MessageDispatcher dispatcher;

		for(;;) {
			if(gameSocket.isRunning()){
				//blocks thread if message queue is empty
				dispatcher.dispatchMessage( gameSocket.getNextMessage() );
			}
		}
	}
	catch(...) {
		BACKTRACE_PRINT();
		return 1;
	}

	return 0;
}
