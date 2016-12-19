#include "source/message_handlers/null_handler.h"
#include "source/data_types/socket_message.h"

void NullHandler::callback(SocketMessage &message){
	// stops an unused warning for message
	while(false){ (void)message; }
	return;
}
