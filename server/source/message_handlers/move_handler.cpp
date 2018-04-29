#include "source/message_handlers/move_handler.h"
#include "source/data_types/socket_message.h"
#include "source/data_types/mapped_vector.h"
#include "source/math/geometric_math.h"
#include <iostream>
#include "source/logging/exception_handler.h"

struct PositionData
{
	Point3D pos {0.0, 0.0, 0.0};
	Point3D vel {0.0, 0.0, 0.0};
	Point3D rot {0.0, 0.0, 0.0};

	void loadStateFromMessage( const SocketMessage &message)
	{
		pos.x = message.getNextDouble();
		pos.y = message.getNextDouble();
		pos.z = message.getNextDouble();
		//vel.x = message.getNextDouble();
		//vel.y = message.getNextDouble();
		//vel.z = message.getNextDouble();
		//rot.x = message.getNextDouble();
		//rot.y = message.getNextDouble();
		//rot.z = message.getNextDouble();

		if(isValid() == false){
			throw std::runtime_error(LOG_EXCEPTION("Position Data was invalid"));
		}
	}

	void saveStateToMessage(SocketMessage &message)
	{
		ByteArray array;

		array.append(pos.x);
		array.append(pos.y);
		array.append(pos.z);
		array.append(vel.x);
		array.append(vel.y);
		array.append(vel.z);
		array.append(rot.x);
		array.append(rot.y);
		array.append(rot.z);

		message.setMessage(array);
	}

	bool isValid()
	{
		//TODO: check if position is in a valid state
		return true;
	}
};

struct MoveMessage
{
	PositionData pos{};

	MoveMessage() = default;

	explicit MoveMessage( const SocketMessage &message)
	{
		loadStateFromMessage(message);
	}

	void loadStateFromMessage( const SocketMessage &message)
	{
		pos.loadStateFromMessage(message);
	}

	void saveStateToMessage(SocketMessage &message)
	{
		pos.saveStateToMessage(message);
	}

	bool isValid()
	{
		return pos.isValid();
	}
};

class MoveHandler::MoveHandlerData
{
public:
	MappedVector<int,PositionData> positions{};
	void setPosition(int playerID, const MoveMessage& message)
	{
		PositionData &playerPosition = positions[playerID];
		if(isCheating(playerPosition, message.pos)){
			//TODO: placeholder
		}
		playerPosition = message.pos;
	}
	bool isCheating(const PositionData &current, const PositionData &proposed) const
	{
		//TODO: do something intellegent here.
		double delta = v_math::squareDistance(current.pos,proposed.pos);
		return delta>100;
	}
};



void MoveHandler::callback( const SocketMessage &message)
{
	int playerID = message.getFD();
	MoveMessage mv(message);

	d->setPosition(playerID, mv);
	PositionData &playerPosition = d->positions[playerID];
	std::cout << playerPosition.pos.x << ", " << playerPosition.pos.y << std::endl;
	std::cout << "moved" << std::endl;
	return;
}





//constructor and destructor
MoveHandler::MoveHandler():d(new MoveHandlerData()) {}

MoveHandler::~MoveHandler()
{
	delete d;
}
