#include "source/server/socket/socket_node.h"
#include <signal.h>

#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/socket_reader.h"
#include "source/server/socket/socket_writer.h"
#include "source/server/socket/socket_connector.h"
#include "source/server/socket/system_wrapper.h"
#include "source/data_types/socket_message.h"
#include "source/logging/exception_handler.h"


SocketInterface::~SocketInterface(){}

Socket::Socket(const ServerConfig &config) : shouldContinueRunning(),
									systemWrap( new SystemWrapper()),
									FDs( new SetOfFileDescriptors( systemWrap.get() ) ),
									reader(  new SocketReader(systemWrap.get(), FDs.get(), &shouldContinueRunning) ),
									writer( new SocketWriter(systemWrap.get(), FDs.get(), &shouldContinueRunning) ),
									connector( new SocketServerConnector(config.port, systemWrap.get(), FDs.get(), &shouldContinueRunning) ),
									readerThread(),
									writerThread(),
									connectorThread() {

	signal(SIGPIPE, SIG_IGN); //ignore sinal when writing to closed sockets to prevent crash on client disconnect
	LOG_INFO("starting Socket");
	shouldContinueRunning.store(true);
	readerThread = std::thread(&Socket::startReader, this);
	writerThread = std::thread(&Socket::startWriter, this);
	connectorThread = std::thread(&Socket::startConnector, this);
}

void Socket::disconnectClient(int FD){
	FDs->removeFD(FD);
}

void Socket::shutdown()
{
	shouldContinueRunning.store(false); //tell loop in socket thread to exit and return
	reader->shutdown();

	if(connectorThread.joinable()){
		LOG_INFO("connectorThread Exiting");
		connectorThread.join();//wait for thread to finish returning
	}

	if(readerThread.joinable()){
		LOG_INFO("readerThread Exiting");
		readerThread.join();//wait for thread to finish returning
	}

	if(writerThread.joinable()){
		LOG_INFO("writerThread Exiting");
		writerThread.join();//wait for thread to finish returning
	}

	LOG_INFO("Exited");
}

Socket::~Socket()
{
	shutdown();
}


SocketMessage Socket::getNextMessage(){
	return reader->getNextMessage();
}

void Socket::setClientValidator(ClientValidatorInterface * validator){
	connector->setClientValidator(validator);
}

void Socket::sendMessage(SocketMessage &message){
	writer->sendMessage(message);
}

bool Socket::isRunning() {
	return shouldContinueRunning.load();
}




void Socket::startReader(){ //blocking! should only be called in a new thread
	LOG_INFO("starting");
	try{
		reader->startPoll(); //loops until *shouldContinueRunning == false or error
	}
	catch(std::exception const &e) {
		BACKTRACE_PRINT();
		LOG_ERROR("exception thrown: " << e.what() );
	}
	catch(int &e) {
		BACKTRACE_PRINT();
		LOG_ERROR("int exception thrown: " << e);
	}
	catch(...){
		BACKTRACE_PRINT();
		LOG_ERROR("unknown exception thrown. Shutting down.");
	}
	LOG_INFO("ending");
	shouldContinueRunning = false; //update the status of the server
	return;
}




void Socket::startWriter(){ //blocking! should only be called in a new thread
	LOG_INFO("starting");
	try{
		writer->startPoll(); //loops until *shouldContinueRunning == false
	}
	catch(std::exception const &e) {
		BACKTRACE_PRINT();
		LOG_ERROR("exception thrown: " << e.what() );
	}
	catch(int &e) {
		BACKTRACE_PRINT();
		LOG_ERROR("int exception thrown: " << e);
	}
	catch(...){
		BACKTRACE_PRINT();
		LOG_ERROR("unknown exception thrown. Shutting down.");
	}
	LOG_INFO("ending");
	shouldContinueRunning = false; //update the status of the server
	return;
}



void Socket::startConnector(){ //blocking! should only be called in a new thread
	LOG_INFO("starting");
	try{
		connector->startPoll(); //loops until *shouldContinueRunning == false
	}
	catch(std::exception const &e) {
		BACKTRACE_PRINT();
		LOG_ERROR("exception thrown: " << e.what() );
	}
	catch(int &e) {
		BACKTRACE_PRINT();
		LOG_ERROR("int exception thrown: " << e);
	}
	catch(...){
		BACKTRACE_PRINT();
		LOG_ERROR("unknown exception thrown. Shutting down.");
	}
	LOG_INFO("ending");
	shouldContinueRunning = false; //update the status of the server
	return;
}
