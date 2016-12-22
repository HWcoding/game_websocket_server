#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include "source/server/socket/socket_node.h"
#include "source/message_handlers/message_dispatcher.h"
#include "source/data_types/socket_message.h"
#include "source/logging/exception_handler.h"

std::string loadFileToString(std::string filename);
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

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;

class TimeRequestHandler: public HTTPRequestHandler
{
public:
	TimeRequestHandler(const std::string& format): _format(format)
	{
	}

	void handleRequest(HTTPServerRequest& request,
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

		std::string responseString = loadFileToString("./client/Testindex.html");



		std::ostream& ostr = response.send();
		ostr << responseString;
	}

private:
	std::string _format;
};

class TimeRequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
	TimeRequestHandlerFactory(const std::string& format):
		_format(format)
	{
	}

	HTTPRequestHandler* createRequestHandler(
		const HTTPServerRequest& request)
	{
		if (request.getURI() == "/")
			return new TimeRequestHandler(_format);
		else
			return 0;
	}

private:
	std::string _format;
};

class HTTPTimeServer: public Poco::Util::ServerApplication
{
public:
	HTTPTimeServer(): _helpRequested(false)
	{
	}

	~HTTPTimeServer()
	{
	}

protected:
	void initialize(Application& self)
	{
		loadConfiguration();
		ServerApplication::initialize(self);
	}

	void uninitialize()
	{
		ServerApplication::uninitialize();
	}

	void defineOptions(OptionSet& options)
	{
		ServerApplication::defineOptions(options);

		options.addOption(
		Option("help", "h", "display argument help information")
			.required(false)
			.repeatable(false)
			.callback(OptionCallback<HTTPTimeServer>(
				this, &HTTPTimeServer::handleHelp)));
	}

	void handleHelp(const std::string& name,
					const std::string& value)
	{
		(void)name;
		(void)value;
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader(
			"A web server that serves the current date and time.");
		helpFormatter.format(std::cout);
		stopOptionsProcessing();
		_helpRequested = true;
	}

	int main(const std::vector<std::string>& args)
	{
		(void)args;
		if (!_helpRequested)
		{
			unsigned short port = (unsigned short)
				config().getInt("HTTPTimeServer.port", 8080);
			std::string format(
				config().getString("HTTPTimeServer.format",
								   DateTimeFormat::SORTABLE_FORMAT));

			ServerSocket svs(port);
			HTTPServer srv(new TimeRequestHandlerFactory(format),
				svs, new HTTPServerParams);
			srv.start();
			waitForTerminationRequest();
			srv.stop();
		}
		return Application::EXIT_OK;
	}

private:
	bool _helpRequested;
};



void serverThread(int argc, char** argv);
void serverThread(int argc, char** argv)
{
	try{

		HTTPTimeServer app;
		int ret = app.run(argc, argv);
		std::cout<<"serverThread returned "<<ret<<std::endl;
	}
	catch (...) {
		std::cout<<"exception thrown"<<std::endl;
		BACKTRACE_PRINT();
	}

}

int main(int argc, char** argv)
{
	std::thread t1;
	try{
		std::cout<<"work in progress"<<std::endl;

		ServerConfig config;
		config.port = std::string("5600");

		MessageDispatcher dispatcher;
		Socket gameSocket(config);


		t1 = std::thread(serverThread, argc, argv);


		for(;;) {
			if(gameSocket.isRunning()){
				dispatcher.dispatchMessage( gameSocket.getNextMessage() );
			}
		}

	}
	catch(...) {
		BACKTRACE_PRINT();
	}
	t1.join();

	return 0;
}
