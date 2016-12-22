#include "source/server/socket/socket_node.h"
#include <signal.h>

#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/socket_reader.h"
#include "source/server/socket/socket_writer.h"
#include "source/server/socket/socket_connector.h"
#include "source/server/socket/system_wrapper.h"
#include "source/data_types/socket_message.h"


SocketInterface::~SocketInterface(){}

Socket::Socket(const ServerConfig &config) : shouldContinueRunning(true),
									systemWrap( new SystemWrapper()),
									FDs( new SetOfFileDescriptors( systemWrap.get() ) ),
									reader(  new SocketReader(systemWrap.get(), FDs.get(), &shouldContinueRunning) ),
									writer( new SocketWriter(systemWrap.get(), FDs.get(), &shouldContinueRunning) ),
									connector( new SocketServerConnector(config.port, systemWrap.get(), FDs.get(), &shouldContinueRunning) ),
									readerThread(),
									writerThread(),
									connectorThread() {

	signal(SIGPIPE, SIG_IGN); //ignore sinal when writing to closed sockets to prevent crash on client disconnect
	//LOG_INFO("Socket", "starting Socket");
	shouldContinueRunning = true;
	readerThread = std::thread(&Socket::startReader, this);
	writerThread = std::thread(&Socket::startWriter, this);
	connectorThread = std::thread(&Socket::startConnector, this);
}

void Socket::disconnectClient(int FD){
	FDs->removeFD(FD);
}

Socket::~Socket(){
	shouldContinueRunning = false; //tell loop in socket thread to exit and return
	reader->shutdown();

	if(connectorThread.joinable()){
		//LOG_INFO("Socket", "connectorThread Exiting");
		connectorThread.join();//wait for thread to finish returning
	}

	if(readerThread.joinable()){
		//LOG_INFO("Socket", "readerThread Exiting");
		readerThread.join();//wait for thread to finish returning
	}

	if(writerThread.joinable()){
		//LOG_INFO("Socket", "writerThread Exiting");
		writerThread.join();//wait for thread to finish returning
	}

	//LOG_INFO("Socket", "Exited");
}


SocketMessage Socket::getNextMessage(){
	return reader->getNextMessage();
}

void Socket::sendMessage(SocketMessage &message){
	writer->sendMessage(message);
}

bool Socket::isRunning() {
	return shouldContinueRunning;
}




void Socket::startReader(){ //blocking! should only be called in a new thread
	//LOG_INFO("Socket::startReader","starting");
	try{
		reader->startPoll(); //loops until *shouldContinueRunning == false or error
	}
	catch(std::exception const &e) {
		//LOG_ERROR("Socket::startReader","exception thrown: " << e.what() );
		//BACKTRACE_PRINT();
	}
	catch(int &e) {
		//LOG_ERROR("Socket::startReader","int exception thrown: " << e);
		//BACKTRACE_PRINT();
	}
	catch(...){
		//LOG_ERROR("Socket::startReader","unknown exception thrown. Shutting down.");
		//BACKTRACE_PRINT();
	}
	//LOG_INFO("Socket::startReader","ending");
	shouldContinueRunning = false; //update the status of the server
	return;
}




void Socket::startWriter(){ //blocking! should only be called in a new thread
	//LOG_INFO("Socket::startWriter","starting");
	try{
		writer->startPoll(); //loops until *shouldContinueRunning == false
	}
	catch(std::exception const &e) {
		//LOG_ERROR("Socket::startWriter","exception thrown: " << e.what() );
		//BACKTRACE_PRINT();
	}
	catch(int &e) {
		//LOG_ERROR("Socket::startWriter","int exception thrown: " << e);
		//BACKTRACE_PRINT();
	}
	catch(...){
		//LOG_ERROR("Socket::startWriter","unknown exception thrown. Shutting down.");
		//BACKTRACE_PRINT();
	}
	//LOG_INFO("Socket::startWriter","ending");
	shouldContinueRunning = false; //update the status of the server
	return;
}



void Socket::startConnector(){ //blocking! should only be called in a new thread
	//LOG_INFO("Socket::startConnector","starting");
	try{
		connector->startPoll(); //loops until *shouldContinueRunning == false
	}
	catch(std::exception const &e) {
		//LOG_ERROR("Socket::startConnector","exception thrown: " << e.what() );
		//BACKTRACE_PRINT();
	}
	catch(int &e) {
		//LOG_ERROR("Socket::startConnector","int exception thrown: " << e);
		//BACKTRACE_PRINT();
	}
	catch(...){
		//LOG_ERROR("Socket::startConnector","unknown exception thrown. Shutting down.");
		//BACKTRACE_PRINT();
	}
	//LOG_INFO("Socket::startConnector","ending");
	shouldContinueRunning = false; //update the status of the server
	return;
}
