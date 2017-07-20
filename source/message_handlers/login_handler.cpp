#include "source/message_handlers/login_handler.h"
#include "source/data_types/socket_message.h"
#include <iostream>

void LoginHandler::callback(  const SocketMessage &message){
	std::cout<<"used login"<<std::endl;
	(void)message;
	return;
}
