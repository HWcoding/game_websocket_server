
#include "tests/test_lib/mocks_stubs/mock_system_wrapper.h"

#include <limits>
#include <errno.h>
#include <cstring> //for memset/memcpy
#include <map>
#include <vector>
#include <sys/epoll.h> //for type definitions
#include <sys/socket.h> //for type definitions
#include <sys/types.h>
#include <netdb.h>


class MockSocket
{
	bool hasNewEvent;
	struct epoll_event event;

	std::string	socketReadBuffer;
	std::string	socketWriteBuffer;

	int flags;
	int family;
	int socktype;
	int protocol;
	struct sockaddr_storage address;

	ssize_t bytesUntilWriteFail; //number of bytes to write before simulating a full buffer. A negative value turns off the feature. -1 by default
	ssize_t bytesUntilReadFail; //number of bytes to read before simulating an empty buffer. A negative value turns off the feature. -1 by default
public:
	MockSocket();
	bool hasEvent();
	const struct epoll_event &getEvent();
	int getFlags();
	void setFlags(int _flags);

	void setBytesUntilWriteFail(ssize_t bytes);
	void setBytesUntilReadFail(ssize_t bytes);

	void setSocketWriteBuffer(std::string buf);
	void setSocketReadBuffer(std::string buf);

	const std::string &getSocketWriteBuffer();
	const std::string &getSocketReadBuffer();

	void create(int _family, int _socktype, int _protocol);
	int bind(const struct sockaddr *ai_addr, unsigned int ai_addrlen);
	size_t readDataFromReadBuffer(char* buffer, size_t amount);
	size_t writeDataToWriteBuffer(const char* buffer, size_t amount);
};


MockSocket::MockSocket() : hasNewEvent(false),  event(), socketReadBuffer(),
                           socketWriteBuffer(), flags(0), family(0), socktype(0),
                           protocol(0), address(), bytesUntilWriteFail(-1),
                           bytesUntilReadFail(-1) {
	memset(&address,0, sizeof(address));
	event.data.fd = -1;
	event.events = 0;
}

void MockSocket::setBytesUntilWriteFail(ssize_t bytes) {
	bytesUntilWriteFail = bytes;
}

void MockSocket::setBytesUntilReadFail(ssize_t bytes) {
	bytesUntilReadFail = bytes;
}

bool MockSocket::hasEvent() {
	return hasNewEvent & event.events;
}

const struct epoll_event &MockSocket::getEvent() {
	return event;
}

int MockSocket::getFlags() {
	return flags;
}

void MockSocket::setFlags(int _flags) {
	flags = _flags;
}

void MockSocket::setSocketWriteBuffer(std::string buf) {
	socketWriteBuffer = std::move(buf);
}

void MockSocket::setSocketReadBuffer(std::string buf) {
	socketReadBuffer = std::move(buf);
}

const std::string &MockSocket::getSocketWriteBuffer() {
	return socketWriteBuffer;
}

const std::string &MockSocket::getSocketReadBuffer() {
	return socketReadBuffer;
}

void MockSocket::create(int _family, int _socktype, int _protocol) {
	family = _family;
	socktype = _socktype;
	protocol = _protocol;
}

int MockSocket::bind(const struct sockaddr *ai_addr, unsigned int ai_addrlen) {
	size_t length = static_cast<size_t>(ai_addrlen);
	if(length < sizeof(address)){
		memcpy(&address, ai_addr, length);
		return 0;
	}
	else return -1; //TODO write an error
}

size_t MockSocket::readDataFromReadBuffer(char* buffer, size_t amount) {
	//if the amount to read is more than the bytes until a simulated empty buffer, set the amount to the bytes fail value
	if(bytesUntilReadFail >= 0 && static_cast<size_t>(bytesUntilReadFail) < amount) {
		amount = static_cast<size_t>(bytesUntilReadFail);
	}
	//if the amount to read is more than is left in the readBuffer, set it to the amount left in the read buffer
	size_t length = amount < socketReadBuffer.size() ? amount : socketReadBuffer.size();
	memcpy(buffer, socketReadBuffer.c_str(), length);
	//set the read buffer to contain the remaining unread bits
	if( length < socketReadBuffer.size() ) {
		socketReadBuffer = socketReadBuffer.substr(length);
	}
	//clear the read buffer if all bits are read
	else {
		socketReadBuffer = std::string();
	}
	return length;
}

size_t MockSocket::writeDataToWriteBuffer(const char* buffer, size_t amount) {
	//if the amount to write is more than the bytes until a simulated full buffer, set the amount to the bytes fail value
	if(bytesUntilWriteFail >= 0 && static_cast<size_t>(bytesUntilWriteFail) < amount) {
		amount = static_cast<size_t>(bytesUntilWriteFail);
	}
	socketWriteBuffer.append(buffer, amount);
	return amount;
}













