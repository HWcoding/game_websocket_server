#define TEST_FILE_LINK_DEPENDENCIES "source/message_handlers/message_dispatcher.cpp,\
                                     source/data_types/socket_message.cpp,\
                                     source/data_types/byte_array.cpp"

#include "source/message_handlers/message_dispatcher.h"
#include "source/message_handlers/listener.h"
#include "source/data_types/socket_message.h"
#include "source/data_types/message_type.h"
#include <vector>
#include <string>

#include "tests/test.h"

//stubs
class MoveHandler : public Listener
{
public:
	void callback( const SocketMessage &message);
	MoveHandler();
	~MoveHandler();
};
MoveHandler::MoveHandler(){}
MoveHandler::~MoveHandler(){}
void MoveHandler::callback( const SocketMessage &message){(void)message;}

class AbilityHandler : public Listener
{
public:
	void callback( const SocketMessage &message);
	AbilityHandler();
	~AbilityHandler();
};
AbilityHandler::AbilityHandler(){}
AbilityHandler::~AbilityHandler(){}
void AbilityHandler::callback( const SocketMessage &message){(void)message;}

class LogoutHandler : public Listener
{
public:
	void callback( const SocketMessage &message);
	LogoutHandler();
	~LogoutHandler();
};
LogoutHandler::LogoutHandler(){}
LogoutHandler::~LogoutHandler(){}
void LogoutHandler::callback( const SocketMessage &message){(void)message;}

class LoginHandler : public Listener
{
public:
	void callback( const SocketMessage &message);
	LoginHandler();
	~LoginHandler();
};
LoginHandler::LoginHandler(){}
LoginHandler::~LoginHandler(){}
void LoginHandler::callback( const SocketMessage &message){(void)message;}

class NullHandler : public Listener
{
public:
	void callback( const SocketMessage &message);
	NullHandler();
	~NullHandler();
};
NullHandler::NullHandler(){}
NullHandler::~NullHandler(){}
void NullHandler::callback( const SocketMessage &message){(void)message;}




// mocks
class TestListener : public Listener
{
	std::string exceptionName;

	void callback( const SocketMessage &message)
	{
		// throw an exception to let caller know
		// which function was called
		throw std::string(exceptionName);
		// silence unused warnings
		(void)message;
	}
public:
	TestListener(std::string name) : exceptionName(name){}
};


// redefined from message_dispatcher.cpp
struct MessageDispatcherListenersBase
{
	Listener *listeners[MessageType::NumberOfMessageTypes] {};
	virtual~MessageDispatcherListenersBase()=0;
};
MessageDispatcherListenersBase::~MessageDispatcherListenersBase(){}


struct MessageDispatcherTestListener : public MessageDispatcherListenersBase
{
	// MessageHandler::processMessages should call the callback method of
	// one of these objects. Which method gets called is based on what the
	// input message type is.

	TestListener moveHandler {"Move"};
	TestListener abillityHandler {"Ability"};
	TestListener loginHandler {"Login"};
	TestListener logoutHandler {"Logout"};
	TestListener nullHandler {"Null"};

	~MessageDispatcherTestListener(){}
	MessageDispatcherTestListener()
	{
		listeners[MessageType::null]    = &nullHandler;
		listeners[MessageType::move]    = &moveHandler;
		listeners[MessageType::ability] = &abillityHandler;
		listeners[MessageType::login]   = &loginHandler;
		listeners[MessageType::logout]  = &logoutHandler;
	}

	MessageDispatcherTestListener(const MessageDispatcherTestListener&) = delete;
	MessageDispatcherTestListener& operator=(const MessageDispatcherTestListener&) = delete;
};















SocketMessage createTestMessage(MessageType type)
{
	return SocketMessage(1, type, 1,
				ByteArray(),
				ByteArray(),
				ByteArray(),
				ByteArray());
}


void testCallback(MessageType type, std::string expectedResponse)
{
	// create a message of type "type" to send to MessageHandler
	std::vector<SocketMessage> messages = { createTestMessage(type) };

	try {
		//create a dispetcher that uses mock listeners
		MessageDispatcher dispatcher(new MessageDispatcherTestListener());

		//mock should throw a string exception with the callback object name
		dispatcher.dispatchMessages(messages);
	}
	catch(std::string calledFunction) {

		// check to see if exception matches epectected response
		EXPECT_STREQ(calledFunction.c_str(), expectedResponse.c_str());
		return;
	}
	ADD_FAILURE()<<"Message contained type that did not" \
	                        " map to a handler. Type is "<<messages[0].getType();


}


TEST(MessageHandlerTest, MoveMessageUsesMoveCallback)
{
	testCallback(MessageType::move, "Move");
}

TEST(MessageHandlerTest, NullMessageUsesNullCallback)
{
	testCallback(MessageType::null, "Null");
}

TEST(MessageHandlerTest, AbilityMessageUsesAbilityCallback)
{
	testCallback(MessageType::ability, "Ability");
}

TEST(MessageHandlerTest, LoginMessageUsesLoginCallback)
{
	testCallback(MessageType::login, "Login");
}

TEST(MessageHandlerTest, LogoutMessageUsesLogoutCallback)
{
	testCallback(MessageType::logout, "Logout");
}

TEST(MessageHandlerTest, InvalidMessageUsesNullCallback)
{
	testCallback(MessageType::NumberOfMessageTypes, "Null");
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	STAY_SILENT_ON_SUCCESS();
	return RUN_ALL_TESTS();
}
