

/*#include "Poco/Net/DNS.h"
#include <iostream>
using Poco::Net::DNS;
using Poco::Net::IPAddress;
using Poco::Net::HostEntry;

int main()
{
	const HostEntry& entry = DNS::hostByName("www.hwcoding.com");
	std::cout << "Canonical Name: " << entry.name() << std::endl;

	const HostEntry::AliasList& aliases = entry.aliases();
	for (auto it = aliases.begin(); it != aliases.end(); ++it)
		std::cout << "Alias: " << *it << std::endl;

	const HostEntry::AddressList& addrs = entry.addresses();
	for (auto it = addrs.begin(); it != addrs.end(); ++it)
		std::cout << "Address: " << it->toString() << std::endl;
}
*/
/*
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/StreamCopier.h"
#include <iostream>
#include <thread>
#include <chrono>






#include "Poco/Net/ServerSocket.h"



void serverThread(Poco::Net::ServerSocket srv);
void clientThread();

void serverThread(Poco::Net::ServerSocket srv)
{
	//Poco::Net::ServerSocket srv(8080); // does bind + listen

	for (;;)
	{
		using namespace std::chrono_literals;
		//std::this_thread::sleep_for(1000ms);
		Poco::Net::StreamSocket ss = srv.acceptConnection();
		Poco::Net::SocketStream str(ss);
		std::string response("HTTP/1.1 200 OK\r\n"
		                      "Content-Type: text/html\r\n"
		                      "\r\n");
		for(int i = 1; i != 0; i--){
			response.append("<html><head><title>My 1st Web Server</title></head>"
			                 "<body><h1>Hello, world!</h1></body></html>");
		}
		str <<response<<std::flush;

		//str << "HTTP/1.1 200 OK\r\n"
		//	"Content-Type: text/html\r\n"
		//	"\r\n"

		//	"<html><head><title>My 1st Web Server</title></head>"
		//	"<body><h1>Hello, world!</h1></body></html>"
		//	<< std::flush;
	}

}


void clientThread()
{

	//for(;;){
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(120ms);
		Poco::Net::SocketAddress sa("localhost", 8080);
		Poco::Net::StreamSocket socket(sa);
		Poco::Net::SocketStream str(socket);
		str << "GET / HTTP/1.1\r\n"
			"Host: localhost\r\n"
			"\r\n"
			<< std::flush;
		//str.flush();
		std::string test;
		while (Poco::StreamCopier::copyToString(str, test)>0){}
		if(test.size()>0) {
			std::cout<<test<<"     size "<<test.size()<<std::endl;
		//	break;
		}
		//std::string test;
		//str>>test;
		//std::string test(std::istreambuf_iterator<char>(str), {});
		//std::cout<<test;
	//}
}












int main()
{
	Poco::Net::ServerSocket srv(8080); // does bind + listen
	std::thread t1(serverThread, srv);
	std::thread t2(clientThread);
	//Join the thread with the main thread
	t1.join();
	t2.join();
	//Poco::StreamCopier::copyStream(str, std::cout);
}
*/

/*struct ServerConfig {
	std::string port = std::string();
	int64_t loopSpeed = 100;
	int MAXEVENTS = 200;
	size_t MaxReaderSocketBufferSize = 32760;
	size_t MaxWebsocketReadBufferSize = 262144;
	int maxWaitTime = 1000;
	size_t maxHandshakeSize = 2048;
	ServerConfig() {}
};*/

#include <iostream>
#include <thread>
#include <chrono>
#include "source/server/socket/socket_node.h"
int main(){
	std::cout<<"work in progress"<<std::endl;

	ServerConfig config;
	config.port = std::string("5600");

	Socket gameSocket(config);

	for(;;) {
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(5s);
	}

	return 0;
}
