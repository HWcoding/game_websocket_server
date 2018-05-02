
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


class MockSocket {
public:
	//struct addrinfo address;

	bool hasEvent;
	struct epoll_event event;

	std::string	socketReadBuffer;
	std::string	socketWriteBuffer;

	int flags;
	int family;
	int socktype;
	int protocol;
	struct sockaddr_storage address;

	int BytesUntilWriteFail; //number of bytes to write before simulating a full buffer. A negative value turns off the feature. -1 by default
	int BytesUntilReadFail; //number of bytes to read before simulating an empty buffer. A negative value turns off the feature. -1 by default

	MockSocket();
	void create(int _family, int _socktype, int _protocol);
	int bind(const struct sockaddr *ai_addr, unsigned int ai_addrlen);
	size_t readDataFromReadBuffer(char* buffer, size_t amount);
	size_t writeDataToWriteBuffer(const char* buffer, size_t amount);
};

MockSocket::MockSocket() : hasEvent(false),  event(), socketReadBuffer(),
                           socketWriteBuffer(), flags(0), family(0), socktype(0),
                           protocol(0), address(), BytesUntilWriteFail(-1),
                           BytesUntilReadFail(-1) {
	memset(&address,0, sizeof(address));
	event.data.fd = -1;
	event.events = 0;
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
	if(BytesUntilReadFail >= 0 && static_cast<size_t>(BytesUntilReadFail) < amount) {
		amount = static_cast<size_t>(BytesUntilReadFail);
	}
	//if the amount to read is more than is left in the readBuffer, set it ot the amount left in the read buffer
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
	if(BytesUntilWriteFail >= 0 && static_cast<size_t>(BytesUntilWriteFail) < amount) {
		amount = static_cast<size_t>(BytesUntilWriteFail);
	}
	socketWriteBuffer.append(buffer, amount);
	return amount;
}





class MockEpoll {
public:
	std::map<int,struct epoll_event> monitoredFDs;
	std::vector<struct epoll_event> events;

	MockEpoll();
	void fillEvents(std::map<int,MockSocket> &Clients, int MAXEVENTS);
	struct epoll_event* getEvents();
	void addFD(int FD,epoll_event _event);
	void removeFD(int FD);


};

MockEpoll::MockEpoll(): monitoredFDs(), events() {}

void MockEpoll::fillEvents(std::map<int, MockSocket> &Clients, int MAXEVENTS) {
	events.clear();
	int i = 0;
	for(auto FD : monitoredFDs) {
		MockSocket client = Clients[FD.first];
		if(client.hasEvent && (client.event.events & FD.second.events) ) {
			++i;
			if(i > MAXEVENTS) {
				break;
			}
			events.push_back( client.event );
		}
	}
}

struct epoll_event* MockEpoll::getEvents() {
	return &events[0];
}


void MockEpoll::addFD(int FD, epoll_event _event) {
	monitoredFDs[FD] = _event;
}

void MockEpoll::removeFD(int FD) {
	monitoredFDs.erase(FD);
}






class MockSystemState {
public:
	std::map<int,MockEpoll>		epolls;
	std::map<int,MockSocket>	sockets;
	int nextAvailableFD;

	MockSystemState();
	int getNewFD();
	int addEpoll();
	void removeEpoll(int FD);
	int addSocket(MockSocket &client);
	void removeSocket(int FD);
	void SetReadBuffer(int FD, std::string buf);
	std::string GetReadBuffer(int FD);
	std::string GetWriteBuffer(int FD);
	void ClearWriteBuffer(int FD);

	static MockSystemState &getState(bool reset = false) {
		static MockSystemState state = MockSystemState();
		if(reset) {
			state = MockSystemState();
		}
		return state;
	}
};





