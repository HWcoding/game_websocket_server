#include "source/message_handlers/logout_handler.h"
#include "source/data_types/socket_message.h"
#include <iostream>

void LogoutHandler::callback( const SocketMessage &message){
	std::cout<<"used logout"<<std::endl;
	while(false){ (void)message; }
	return;
}
