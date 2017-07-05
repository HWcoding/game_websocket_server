#include "source/message_handlers/message_dispatcher.h"
#include "source/data_types/socket_message.h"
#include "source/data_types/message_type.h"
#include "source/message_handlers/message_dispatcher_listeners.h"

MessageDispatcher::MessageDispatcher() : d(new MessageDispatcherListeners()){}
MessageDispatcher::~MessageDispatcher()
{
	delete d;
}

MessageDispatcher::MessageDispatcher(MessageDispatcherListenersBase *base) : d(base) {}

void MessageDispatcher::dispatchMessages(const std::vector<SocketMessage> &messages)
{
	for(auto message : messages){
		// call the correct function based on the message type
		uint32_t type = message.getType();
		if(type<MessageType::NumberOfMessageTypes)
			d->listeners[type]->callback(message);
		else
			d->listeners[MessageType::null]->callback(message);
	}
}
#include <iostream>
void MessageDispatcher::dispatchMessage(const SocketMessage &message)
{

	// call the correct function based on the message type
	uint32_t type =message.getType();
	if(type<MessageType::NumberOfMessageTypes) {
		d->listeners[type]->callback(message);
		//std::cout<<"message type "<<type<<std::endl;
	}
	else
		d->listeners[MessageType::null]->callback(message);

}
