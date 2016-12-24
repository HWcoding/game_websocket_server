#ifndef SOURCE_SERVER_HTML_LANDING_PAGE_NODE_H_
#define SOURCE_SERVER_HTML_LANDING_PAGE_NODE_H_
//#include "source/server/html/landing_page_node.h"

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Util/ServerApplication.h"
#include <string>

//forward declarations
namespace Poco {
	namespace Net {
		class HTTPServerRequest;
		class HTTPServerResponse;
	}
	namespace Util {
		class OptionSet;
	}
} // Poco



class IndexPageRequestHandler:
public Poco::Net::HTTPRequestHandler
{
public:
	IndexPageRequestHandler(const std::string& format);

	void handleRequest(Poco::Net::HTTPServerRequest& request,
					   Poco::Net::HTTPServerResponse& response);

private:
	std::string responseString {};
	std::string _format;
};



class IndexPageRequestHandlerFactory:
public Poco::Net::HTTPRequestHandlerFactory
{
public:
	IndexPageRequestHandlerFactory(const std::string& format): _format(format) {}

	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

private:
	std::string _format;
};



class HTTPGameServer: public Poco::Util::ServerApplication
{
public:
	HTTPGameServer(): _helpRequested(false)	{}
	~HTTPGameServer() {}

protected:
	void initialize(Application& self);

	void uninitialize();

	void defineOptions(Poco::Util::OptionSet& options);

	void handleHelp(const std::string& name,
					const std::string& value);

	int main(const std::vector<std::string>& args);

private:
	bool _helpRequested;
};


#endif /* SOURCE_SERVER_HTML_LANDING_PAGE_NODE_H_ */
