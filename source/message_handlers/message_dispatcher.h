#ifndef SOURCE_MESSAGE_HANDLERS_MESSAGE_DISPATCHER_H
#define SOURCE_MESSAGE_HANDLERS_MESSAGE_DISPATCHER_H
//#include "source/message_handlers/message_dispatcher.h"

#include <vector>

class SocketMessage;
class MessageDispatcherListenersBase;

class MessageDispatcher
{
public:
	MessageDispatcher(const MessageDispatcher&) = delete;
	MessageDispatcher& operator=(const MessageDispatcher&) = delete;
	MessageDispatcher();
	~MessageDispatcher();

	void dispatchMessages(std::vector<SocketMessage> &messages);



	explicit MessageDispatcher(MessageDispatcherListenersBase*); //for testing
private:
	MessageDispatcherListenersBase *d{nullptr};
};

#endif /* SOURCE_MESSAGE_HANDLERS_MESSAGE_DISPATCHER_H */
