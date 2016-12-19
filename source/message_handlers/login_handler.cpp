#include "source/message_handlers/login_handler.h"
#include "source/data_types/socket_message.h"
#include <iostream>

void LoginHandler::callback(SocketMessage &message){
	std::cout<<"used login"<<std::endl;
	while(false){ (void)message; }
	return;
}
