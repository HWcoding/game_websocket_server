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
		LOG_INFO("HTTP_ServerThread returned "<<ret);
		(void)ret; //stops unused warning in release build;
	}
	catch (...) {
		LOG_ERROR("exception thrown");
		BACKTRACE_PRINT();
	}

}
