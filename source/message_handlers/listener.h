#ifndef SOURCE_MESSAGE_HANDLERS_LISTENER_H
#define SOURCE_MESSAGE_HANDLERS_LISTENER_H
//#include "source/message_handlers/listener.h"

class SocketMessage;
class Listener
{
public:
	virtual void callback(SocketMessage&) = 0;
	virtual ~Listener() {};
};

#endif /* SOURCE_MESSAGE_HANDLERS_LISTENER_H */