class MockEpoll
{
//public:
	std::map<int,struct epoll_event> monitoredFDs;
	std::vector<struct epoll_event> events;
public:
	MockEpoll();
	void fillEvents(std::map<int,MockSocket> &Clients, int MAXEVENTS);
	struct epoll_event* getEvents();
	void setFD(int FD,epoll_event _event);
	void removeFD(int FD);
	const struct epoll_event &getFD(int FD);
	size_t copyEventsTo(struct epoll_event *destination, size_t maxSize);
};

MockEpoll::MockEpoll(): monitoredFDs(), events() {}

void MockEpoll::fillEvents(std::map<int, MockSocket> &Clients, int MAXEVENTS) {
	events.clear();
	int i = 0;
	for(auto FD : monitoredFDs) {
		MockSocket client = Clients[FD.first];
		if(client.hasEvent() && FD.second.events) {
			++i;
			if(i > MAXEVENTS) {
				break;
			}
			events.push_back( client.getEvent() );
		}
	}
}

struct epoll_event* MockEpoll::getEvents() {
	return &events[0];
}


void MockEpoll::setFD(int FD, epoll_event _event) {
	monitoredFDs[FD] = _event;
}
const struct epoll_event &MockEpoll::getFD(int FD) {
	return monitoredFDs[FD];
}

void MockEpoll::removeFD(int FD) {
	monitoredFDs.erase(FD);
}

size_t MockEpoll::copyEventsTo(struct epoll_event *destination, size_t maxSize) {
	size_t copySize = events.size();
	if(copySize>maxSize) copySize = maxSize;
	memcpy(destination, &events[0], copySize);
	return events.size();
}














class MockSystemState
{
	std::map<int,MockEpoll>		epolls;
	std::map<int,MockSocket>	sockets;
	int nextAvailableFD;
public:
	MockSystemState();
	int getNewFD();
	int addEpoll();
	void removeEpoll(int FD);
	int addSocket(MockSocket &client);
	void removeSocket(int FD);
	void setReadBuffer(int FD, std::string buf);
	std::string getReadBuffer(int FD);
	std::string getWriteBuffer(int FD);
	void clearWriteBuffer(int FD);
	void setEvent(int epoll, int FD, struct epoll_event *event);
	void removeFD(int epoll, int FD);
	void addFD(int epoll, int FD, struct epoll_event *event);
	void setBytesTillWriteFail(int socket, ssize_t bytes);
	void setBytesTillReadFail(int socket, ssize_t bytes);
	size_t epollWait(int epollFD, struct epoll_event *_events, int maxSize);
	int getSocketFlags(int socket);
	void setSocketFlags(int socket, int flags);
	size_t writeToSocket(int socket, const char *buf, size_t count);
	size_t readFromSocket(int socket, char *buf, size_t count);
	void bindSocket(int socket, const struct sockaddr *addr, unsigned int addrlen);

	static MockSystemState &getState(bool reset = false) {
		static MockSystemState state = MockSystemState();
		if(reset) {
			state = MockSystemState();
		}
		return state;
	}
};


void MockSystemState::setEvent(int epoll, int FD, struct epoll_event *event) {
	epolls[epoll].setFD(FD, *event);
}

void MockSystemState::removeFD(int epoll, int FD) {
	epolls[epoll].removeFD(FD);
}

void MockSystemState::addFD(int epoll, int FD, struct epoll_event *event) {
	epolls[epoll].setFD(FD, *event);
}

void MockSystemState::setBytesTillWriteFail(int socket, ssize_t bytes) {
	sockets[socket].setBytesUntilWriteFail(bytes);
}
void MockSystemState::setBytesTillReadFail(int socket, ssize_t bytes) {
	sockets[socket].setBytesUntilReadFail(bytes);
}

int MockSystemState::getSocketFlags(int socket) {
	return sockets[socket].getFlags();
}

void MockSystemState::setSocketFlags(int socket, int flags) {
	sockets[socket].setFlags(flags);
}


size_t MockSystemState::writeToSocket(int socket, const char *buf, size_t count) {
	return sockets[socket].writeDataToWriteBuffer(buf, count);
}

size_t MockSystemState::readFromSocket(int socket, char *buf, size_t count) {
	return sockets[socket].readDataFromReadBuffer(buf, count) ;
}



void MockSystemState::bindSocket(int socket, const struct sockaddr *addr,
	                             unsigned int addrlen) {
	sockets[socket].bind(addr, addrlen);
}


