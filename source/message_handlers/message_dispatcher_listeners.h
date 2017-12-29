#ifndef SOURCE_MESSAGE_HANDLERS_MESSAGE_DISPATCHER_LISTENERS_H
#define SOURCE_MESSAGE_HANDLERS_MESSAGE_DISPATCHER_LISTENERS_H
//#include "source/message_handlers/message_dispatcher_listeners.h"

#include "source/message_handlers/null_handler.h"
#include "source/message_handlers/move_handler.h"
#include "source/message_handlers/ability_handler.h"
#include "source/message_handlers/login_handler.h"
#include "source/message_handlers/logout_handler.h"
#include "source/data_types/message_type.h"
#include "source/message_handlers/message_dispatcher_listeners_base.h"


class MessageDispatcherListeners : public MessageDispatcherListenersBase
{
private:
	MoveHandler moveHandler {};
	AbilityHandler abillityHandler {};
	LoginHandler loginHandler {};
	LogoutHandler logoutHandler {};
	NullHandler nullHandler {};
public:
	~MessageDispatcherListeners() override = default;
	MessageDispatcherListeners() {
		listeners[MessageType::null]    = &nullHandler;
		listeners[MessageType::move]    = &moveHandler;
		listeners[MessageType::ability] = &abillityHandler;
		listeners[MessageType::login]   = &loginHandler;
		listeners[MessageType::logout]  = &logoutHandler;
	}

	//dispatchMessage()

	MessageDispatcherListeners(const MessageDispatcherListeners&) = delete;
	MessageDispatcherListeners& operator=(const MessageDispatcherListeners&) = delete;
};

#endif /* SOURCE_MESSAGE_HANDLERS_MESSAGE_DISPATCHER_LISTENERS_H */
