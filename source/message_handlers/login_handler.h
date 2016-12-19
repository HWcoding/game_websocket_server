#ifndef SOURCE_MESSAGE_HANDLERS_LOGIN_HANDLER_H
#define SOURCE_MESSAGE_HANDLERS_LOGIN_HANDLER_H
//#include "source/message_handlers/login_handler.h"

#include "source/message_handlers/listener.h"

class SocketMessage;
class LoginHandler : public Listener
{
public:
	void callback(SocketMessage &message);
};

#endif /* SOURCE_MESSAGE_HANDLERS_LOGIN_HANDLER_H */