size_t MockSystemState::epollWait(int epollFD,
	                            struct epoll_event *_events,
	                            int maxSize) {
	MockEpoll& epoll = epolls[epollFD];
	epoll.fillEvents(sockets, maxSize);
	return epoll.copyEventsTo(_events, static_cast<size_t>(maxSize));
}



int MockSystemState::getNewFD() {
	return ++nextAvailableFD;
}

int MockSystemState::addEpoll() {
	int FD = getNewFD();
	MockEpoll epoll;
	epolls[FD] = epoll;
	return FD;
}

void MockSystemState::removeEpoll(int FD) {
	epolls.erase(FD);
}

int MockSystemState::addSocket(MockSocket &client) {
	int FD = getNewFD();
	sockets[FD] = client;
	return FD;
}

void MockSystemState::removeSocket(int FD) {
	for(auto epoll : epolls){
		epoll.second.removeFD(FD);
	}
	sockets.erase(FD);
}

MockSystemState::MockSystemState(): epolls(), sockets(), nextAvailableFD(1) {

}

void MockSystemState::setReadBuffer(int FD, std::string buf) {
	sockets[FD].setSocketReadBuffer(std::move(buf));
}

std::string MockSystemState::getReadBuffer(int FD) {
	return sockets[FD].getSocketReadBuffer();
}

std::string MockSystemState::getWriteBuffer(int FD) {
	return sockets[FD].getSocketWriteBuffer();
}

void MockSystemState::clearWriteBuffer(int FD) {
	sockets[FD].setSocketWriteBuffer(std::move(std::string()));
}































MockSystemWrapperState::MockSystemWrapperState(): nullChar(0),
                        mockAddrinfo(new addrinfo()), mockAddrinfoPointer(NULL),
                        mockSockaddr(new sockaddr_storage()) {
	memset(mockAddrinfo.get(), 0, sizeof(addrinfo));
	memset(mockSockaddr.get(), 0, sizeof(sockaddr_storage));
	mockAddrinfo->ai_addr = reinterpret_cast<sockaddr*>(&mockSockaddr);
	mockAddrinfoPointer = mockAddrinfo.get();
}

MockSystemWrapperState& MockSystemWrapperState::operator=(MockSystemWrapperState&& old) {
	if( &old != this) {
		mockAddrinfo.reset(old.mockAddrinfo.release());
		mockAddrinfoPointer = mockAddrinfo.get();
		mockSockaddr.reset(old.mockSockaddr.release());
	}
	return *this;
}

MockSystemWrapperState::~MockSystemWrapperState(){}

void MockSystemWrapperState::setReadBuffer(int FD, std::string buf) {
	MockSystemState::getState().setReadBuffer(FD, buf);
}

std::string MockSystemWrapperState::getReadBuffer(int FD) {
	return MockSystemState::getState().getReadBuffer(FD);
}

void MockSystemWrapperState::clearWriteBuffer(int FD) {
	return MockSystemState::getState().clearWriteBuffer(FD);
}

std::string MockSystemWrapperState::getWriteBuffer(int FD) {
	return MockSystemState::getState().getWriteBuffer(FD);
}

void MockSystemWrapperState::setBytesTillWriteFail(int socket, ssize_t bytes) {
	MockSystemState::getState().setBytesTillWriteFail(socket, bytes);
}
void MockSystemWrapperState::setBytesTillReadFail(int socket, ssize_t bytes) {
	MockSystemState::getState().setBytesTillReadFail(socket, bytes);
}

void MockSystemWrapperState::resetState() {
	MockSystemWrapperState::getMockSystemInstance(true);
}

MockSystemWrapperState &MockSystemWrapperState::getMockSystemInstance(bool reset) {
	static MockSystemWrapperState wrapper;
	if(reset) {
		MockSystemState::getState(true);
		wrapper = std::move(MockSystemWrapperState());
	}
	return wrapper;
}






































static int epollControl(int epoll, int op, int FD, struct epoll_event *event) {
	//return epoll_ctl(epoll, op, FD, event);
	if(op == EPOLL_CTL_MOD){
		MockSystemState::getState().setEvent(epoll, FD, event);
	}
	if(op == EPOLL_CTL_DEL){
		MockSystemState::getState().removeFD(epoll, FD);
	}
	if(op == EPOLL_CTL_ADD){
		MockSystemState::getState().addFD(epoll, FD, event);
	}
	return 0;
}



//redefined base members using link seam
size_t epollWait(int epollFD,
	                            struct epoll_event *_events,
	                            int MAXEVENTS, int timeout) {
	//return epoll_wait(epollFD,  events, MAXEVENTS, timeout);
	//std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
	(void)timeout; // to clear unused warning
	return MockSystemState::getState().epollWait(epollFD, _events, MAXEVENTS);
}

