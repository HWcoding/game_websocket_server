#ifndef SOURCE_MESSAGE_HANDLERS_LOGOUT_HANDLER_H
#define SOURCE_MESSAGE_HANDLERS_LOGOUT_HANDLER_H
//#include "source/message_handlers/logout_handler.h"

#include "source/message_handlers/listener.h"

class SocketMessage;
class LogoutHandler : public Listener
{
public:
	void callback(SocketMessage &message);
};

#endif /* SOURCE_MESSAGE_HANDLERS_LOGOUT_HANDLER_H */
