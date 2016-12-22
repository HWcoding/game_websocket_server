#ifndef SOURCE_MESSAGE_HANDLERS_MOVE_HANDLER_H
#define SOURCE_MESSAGE_HANDLERS_MOVE_HANDLER_H
//#include "source/message_handlers/move_handler.h"

#include "source/message_handlers/listener.h"

class SocketMessage;
class MoveHandler : public Listener
{
private:
	struct MoveHandlerData;
	MoveHandlerData *d{nullptr};
public:
	MoveHandler(const MoveHandler&) = delete;
	MoveHandler& operator=(const MoveHandler&) = delete;

	MoveHandler();
	~MoveHandler();
	void callback( const SocketMessage &message);
};

#endif /* SOURCE_MESSAGE_HANDLERS_MOVE_HANDLER_H */
