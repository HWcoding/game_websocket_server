#include "source/data_types/socket_message.h"
#include "source/message_handlers/message_dispatcher.h"
#include "source/data_types/message_type.h"

std::vector<SocketMessage> buildTestMessages();
std::vector<SocketMessage> buildTestMessages()
{
	std::vector<SocketMessage> messages(3);

	ByteArray move1;
	move1.append(1.2f);
	move1.append(4.6f);
	move1.append(4.1f);
	move1.append(0.6f);
	move1.append(0.2f);
	move1.append(0.1f);
	move1.append(0.0f);

	messages[0].setMessage(1, MessageType::move, 1,
				ByteArray(std::string("123.456.789")),
				ByteArray(std::string("5132")),
				ByteArray(std::string("DJSK765SJdKS")), move1);

	messages[1].setMessage(2, MessageType::ability, 1,
				ByteArray(std::string("123.456.789")),
				ByteArray(std::string("5132")),
				ByteArray(std::string("DJSK765SJdKS")),
				ByteArray(std::string("This is a Message")));

	messages[2].setMessage(2, MessageType::move, 1,
				ByteArray(std::string("123.456.789")),
				ByteArray(std::string("5132")),
				ByteArray(std::string("DJSK765SJdKS")), move1);

	return messages;
}



int main(){
	std::vector<SocketMessage> messages=buildTestMessages();
	MessageDispatcher dispatcher;
	dispatcher.dispatchMessages(messages);
	return 0;
}