bool epollControlAdd(int epoll, int FD,
	                                struct epoll_event *event) {
	epollControl(epoll, EPOLL_CTL_ADD, FD, event);
	return false;
}
void epollControlDelete(int epoll, int FD,
	                                   struct epoll_event *event) {
	epollControl(epoll, EPOLL_CTL_DEL, FD, event);
}
void epollControlMod(int epoll, int FD,
	                                struct epoll_event *event) {
	epollControl(epoll, EPOLL_CTL_MOD, FD, event);
}

int epollCreate(int flags) {
	//return epoll_create1(flags);
	++flags;//to clear unused warning
	return MockSystemState::getState().addEpoll();
}

int getFlags(int FD) {
	//return fcntl (FD,F_GETFL, 0);
	return MockSystemState::getState().getSocketFlags(FD);
}

void setFlags(int FD, int _flags) {
	//return fcntl (FD, F_SETFL, flags);
	MockSystemState::getState().setSocketFlags(FD, _flags);
}

void closeFD(int FD) {
	//return close(FD);
	MockSystemState::getState().removeSocket(FD);
}

size_t writeFD(int FD, const void *buf, size_t count) {
	//return write(FD, buf, count);
	return MockSystemState::getState().writeToSocket(FD, static_cast<const char*>(buf), count);
}

size_t readFD(int FD, void *buf, size_t count, bool &done) {
	done = true;
	//return read(FD, buf, count);
	return MockSystemState::getState().readFromSocket(FD, static_cast<char*>(buf), count);
}

void getNameInfo(const struct sockaddr *sa, unsigned int salen,
	                            char *host , unsigned int hostlen,  char *serv,
	                            unsigned int servlen, int flags) {
	//return getnameinfo(sa, salen, host , hostlen,  serv, servlen, flags);
	(void)sa;//to clear unused warning
	(void)salen;//to clear unused warning
	(void)flags;//to clear unused warning
	if(host!=NULL && hostlen >0) host[0] = '\0';
	if(serv!=NULL && servlen >0) serv[0] = '\0';
}

void getAddrInfo(const char *node, const char *service,
	                            const struct addrinfo *hints,
	                            struct addrinfo **res) {
	//return getaddrinfo(node, service, hints, res);
	(void)node;//to clear unused warning
	(void)service;//to clear unused warning
	(void)hints;//to clear unused warning

	memcpy(res, &MockSystemWrapperState::getMockSystemInstance().mockAddrinfoPointer, sizeof(addrinfo*));
}

void freeAddrInfo(struct addrinfo *res) {
	//freeaddrinfo(res);
	(void)res;//to clear unused warning
	return;
}

const char* gaiStrError(int errcode) {
	//return gai_strerror(errcode);
	(void)errcode;//to clear unused warning
	return &MockSystemWrapperState::getMockSystemInstance().nullChar;
}

int getSockOpt(int sockfd, int level,
	                          int optname, void *optval,
	                          unsigned int *optlen) {
	//return getsockopt(sockfd, level, optname, optval, optlen);
	(void)sockfd;//to clear unused warning
	(void)level;//to clear unused warning
	(void)optval;//to clear unused warning
	(void)optname;//to clear unused warning

	memset(optlen, 0, sizeof(int) );
	return 0;

}

char* strError(int errnum) {
	//return strerror(errnum);
	(void)errnum;//to clear unused warning
	return &MockSystemWrapperState::getMockSystemInstance().nullChar;
}

int createSocket(int domain, int type, int protocol) {
	//return socket(domain, type, protocol);

	MockSocket socket;
	socket.create(domain, type, protocol);
	return MockSystemState::getState().addSocket(socket);
}

void bindSocket(int sockfd,
	                           const struct sockaddr *addr,
	                           unsigned int addrlen) {
	//return bind(sockfd, addr, addrlen);
	MockSystemState::getState().bindSocket(sockfd, addr, addrlen);
}

void listenSocket(int sockfd, int backlog) {
	//return listen(sockfd, backlog);
	(void)sockfd;//to clear unused warning
	(void)backlog;//to clear unused warning
}

int acceptSocket(int sockfd, struct sockaddr *addr,
	                            unsigned int *addrlen, bool &done) {
	//return accept(sockfd, addr, addrlen);
	done = false;
	(void)sockfd;//to clear unused warning
	(void)addr;//to clear unused warning
	(void)addrlen;//to clear unused warning
	int ret = MockSystemState::getState().getNewFD();
	if (ret == -1) done = true;
	return ret;
}
