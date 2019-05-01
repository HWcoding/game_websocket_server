#include "source/server/socket/socket.h"
#include "source/message_handlers/message_dispatcher.h"
#include "source/data_types/socket_message.h"
#include "source/logging/exception_handler.h"
#include "source/server/socket/websocket/websocket_authenticator.h"
#include "source/signal_handler.h"
#include <thread>
#include <atomic>


class ThreadWrapper
{
public:
	template< class Function, class... Args >
	explicit ThreadWrapper( Function&& f, Args&&... args ): Thread(f, args...){}

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
			dispatcher.dispatchMessage( gameSocket->getNextMessage() );
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




int main()
{
	std::atomic<bool> run(true);
	SignalHandler::setSignalHandler(&run);

	std::cout<<"Work in progress"<<std::endl;
	std::cout<<"Navigate a browser to localhost to test"<<std::endl;
	std::cout<<"You can move the green circle using w, a, s, and d"<<std::endl;
	std::cout<<"The circle's position will be printed in this console"<<std::endl;
	std::cout<<"\033[31mQuit by entering \"q\"\033[0m"<<std::endl;

	ServerConfig config;
	config.port = std::string("5590");
	Socket gameSocket(config, &run);

	// totally insecure validator
	class MyClientValidator : public ClientValidatorInterface
	{
	public:

		bool areClientHeadersValid(ConnectionHeaders &headers) override
		{
			(void)headers;
			//accept all connections
			return true;
		}
		bool isClientIPValid(std::string &ip, std::string &port) override
		{
			(void)ip;
			(void)port;
			//accept all connections
			return true;
		}
		~MyClientValidator() override = default;
	};

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
