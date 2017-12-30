#include "source/server/socket/socket_connector.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/system_wrapper.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <signal.h>
#include <netdb.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <string.h>
#include "source/server/socket/websocket/websocket_authenticator.h"
#include "source/data_types/byte_array.h"
#include "source/logging/exception_handler.h"



SocketServerConnector::SocketServerConnector(const std::string &_port,
                                             SystemInterface *_systemWrap,
                                             SetOfFileDescriptors *FDs,
                                             std::atomic<bool>* run) : SocketNode(_systemWrap, FDs, run),
                                                                       Authenticator(new WebsocketAuthenticator(_systemWrap, FDs)),
                                                                       maxMessageSize(99999),
                                                                       port(_port),
                                                                       listeningFD(-1)
{
	signal(SIGPIPE, SIG_IGN); //ignore error when writing to closed sockets to prevent crash on client disconnect
}


SocketServerConnector::~SocketServerConnector(){
	if(listeningFD != -1){
		fileDescriptors->removeFD(listeningFD);
		listeningFD = -1;
	}
}


void SocketServerConnector::closeFD(int FD){
	Authenticator->closeFD(FD);
	SocketNode::closeFD(FD);
}


int SocketServerConnector::addFD(int FD){
	return fileDescriptors->addFD(FD);
}

bool SocketServerConnector::handleEpollErrors(epoll_event &event){
	if ( (event.events & EPOLLERR) || (event.events & EPOLLHUP) ){// An error occured
		LOG_ERROR("epoll error");
		int error = 0;
		socklen_t errlen = sizeof(error);
		if (systemWrap->getSockOpt(event.data.fd, SOL_SOCKET, SO_ERROR, reinterpret_cast<void *>(&error), &errlen) == 0){
			LOG_ERROR("error: " << systemWrap->strError(error) );
		}
		closeFD(event.data.fd);

		if(event.data.fd == listeningFD){ //error is on listening FD
			listeningFD = -1;
			throw std::runtime_error(LOG_EXCEPTION(std::string() + std::strerror(errno) + " in epoll")); //server can't continue
		}
		return true; //there was an error
	}
	return false;
}

void SocketServerConnector::handleEpollRead(epoll_event &event){
	if((event.events & EPOLLIN)) {			//the socket is ready for reading
		if(listeningFD == event.data.fd) {	//There is a notification on the listening socket. one or more new connections.
			try{
				newConnection(); 			//connect to new client
			}
			catch(std::runtime_error &ret) {
				BACKTRACE_PRINT();
			}
		}
		else{ //there is a handshake from client
			try{
				readHandshake(event.data.fd);
			}
			catch(std::runtime_error &ret) {
				BACKTRACE_PRINT();
				LOG_INFO("threw while calling readHandshake(" << event.data.fd << ")");
				closeFD(event.data.fd);
			}
		}
	}
}

void SocketServerConnector::handleEpollWrite(epoll_event &event){
	if(event.events & EPOLLOUT){			//the socket is ready for Writing; client is waiting on handshake
		try{
			sendHandshake(event.data.fd);
		}
		catch(std::runtime_error &ret){
			BACKTRACE_PRINT();
			LOG_INFO("threw while calling sendHandshake(" << event.data.fd << ")");
			closeFD(event.data.fd);
		}
	}
}

void SocketServerConnector::setClientValidator(ClientValidatorInterface * validator)
{
	Authenticator->setClientValidator(validator);
}

void SocketServerConnector::setupEpoll(){
	SocketNode::setupEpoll();
	listeningFD = getListeningPort();
	openListeningPort();
}

bool SocketServerConnector::createAndBindListeningFD(struct addrinfo *addressInfo){
	struct addrinfo *rp = nullptr;

	for(rp = addressInfo; rp != nullptr; rp = rp->ai_next){
		try{
			listeningFD = systemWrap->createSocket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		}
		catch(std::runtime_error & ret){
			continue;
		}

		try{
			systemWrap->bindSocket(listeningFD, rp->ai_addr, rp->ai_addrlen);
			addFD(listeningFD);
			return true;
		}
		catch(std::runtime_error & ret){
			systemWrap->closeFD(listeningFD);
			listeningFD = -1;
		}
	}
	return false;
}

void SocketServerConnector::createAddressInfoHints(struct addrinfo &hints){
	memset (&hints, 0, sizeof (struct addrinfo));
	hints.ai_family = AF_UNSPEC;	 // Return IPv4 and IPv6
	hints.ai_socktype = SOCK_STREAM; // A TCP socket
	hints.ai_flags = AI_PASSIVE;	 // All interfaces
}

int SocketServerConnector::getListeningPort(){
	struct addrinfo hints;
	struct addrinfo *result;
	result = nullptr;
	createAddressInfoHints(hints);
	systemWrap->getAddrInfo(nullptr, port.c_str(), &hints, &result);
	LOG_INFO("Trying to bind to port " << port);

	bool done = false;
	while(!done && *running ){//try to connect to port until server shutsdown or we succeed
		done = createAndBindListeningFD(result);
		if(!done && *running ){
			std::this_thread::sleep_for( std::chrono::milliseconds(100) ); //port was busy, take a break before retrying
		}
	}
	systemWrap->freeAddrInfo (result);

	if (!done){
		throw std::runtime_error(LOG_EXCEPTION(std::string() + "Could not bind port: " + port));
	}
	else{
		LOG_INFO("Bound port: " << port << " on FD " << listeningFD);
	}
	return listeningFD;
}


