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

	bool isNotEmpty() const;

	int getFD() const
	{
		return FD;
	}

	uint32_t getType() const
	{
		return type;
	}

	uint32_t getPriority() const
	{
		return priority;
	}

	std::chrono::system_clock::time_point getSentTime() const
	{
		return sentTime;
	}

	ByteArray getIP() const
	{
		return IP;
	}

	ByteArray getPort() const
	{
		return port;
	}

	ByteArray getCSRFkey() const
	{
		return CSRFkey;
	}

	int8_t getNextInt8() const
	{
		return message.getNextInt8();
	}

	int16_t getNextInt16() const
	{
		return message.getNextInt16();
	}

	int32_t getNextInt32() const
	{
		return message.getNextInt32();
	}

	int64_t getNextInt64() const
	{
		return message.getNextInt64();
	}

	uint8_t getNextUint8() const
	{
		return message.getNextUint8();
	}

	uint16_t getNextUint16() const
	{
		return message.getNextUint16();
	}

	uint32_t getNextUint32() const
	{
		return message.getNextUint32();
	}

	uint64_t getNextUint64() const
	{
		return message.getNextUint64();
	}

	float getNextFloat() const
	{
		return message.getNextFloat();
	}

	double getNextDouble() const
	{
		return message.getNextDouble();
	}

	bool getNextBool() const
	{
		return message.getNextBool();
	}

	std::string getNextString() const
	{
		return message.getNextString();
	}
	ByteArray getMessage() const
	{
		return message;
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
