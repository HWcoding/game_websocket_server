#include "source/data_types/socket_message.h"

bool SocketMessage::isNotEmpty()  const
{
	return !(message.empty() || FD == -1);
}

std::chrono::system_clock::time_point SocketMessage::calculateTime()
{
	return std::chrono::system_clock::now();
}


void SocketMessage::setMessage(int _FD, uint8_t _opcode, uint32_t _type, uint32_t _priority, const ByteArray & _IP,
						const ByteArray & _port, const ByteArray & _CSRFkey,
						const ByteArray & _message)
{
	FD = _FD;
	opcode = _opcode;
	type = _type;
	priority =_priority;
	IP = _IP;
	port = _port;
	CSRFkey = _CSRFkey;
	message = _message;
	calculateTime();
}

void SocketMessage::setMessage(	const ByteArray & _message)
{
	message = _message;
}

// constructors
SocketMessage::SocketMessage() noexcept :
	sentTime(calculateTime())
{}

SocketMessage::SocketMessage(int _FD, uint8_t _opcode, uint32_t _type, uint32_t _priority, const ByteArray _IP,
						const ByteArray _port, const ByteArray _CSRFkey,
						const ByteArray _message) noexcept :
	FD(_FD),
	opcode(_opcode),
	type(_type),
	priority(_priority),
	sentTime(calculateTime()),

	IP(std::move(_IP)),
	port(std::move(_port)),
	CSRFkey(std::move(_CSRFkey)),
	message(std::move(_message))
{}


SocketMessage::SocketMessage(SocketMessage&& m) noexcept : //move constructor
	FD(m.FD),
	opcode(m.opcode),
	type(m.type),
	priority(m.priority),
	sentTime(std::move(m.sentTime)),
	IP(std::move(m.IP)),
	port(std::move(m.port)),
	CSRFkey(std::move(m.CSRFkey)),
	message(std::move(m.message))
{
	m.FD = -1;
	m.type = 0;
}

SocketMessage& SocketMessage::operator=(const SocketMessage &m) noexcept  //copy assignment
{
	if (this != &m){
		FD = m.FD;
		opcode = m.opcode;
		type = m.type;
		priority = m.priority;
		sentTime = m.sentTime;
		IP = m.IP;
		port = m.port;
		CSRFkey = m.CSRFkey;
		message = m.message;
	}
	return *this;
}
