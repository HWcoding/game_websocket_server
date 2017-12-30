#ifndef SOURCE_MESSAGE_HANDLERS_LISTENER_H
#define SOURCE_MESSAGE_HANDLERS_LISTENER_H
//#include "source/message_handlers/listener.h"

class SocketMessage;
class Listener
{
public:
	virtual void callback(const SocketMessage&) = 0;
	virtual ~Listener() = default;
};

#endif /* SOURCE_MESSAGE_HANDLERS_LISTENER_H */
