#include "source/data_types/byte_array.h"
#include <string.h> //memcopy
#include "source/logging/exception_handler.h"

void ByteArray::appendBytes(const void* bytes, size_t sizeOfBytes)
{
	size_t thisSize = std::vector<uint8_t>::size();
	std::vector<uint8_t>::resize( thisSize+ sizeOfBytes );
	memcpy( &std::vector<uint8_t>::operator[](thisSize), bytes, sizeOfBytes );
}

void ByteArray::getBytes(void* bytes, size_t sizeOfBytes) const
{
	if(currentIndex+sizeOfBytes <= std::vector<uint8_t>::size()) {
		memcpy(bytes, &std::vector<uint8_t>::operator[](currentIndex), sizeOfBytes );
		currentIndex += sizeOfBytes;
	}
	else {
		throwInt("ByteArray::getBytes tried to read past end of buffer");
	}
}

void ByteArray::append(const std::string &str)
{
	if(str.size() > 65535 ) throw -1;
	uint16_t strSize = static_cast<uint16_t>( str.size() );
	appendBytes(&strSize, sizeof (strSize));
	appendBytes(str.c_str(), strSize);
}

void ByteArray::appendWithNoSize(const std::string &str)
{
	appendBytes(str.c_str(), str.size() );
}

std::string ByteArray::getNextString() const
{
	std::string output;
	size_t strSize = static_cast<size_t>(getNextUint16());
	output.resize(strSize);
	getBytes(&output[0], strSize);
	return output;
}


std::string ByteArray::toString() const
{
	std::string output;
	size_t pos = tell();
	output.resize(std::vector<uint8_t>::size());
	getBytes(&output[0], std::vector<uint8_t>::size());
	seek(pos);
	return output;
}

void ByteArray::seek(size_t pos) const
{
	if( pos < std::vector<uint8_t>::size() ) currentIndex = pos;
	else {
		throwInt("ByteArray::seek tried to seek to: "<<pos<<", which is past end of buffer with size: "<<std::vector<uint8_t>::size());
	}
}
