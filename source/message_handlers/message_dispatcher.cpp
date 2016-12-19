#include "source/message_handlers/message_dispatcher.h"
#include "source/message_handlers/null_handler.h"
#include "source/message_handlers/move_handler.h"
#include "source/message_handlers/ability_handler.h"
#include "source/message_handlers/login_handler.h"
#include "source/message_handlers/logout_handler.h"
#include "source/data_types/socket_message.h"
#include "source/data_types/message_type.h"


struct MessageDispatcherListenersBase
{
	Listener *listeners[MessageType::NumberOfMessageTypes] {};
	virtual~MessageDispatcherListenersBase()=0;
};
MessageDispatcherListenersBase::~MessageDispatcherListenersBase(){}



struct MessageDispatcherListeners : public MessageDispatcherListenersBase
{
	MoveHandler moveHandler {};
	AbilityHandler abillityHandler {};
	LoginHandler loginHandler {};
	LogoutHandler logoutHandler {};
	NullHandler nullHandler {};
	~MessageDispatcherListeners(){}
	MessageDispatcherListeners(){
		listeners[MessageType::null]    = &nullHandler;
		listeners[MessageType::move]    = &moveHandler;
		listeners[MessageType::ability] = &abillityHandler;
		listeners[MessageType::login]   = &loginHandler;
		listeners[MessageType::logout]  = &logoutHandler;
	}

	MessageDispatcherListeners(const MessageDispatcherListeners&) = delete;
	MessageDispatcherListeners& operator=(const MessageDispatcherListeners&) = delete;
};


MessageDispatcher::MessageDispatcher() : d(new MessageDispatcherListeners()){}
MessageDispatcher::~MessageDispatcher(){
	delete d;
}

MessageDispatcher::MessageDispatcher(MessageDispatcherListenersBase *base) : d(base) {}

void MessageDispatcher::dispatchMessages(std::vector<SocketMessage> &messages)
{
	for(size_t i=0; i<messages.size(); ++i){
		// call the correct function based on the message type
		uint32_t type =messages[i].getType();
		if(type<MessageType::NumberOfMessageTypes)
			d->listeners[type]->callback(messages[i]);
		else
			d->listeners[MessageType::null]->callback(messages[i]);
	}
}
