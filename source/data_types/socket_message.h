#ifndef SOURCE_DATA_TYPES_SOCKET_MESSAGE_H
#define SOURCE_DATA_TYPES_SOCKET_MESSAGE_H
//#include "source/data_types/socket_message.h"

#include <chrono>
#include <stdint.h>
#include "source/data_types/byte_array.h"

class SocketMessage{
public:
	void setMessage(int _FD, uint32_t _type, uint32_t _priority, const ByteArray & _IP,
							const ByteArray & _port, const ByteArray & _CSRFkey,
							const ByteArray & _message);

	void setMessage(const ByteArray & _message);

	bool isNotEmpty();

	int getFD()
	{
		return FD;
	}

	uint32_t getType()
	{
		return type;
	}

	uint32_t getPriority()
	{
		return priority;
	}

	std::chrono::system_clock::time_point getSentTime()
	{
		return sentTime;
	}

	ByteArray getIP()
	{
		return IP;
	}

	ByteArray getPort()
	{
		return port;
	}

	ByteArray getCSRFkey()
	{
		return CSRFkey;
	}

	int8_t getNextInt8()
	{
		return message.getNextInt8();
	}

	int16_t getNextInt16()
	{
		return message.getNextInt16();
	}

	int32_t getNextInt32()
	{
		return message.getNextInt32();
	}

	int64_t getNextInt64()
	{
		return message.getNextInt64();
	}

	uint8_t getNextUint8()
	{
		return message.getNextUint8();
	}

	uint16_t getNextUint16()
	{
		return message.getNextUint16();
	}

	uint32_t getNextUint32()
	{
		return message.getNextUint32();
	}

	uint64_t getNextUint64()
	{
		return message.getNextUint64();
	}

	float getNextFloat()
	{
		return message.getNextFloat();
	}

	double getNextDouble()
	{
		return message.getNextDouble();
	}

	bool getNextBool()
	{
		return message.getNextBool();
	}

	std::string getNextString()
	{
		return message.getNextString();
	}


	// constructors
	SocketMessage() noexcept;

	SocketMessage(int _FD, uint32_t _type, uint32_t _priority, const ByteArray &_IP,
							const ByteArray &_port, const ByteArray &_CSRFkey,
							const ByteArray &_message) noexcept;

	SocketMessage(int _FD, uint32_t _type, uint32_t _priority, const ByteArray &_IP,
							const ByteArray &_port, const ByteArray &_CSRFkey,
							ByteArray &&_message) noexcept;

	SocketMessage(const SocketMessage &m) noexcept; //copy constructor

	SocketMessage(SocketMessage&& m) noexcept; //move constructor

	SocketMessage& operator=(const SocketMessage &m) noexcept;  //copy assignment


private:
	std::chrono::system_clock::time_point calculateTime();

	int FD{-1};
	uint32_t type{0};
	uint32_t priority{0};
	std::chrono::system_clock::time_point sentTime;
	ByteArray IP {};
	ByteArray port {};
	ByteArray CSRFkey {};
	ByteArray message {};
};

#endif /* SOURCE_DATA_TYPES_SOCKET_MESSAGE_H */
