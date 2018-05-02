#include "source/server/socket/socket.h"
#include <csignal>

#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/socket_reader.h"
#include "source/server/socket/socket_writer.h"
#include "source/server/socket/socket_connector.h"
#include "source/server/socket/system_wrapper.h"
#include "source/data_types/socket_message.h"
#include "source/logging/exception_handler.h"


SocketInterface::~SocketInterface() = default;

Socket::Socket(const ServerConfig &config, std::atomic<bool> * _shouldContinueRunning) : shouldContinueRunning(_shouldContinueRunning),
									FDs( new SetOfFileDescriptors() ),
									reader(  new SocketReader(FDs.get(), shouldContinueRunning) ),
									writer( new SocketWriter( FDs.get(), shouldContinueRunning) ),
									connector( new SocketServerConnector(config.port, FDs.get(), shouldContinueRunning) ),
									readerThread(),
									writerThread(),
									connectorThread() {

	signal(SIGPIPE, SIG_IGN); //ignore sinal when writing to closed sockets to prevent crash on client disconnect
	LOG_INFO("starting Socket");
	readerThread = std::thread(&Socket::startReader, this);
	writerThread = std::thread(&Socket::startWriter, this);
	connectorThread = std::thread(&Socket::startConnector, this);
}

void Socket::disconnectClient(int FD){
	FDs->removeFD(FD);
}

void Socket::shutdown()
{
	shouldContinueRunning->store(false); //tell loop in socket thread to exit and return
	reader->shutdown();
	LOG_INFO("Exiting. Waiting on connectorThread");
	if(connectorThread.joinable()){
		connectorThread.join();//wait for thread to finish returning
		LOG_INFO("connectorThread Exited. Waiting on readerThread");
	}

	if(readerThread.joinable()){
		readerThread.join();//wait for thread to finish returning
		LOG_INFO("readerThread Exited. Waiting on writerThread");
	}

	if(writerThread.joinable()){
		writerThread.join();//wait for thread to finish returning
		LOG_INFO("writerThread Exited");
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
	return shouldContinueRunning->load();
}




void Socket::startReader(){ //blocking! should only be called in a new thread
	LOG_INFO("starting");
	try{
		reader->startPoll(); //loops until *shouldContinueRunning == false or error
	}
	catch(std::runtime_error const &e) {
		BACKTRACE_PRINT();
		LOG_ERROR("runtime exception thrown: " << e.what() );
	}
	catch(std::exception const &e) {
		BACKTRACE_PRINT();
		LOG_ERROR("exception thrown: " << e.what() );
	}
	catch(...){
		BACKTRACE_PRINT();
		LOG_ERROR("unknown exception thrown. Shutting down.");
	}
	LOG_INFO("ending");
	shouldContinueRunning->store(false); //update the status of the server
	return;
}




void Socket::startWriter(){ //blocking! should only be called in a new thread
	LOG_INFO("starting");
	try{
		writer->startPoll(); //loops until *shouldContinueRunning == false
	}
	catch(std::runtime_error const &e) {
		BACKTRACE_PRINT();
		LOG_ERROR("runtime exception thrown: " << e.what());
	}
	catch(std::exception const &e) {
		BACKTRACE_PRINT();
		LOG_ERROR("exception thrown: " << e.what() );
	}
	catch(...){
		BACKTRACE_PRINT();
		LOG_ERROR("unknown exception thrown. Shutting down.");
	}
	LOG_INFO("ending");
	shouldContinueRunning->store(false); //update the status of the server
	return;
}



void Socket::startConnector(){ //blocking! should only be called in a new thread
	LOG_INFO("starting");
	try{
		connector->startPoll(); //loops until *shouldContinueRunning == false
	}
	catch(std::runtime_error const &e) {
		BACKTRACE_PRINT();
		LOG_ERROR("runtime exception thrown: " << e.what());
	}
	catch(std::exception const &e) {
		BACKTRACE_PRINT();
		LOG_ERROR("exception thrown: " << e.what() );
	}
	catch(...){
		BACKTRACE_PRINT();
		LOG_ERROR("unknown exception thrown. Shutting down.");
	}
	LOG_INFO("ending");
	shouldContinueRunning->store(false); //update the status of the server
	return;
}
