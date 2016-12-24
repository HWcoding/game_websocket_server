#ifndef SOURCE_SERVER_HTML_HTTP_SERVER_NODE_H_
#define SOURCE_SERVER_HTML_HTTP_SERVER_NODE_H_
//#include "source/server/html/http_server_node.h"

#include <thread>


class LandingPageNode
{
public:
	LandingPageNode(int argc, char** argv);
	~LandingPageNode();
private:
	void serverThread(int argc, char** argv);
	std::thread HTTP_ServerThread;
};

void serverThread(int argc, char** argv);

#endif /* SOURCE_SERVER_HTML_HTTP_SERVER_NODE_H_ */
