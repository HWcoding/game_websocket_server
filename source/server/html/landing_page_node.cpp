#include "source/server/html/http_server_node.h"
#include "source/server/html/landing_page_node.h"
#include "source/logging/exception_handler.h"


LandingPageNode::LandingPageNode(int argc, char** argv) : HTTP_ServerThread()
{
	HTTP_ServerThread = std::thread(&LandingPageNode::serverThread, this, argc, argv);
}

LandingPageNode::~LandingPageNode()
{
	if(HTTP_ServerThread.joinable()){
		LOG_INFO("HTTP_ServerThread Exiting");
		HTTP_ServerThread.join();//wait for thread to finish returning
	}
}

void LandingPageNode::serverThread(int argc, char** argv)
{
	try{

		HTTPGameServer app;
		int ret = app.run(argc, argv);
		std::cout<<"serverThread returned "<<ret<<std::endl;
	}
	catch (...) {
		std::cout<<"exception thrown"<<std::endl;
		BACKTRACE_PRINT();
	}

}
