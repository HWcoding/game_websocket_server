#ifndef SERVER_SOCKET_SOCKET
#define SERVER_SOCKET_SOCKET
//#include "source/server/socket/socket.h"

#include <thread>
#include <string>
#include <memory>
#include <atomic>

class SocketMessage;
class SetOfFileDescriptors;
class SocketReader;
class SocketWriter;
class SocketServerConnector;
class ClientValidatorInterface;

class SocketInterface
{
public:
	virtual SocketMessage getNextMessage() = 0;
	virtual void setClientValidator(ClientValidatorInterface * validator) = 0;
	virtual void sendMessage(SocketMessage &message) = 0;
	virtual void disconnectClient(int FD) = 0;
	virtual void shutdown() = 0;
	virtual bool isRunning() = 0;
	virtual ~SocketInterface() = default;
protected:
	SocketInterface() = default;
};

struct ServerConfig
{
	/** the port to connect the websocket on **/
	std::string port {std::string("443")};
	int64_t loopSpeed {100};
	/** the maximum number of messages to read from the kernel on each epoll **/
	int maxEvents {200};
	/** the size of the buffer used to store messages from a client **/
	size_t MaxReaderSocketBufferSize {32760};
	/** the size of the buffer used to store messages waiting to be sent to a client */
	size_t MaxWebsocketReadBufferSize {262144};
	/** the number of milliseconds that the server threads should hang during epoll calls if no messages are present */
	int maxWaitTime {1000};
	/** the maximum allowable size of a handshake message from a client */
	size_t maxHandshakeSize {2048};
	ServerConfig() = default;
};

/**
 * Class starts new threads to handle socket io and cleans up on destruction
 */
class Socket : public SocketInterface
{

public:
	Socket(const ServerConfig &config, std::atomic<bool> * _shouldContinueRunning);
	SocketMessage getNextMessage() override;
	void setClientValidator(ClientValidatorInterface * validator) override;

	void sendMessage(SocketMessage &message) override;
	void disconnectClient(int FD) override;
	void shutdown() final;

	bool isRunning() override;
	~Socket() override;

private:
	void startReader();
	void startWriter();
	void startConnector();

	Socket& operator=(const Socket&) = delete;
	Socket(const Socket&) = delete;

	std::atomic<bool> * shouldContinueRunning; //flag to tell the thread to end looping and exit
	std::unique_ptr<SetOfFileDescriptors> FDs;
	std::unique_ptr<SocketReader> reader;
	std::unique_ptr<SocketWriter> writer;
	std::unique_ptr<SocketServerConnector> connector;
	std::thread readerThread;
	std::thread writerThread;
	std::thread connectorThread;
};

#endif /* SERVER_SOCKET_SOCKET */
