#ifndef SOURCE_MESSAGE_HANDLERS_ABILITY_HANDLER_H
#define SOURCE_MESSAGE_HANDLERS_ABILITY_HANDLER_H
//#include "source/message_handlers/ability_handler.h"

#include "source/message_handlers/listener.h"

class SocketMessage;

class AbilityHandler : public Listener
{
public:
	void callback( const SocketMessage &message) override;

};


#endif /* SOURCE_MESSAGE_HANDLERS_ABILITY_HANDLER_H */