void SocketServerConnector::openListeningPort(){
	if(listeningFD == -1){
		throw std::runtime_error(LOG_EXCEPTION(" listeningFD equals -1"));
	}
	fileDescriptors->makeNonblocking(listeningFD);
	systemWrap->listenSocket(listeningFD, SOMAXCONN);
	fileDescriptors->startPollingForRead(epollFD, listeningFD);
}


void SocketServerConnector::getPortAndIP(int FD, struct sockaddr &in_addr, unsigned int &in_len, ByteArray &PortBuff, ByteArray &IPBuff){
	(void)FD;
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	memset(&hbuf[0], 0, NI_MAXHOST);
	memset(&sbuf[0], 0, NI_MAXSERV);

	systemWrap->getNameInfo(&in_addr, in_len, hbuf, sizeof hbuf, sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
	LOG_INFO("Accepted connection on descriptor " << FD << " (host=" << hbuf << ", port=" << sbuf << ")");

	size_t hbufLength = strnlen(hbuf,NI_MAXHOST);
	size_t sbufLength = strnlen(sbuf,NI_MAXSERV);

	IPBuff = ByteArray(hbufLength);
	PortBuff = ByteArray(sbufLength);

	memcpy(&IPBuff[0], hbuf, hbufLength);
	memcpy(&PortBuff[0], sbuf, sbufLength);
}


void SocketServerConnector::newConnection(){
	while (*running){
		struct sockaddr in_addr;
		socklen_t in_len = sizeof in_addr;
		bool done;

		int newConnection = fileDescriptors->addFD(systemWrap->acceptSocket(listeningFD, &in_addr, &in_len, done));
		if (done){
			break; //We have processed all connections.
		}

		ByteArray IPBuff;
		ByteArray PortBuff;

		try{
			getPortAndIP(newConnection, in_addr, in_len, PortBuff, IPBuff);
			if(Authenticator->isNotValidConnection(IPBuff,PortBuff)){
				throw std::runtime_error(LOG_EXCEPTION(" descriptor " + std::to_string(newConnection) + \
					" (IP="+std::string(reinterpret_cast<char *>(&IPBuff[0]),IPBuff.size() ) + \
					", port="+ std::string(reinterpret_cast<char *>(&PortBuff[0]),PortBuff.size() ) + \
					")"+ " failed validation"));
			}

			fileDescriptors->setIP(newConnection, IPBuff);
			fileDescriptors->setPort(newConnection, PortBuff);
		}
		catch(std::runtime_error &ret) {
			closeFD(newConnection);
			throw;
		}

		fileDescriptors->makeNonblocking(newConnection);
		waitForHandshake(newConnection);
	}
}

/**
 * @throws std::runtime_error
 */
void SocketServerConnector::waitForHandshake(int FD) {
	if(FD>0){ //a negative number is an error message passed through this function.
		try{
			fileDescriptors->startPollingForRead(epollFD, FD);
		}
		catch (std::runtime_error &ret){
			closeFD(FD);
			throw;
		}
	}
	else {
		throw std::runtime_error(LOG_EXCEPTION(std::string() + std::to_string(FD) + " is an invalid file descriptor"));
	}
}


void SocketServerConnector::readHandshake(int FD){
	const size_t buffSize = 2049;
	ByteArray buffer;
	std::array<uint8_t, buffSize> buf;
	while (*running){
		bool done;
		size_t count = systemWrap->readFD(FD, &buf[0], buffSize-1, done);
		if(done){
			processHandshake(buffer, FD);
			break;
		}
		else if (count == 0){	//End of file. Remote closed connection.
			throw std::runtime_error(LOG_EXCEPTION("end of file on " + std::to_string(FD)));
		}
		else if(( buffer.size() + count ) > maxMessageSize){
			throw std::runtime_error(LOG_EXCEPTION("handshake too large.  Size: " + std::to_string(buffer.size())));
		}
		size_t end = buffer.size();
		buffer.resize(end + count);
		memcpy(&buffer[end],&buf[0],count);
	}
}


void SocketServerConnector::handshakeComplete(int FD){
	fileDescriptors->stopPollingFD(epollFD, FD);
	fileDescriptors->tellServerAboutNewConnection(FD);
	LOG_INFO("Handshake Complete");
}


void SocketServerConnector::processHandshake(ByteArray &in, int FD){
	Authenticator->processHandshake(in, FD);
	fileDescriptors->startPollingForWrite(epollFD, FD);//switch from reading to writing
	LOG_INFO("Handshake processed");
}


void SocketServerConnector::sendHandshake(int FD){
	bool done = Authenticator->sendHandshake(FD);
	if(done){ //we wrote all our data
		handshakeComplete(FD);
	}
}
