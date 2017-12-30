#ifndef SOURCE_MESSAGE_HANDLERS_MESSAGE_DISPATCHER_LISTENERS_BASE_H
#define SOURCE_MESSAGE_HANDLERS_MESSAGE_DISPATCHER_LISTENERS_BASE_H
//#include "source/message_handlers/message_dispatcher_listeners_base.h"


#include "source/message_handlers/null_handler.h"
#include "source/message_handlers/move_handler.h"
#include "source/message_handlers/ability_handler.h"
#include "source/message_handlers/login_handler.h"
#include "source/message_handlers/logout_handler.h"
#include "source/data_types/message_type.h"

class MessageDispatcherListenersBase
{
public:
	Listener* listeners[MessageType::NumberOfMessageTypes] {};
	virtual~MessageDispatcherListenersBase()=0;
};

#endif /* SOURCE_MESSAGE_HANDLERS_MESSAGE_DISPATCHER_LISTENERS_BASE_H */