#include "source/message_handlers/move_handler.h"
#include "source/data_types/socket_message.h"
#include "source/data_types/mapped_vector.h"
#include <iostream>

struct PositionData
{
	float posX{0.0};
	float posY{0.0};
	float posZ{0.0};
	float velX{0.0};
	float velY{0.0};
	float velZ{0.0};
	float rot{0.0};

	float getSquareDistanceTo(const PositionData &destination) const
	{
		float deltaX = posX-destination.posX;
		float deltaY = posY-destination.posY;
		float deltaZ = posZ-destination.posZ;
		return deltaX*deltaX +\
		       deltaY*deltaY +\
		       deltaZ*deltaZ;
	}
};

struct MoveMessage
{
	PositionData pos{};

	MoveMessage(){}

	explicit MoveMessage(SocketMessage &message)
	{
		setStateFromMessage(message);
	}

	void setStateFromMessage(SocketMessage &message)
	{
		pos.posX = message.getNextFloat();
		pos.posY = message.getNextFloat();
		pos.posZ = message.getNextFloat();
		pos.velX = message.getNextFloat();
		pos.velY = message.getNextFloat();
		pos.velZ = message.getNextFloat();
		pos.rot  = message.getNextFloat();
	}

	void addStateToMessage(SocketMessage &message)
	{
		ByteArray array;
		array.append(pos.posX);
		array.append(pos.posY);
		array.append(pos.posZ);
		array.append(pos.velX);
		array.append(pos.velY);
		array.append(pos.velZ);
		array.append(pos.rot);
		message.setMessage(array);
	}


	bool validate()
	{
		return true;
	}
};

struct MoveHandler::MoveHandlerData
{
	MappedVector<int,PositionData> positions{};
	void setPosition(int playerID, const MoveMessage& message)
	{
		PositionData &player = positions[playerID];
		if(isCheating(player, message.pos)){
			//TODO: placeholder
		}
		player = message.pos;
	}
	bool isCheating(const PositionData &current, const PositionData &proposed) const
	{
		float delta = current.getSquareDistanceTo(proposed);
		return delta>100;
	}
};



void MoveHandler::callback(SocketMessage &message)
{
	int playerID = message.getFD();
	d->setPosition(playerID, MoveMessage(message));
	std::cout<<d->positions[playerID].posX<<", "<<d->positions[playerID].posY<<std::endl;
	return;
}





//constructor and destructor
MoveHandler::MoveHandler():d(new MoveHandlerData()) {}

MoveHandler::~MoveHandler()
{
	delete d;
}
