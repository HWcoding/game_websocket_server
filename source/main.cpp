
#include "source/server/html/landing_page_node.h"
#include "source/server/socket/socket_node.h"
#include "source/message_handlers/message_dispatcher.h"
#include "source/data_types/socket_message.h"
#include "source/logging/exception_handler.h"

int main(int argc, char** argv)
{
	try{
		std::cout<<"work in progress"<<std::endl;

		ServerConfig config;
		config.port = std::string("5600");

		MessageDispatcher dispatcher;
		Socket gameSocket(config);
		LandingPageNode landingPage(argc, argv);

		for(;;) {
			if(gameSocket.isRunning()){
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
