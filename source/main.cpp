#include "source/server/socket/socket_node.h"
#include "source/message_handlers/message_dispatcher.h"
#include "source/data_types/socket_message.h"
#include "source/logging/exception_handler.h"
#include "source/server/socket/websocket/websocket_authenticator.h"


class MyClientValidator : public ClientValidatorInterface
{
public:
	bool areClientHeadersValid(ConnectionHeaders &headers)
	{
		/*struct ConnectionHeaders
		{
			std::string IP {};
			std::string port {};
			std::string SecWebSocketProtocol {};
			std::string Cookie {};
		};*/

		(void)headers;
		//accept all connections
		return true;
	}
	bool isClientIPValid(std::string &IP, std::string &port)
	{
		(void)IP;
		(void)port;
		//accept all connections
		return true;
	}
	~MyClientValidator(){};
};

#include <thread>
#include <atomic>
class ThreadWrapper
{
public:
	template< class Function, class... Args >
	explicit ThreadWrapper( Function&& f, Args&&... args )
	{
		Thread = std::thread(f, args...);
	}

	~ThreadWrapper()
	{
		if(Thread.joinable()) {
			Thread.join(); //wait for thread to finish returning
		}
	}
private:
	std::thread Thread {};

};



namespace {
void messageLoop(Socket *gameSocket, std::atomic<bool> *run)
{
	try {

		MessageDispatcher dispatcher;

		while(run->load()) {
			if(gameSocket->isRunning()){
				//blocks thread if message queue is empty
				dispatcher.dispatchMessage( gameSocket->getNextMessage() );
			}
		}
	}
	//runs in separate thread so we need to catch all exceptions to avoid ABORT
	catch(...) {
		//set flag to indicate the thread has stopped running
		run->store(false);

		BACKTRACE_PRINT();
	}
}
} //namespace



#include "source/signal_handler.h"
int main()
{
	std::atomic<bool> run(true);
	SignalHandler *sigHandler = SignalHandler::getSignalHandler(&run);
	if(sigHandler == nullptr) throwInt("sigHandler is null");

	std::cout<<"Work in progress"<<std::endl;
	std::cout<<"Navigate a browser to localhost to test"<<std::endl;
	std::cout<<"You can move the green circle using w, a, s, and d"<<std::endl;
	std::cout<<"The circle's position will be printed in this console"<<std::endl;
	std::cout<<"\033[31mQuit by entering \"q\"\033[0m"<<std::endl;

	ServerConfig config;
	config.port = std::string("5590");
	Socket gameSocket(config);

	MyClientValidator validator;
	gameSocket.setClientValidator(&validator);

	ThreadWrapper loop(&::messageLoop, &gameSocket, &run);

//	int result = daemon(1, 1);
	while(run.load()) {
		std::string input;
		getline(std::cin, input);
		if(input.compare("q") == 0){
			break;
		}
	}
	std::cout<<"\033[1;32mExiting...\033[0m"<<std::endl;
	run.store(false);
	gameSocket.shutdown();

	return 0;
}
