#include "source/server/html/http_server_node.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/ServerSocket.h"
#include <fstream>
#include <iostream>

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Util::Application;

namespace {
std::string loadFileToString(std::string filename)
{
	std::ifstream t( filename.c_str() );
	t.seekg(0, std::ios::end);
	int64_t size = t.tellg();
	if(size == -1) throw -1;
	std::string buffer(static_cast<size_t>(size), ' ');
	t.seekg(0);
	if(t.fail()) throw -1;
	t.read(&buffer[0], size);
	if(t.fail()) throw -1;
	return buffer;
}
}


IndexPageRequestHandler::IndexPageRequestHandler(const std::string& format):
		responseString(loadFileToString("./client/Testindex.html")),
		_format(format)
	{}

void IndexPageRequestHandler::handleRequest(HTTPServerRequest& request,
                              HTTPServerResponse& response)
{
	Application& app = Application::instance();
	app.logger().information("Request from "
	    + request.clientAddress().toString());

	response.setChunkedTransferEncoding(true);
	response.setContentType("text/html");
	response.setKeepAlive(true);

	// set cookie in response
	Poco::Net::HTTPCookie cookie("name", "TEST_COOKIE");
	cookie.setPath("/");
	cookie.setMaxAge(3600); // 1 hour
	response.addCookie(cookie);

	std::ostream& ostr = response.send();
	ostr << responseString;
}










HTTPRequestHandler* IndexPageRequestHandlerFactory::createRequestHandler(
	                                        const HTTPServerRequest& request)
{
	if (request.getURI() == "/")
		return new IndexPageRequestHandler(_format);
	else
		return 0;
}







void HTTPGameServer::initialize(Application& self)
{
	loadConfiguration();
	ServerApplication::initialize(self);
}

void HTTPGameServer::uninitialize()
{
	ServerApplication::uninitialize();
}


int HTTPGameServer::main(const std::vector<std::string>& args)
{
	(void)args;
	if (!_helpRequested)
	{
		unsigned short port = (unsigned short)
			config().getInt("HTTPGameServer.port", 8080);
		std::string format(
			config().getString("HTTPGameServer.format", ""));

		ServerSocket svs(port);
		HTTPServer srv(new IndexPageRequestHandlerFactory(format),
			svs, new HTTPServerParams);
		srv.start();
		waitForTerminationRequest();
		srv.stop();
	}
	return Application::EXIT_OK;
}
