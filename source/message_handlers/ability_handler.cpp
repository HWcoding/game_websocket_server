#include "source/message_handlers/ability_handler.h"
#include "source/data_types/socket_message.h"
#include <iostream>

void AbilityHandler::callback(SocketMessage &message){
	std::cout<<"used ability"<<std::endl;
	while(false){ (void)message; }
	return;
}