int MockSystemState::getNewFD() {
	++nextAvailableFD;
	return nextAvailableFD;
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

void MockSystemState::SetReadBuffer(int FD, std::string buf) {
	sockets[FD].socketReadBuffer = buf;
}

std::string MockSystemState::GetReadBuffer(int FD) {
	return sockets[FD].socketReadBuffer;
}

std::string MockSystemState::GetWriteBuffer(int FD) {
	return sockets[FD].socketWriteBuffer;
}

void MockSystemState::ClearWriteBuffer(int FD) {
	sockets[FD].socketWriteBuffer = std::string();
}







MockSystemWrapper::MockSystemWrapper(): SystemWrapper(), nullChar(0),
                        mockAddrinfo(new addrinfo()), mockAddrinfoPointer(NULL),
                        mockSockaddr(new sockaddr_storage()) {
	memset(mockAddrinfo.get(), 0, sizeof(addrinfo));
	memset(mockSockaddr.get(), 0, sizeof(sockaddr_storage));
	mockAddrinfo->ai_addr = reinterpret_cast<sockaddr*>(&mockSockaddr);
	mockAddrinfoPointer = mockAddrinfo.get();
}

MockSystemWrapper& MockSystemWrapper::operator=(MockSystemWrapper&& old) {
	if( &old != this) {
		mockAddrinfo.reset(old.mockAddrinfo.release());
		mockAddrinfoPointer = mockAddrinfo.get();
		mockSockaddr.reset(old.mockSockaddr.release());
	}
	return *this;
}

MockSystemWrapper::~MockSystemWrapper(){}

void MockSystemWrapper::SetReadBuffer(int FD, std::string buf) {
	MockSystemState::getState().SetReadBuffer(FD, buf);
}

std::string MockSystemWrapper::GetReadBuffer(int FD) {
	return MockSystemState::getState().GetReadBuffer(FD);
}

void MockSystemWrapper::ClearWriteBuffer(int FD) {
	return MockSystemState::getState().ClearWriteBuffer(FD);
}

std::string MockSystemWrapper::GetWriteBuffer(int FD) {
	return MockSystemState::getState().GetWriteBuffer(FD);
}

void MockSystemWrapper::SetBytesTillWriteFail(int socket, int bytes) {
	MockSystemState::getState().sockets[socket].BytesUntilWriteFail = bytes;
}

static int epollControl(int epoll, int op, int FD, struct epoll_event *event) {
	//return epoll_ctl(epoll, op, FD, event);
	if(op == EPOLL_CTL_MOD){
		MockSystemState::getState().epolls[epoll].monitoredFDs[FD] = *event;

	}
	if(op == EPOLL_CTL_DEL){
		MockSystemState::getState().epolls[epoll].removeFD(FD);
	}
	if(op == EPOLL_CTL_ADD){
		MockSystemState::getState().epolls[epoll].addFD(FD, *event);
	}
	return 0;
}

void MockSystemWrapper::resetState() {
	MockSystemWrapper::getMockSystemInstance(true);
}

MockSystemWrapper &MockSystemWrapper::getMockSystemInstance(bool reset) {
	static MockSystemWrapper wrapper;
	if(reset) {
		MockSystemState::getState(true);
		wrapper = std::move(MockSystemWrapper());
	}
	return wrapper;
}


//redefined base members using compile seam
size_t SystemWrapper::epollWait(int epollFD,
	                            struct epoll_event *_events,
	                            int MAXEVENTS, int timeout) const {
	//return epoll_wait(epollFD,  events, MAXEVENTS, timeout);
	//std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
	(void)timeout; // to clear unused warning
	MockEpoll& epoll = MockSystemState::getState().epolls[epollFD];
	epoll.fillEvents(MockSystemState::getState().sockets, MAXEVENTS);
	int copySize = static_cast<int>(epoll.events.size() );
	if(copySize>MAXEVENTS) copySize = MAXEVENTS;
	memcpy(_events, epoll.getEvents(), static_cast<size_t>(copySize));
	return epoll.events.size();
}

bool SystemWrapper::epollControlAdd(int epoll, int FD,
	                                struct epoll_event *event) const {
	epollControl(epoll, EPOLL_CTL_ADD, FD, event);
	return false;
}
void SystemWrapper::epollControlDelete(int epoll, int FD,
	                                   struct epoll_event *event) const {
	epollControl(epoll, EPOLL_CTL_DEL, FD, event);
}
void SystemWrapper::epollControlMod(int epoll, int FD,
	                                struct epoll_event *event) const {
	epollControl(epoll, EPOLL_CTL_MOD, FD, event);
}

int SystemWrapper::epollCreate(int flags) const {
	//return epoll_create1(flags);
	++flags;//to clear unused warning
	return MockSystemState::getState().addEpoll();
}

int SystemWrapper::getFlags(int FD) const {
	//return fcntl (FD,F_GETFL, 0);
	return MockSystemState::getState().sockets[FD].flags;
}

void SystemWrapper::setFlags(int FD, int _flags) const {
	//return fcntl (FD, F_SETFL, flags);
	MockSystemState::getState().sockets[FD].flags = _flags;
}

void SystemWrapper::closeFD(int FD) const {
	//return close(FD);
	MockSystemState::getState().removeSocket(FD);
}

size_t SystemWrapper::writeFD(int FD, const void *buf, size_t count) const {
	//return write(FD, buf, count);
	return MockSystemState::getState().sockets[FD].writeDataToWriteBuffer(static_cast<const char*>(buf), count);
}

size_t SystemWrapper::readFD(int FD, void *buf, size_t count, bool &done) const {
	done = true;
	//return read(FD, buf, count);
	return MockSystemState::getState().sockets[FD].readDataFromReadBuffer(static_cast<char*>(buf), count) ;
}

void SystemWrapper::getNameInfo(const struct sockaddr *sa, unsigned int salen,
	                            char *host , unsigned int hostlen,  char *serv,
	                            unsigned int servlen, int flags) const {
	//return getnameinfo(sa, salen, host , hostlen,  serv, servlen, flags);
	(void)sa;//to clear unused warning
	(void)salen;//to clear unused warning
	(void)flags;//to clear unused warning
	if(host!=NULL && hostlen >0) host[0] = '\0';
	if(serv!=NULL && servlen >0) serv[0] = '\0';
}

void SystemWrapper::getAddrInfo(const char *node, const char *service,
	                            const struct addrinfo *hints,
	                            struct addrinfo **res) const {
	//return getaddrinfo(node, service, hints, res);
	(void)node;//to clear unused warning
	(void)service;//to clear unused warning
	(void)hints;//to clear unused warning

	memcpy(res, &MockSystemWrapper::getMockSystemInstance().mockAddrinfoPointer, sizeof(addrinfo*));
}

void SystemWrapper::freeAddrInfo(struct addrinfo *res) const {
	//freeaddrinfo(res);
	(void)res;//to clear unused warning
	return;
}

const char* SystemWrapper::gaiStrError(int errcode) const {
	//return gai_strerror(errcode);
	(void)errcode;//to clear unused warning
	return &MockSystemWrapper::getMockSystemInstance().nullChar;
}

int SystemWrapper::getSockOpt(int sockfd, int level,
	                          int optname, void *optval,
	                          unsigned int *optlen) const {
	//return getsockopt(sockfd, level, optname, optval, optlen);
	(void)sockfd;//to clear unused warning
	(void)level;//to clear unused warning
	(void)optval;//to clear unused warning
	(void)optname;//to clear unused warning

	memset(optlen, 0, sizeof(int) );
	return 0;

}

char* SystemWrapper::strError(int errnum) const {
	//return strerror(errnum);
	(void)errnum;//to clear unused warning
	return &MockSystemWrapper::getMockSystemInstance().nullChar;
}

int SystemWrapper::createSocket(int domain, int type, int protocol) const {
	//return socket(domain, type, protocol);

	MockSocket socket;
	socket.create(domain, type, protocol);
	return MockSystemState::getState().addSocket(socket);
}

void SystemWrapper::bindSocket(int sockfd,
	                           const struct sockaddr *addr,
	                           unsigned int addrlen) const {
	//return bind(sockfd, addr, addrlen);
	MockSystemState::getState().sockets[sockfd].bind(addr, addrlen);
}

void SystemWrapper::listenSocket(int sockfd, int backlog) const {
	//return listen(sockfd, backlog);
	(void)sockfd;//to clear unused warning
	(void)backlog;//to clear unused warning
}

int SystemWrapper::acceptSocket(int sockfd, struct sockaddr *addr,
	                            unsigned int *addrlen, bool &done) const {
	//return accept(sockfd, addr, addrlen);
	done = false;
	(void)sockfd;//to clear unused warning
	(void)addr;//to clear unused warning
	(void)addrlen;//to clear unused warning
	int ret = MockSystemState::getState().getNewFD();
	if (ret == -1) done = true;
	return ret;
}

SystemWrapper &SystemWrapper::getSystemInstance() {
	return dynamic_cast<SystemWrapper&>(MockSystemWrapper::getMockSystemInstance());
}
