#include "source/data_types/byte_array.h"
#include <string.h> //memcopy

void ByteArray::appendBytes(const void* bytes, size_t sizeOfBytes)
{
	size_t thisSize = std::vector<uint8_t>::size();
	std::vector<uint8_t>::resize( thisSize+ sizeOfBytes );
	memcpy( &std::vector<uint8_t>::operator[](thisSize), bytes, sizeOfBytes );
}

void ByteArray::getBytes(void* bytes, size_t sizeOfBytes) const
{
	if(currentIndex+sizeOfBytes > std::vector<uint8_t>::size())throw -1; //out of bounds
	memcpy(bytes, &std::vector<uint8_t>::operator[](currentIndex), sizeOfBytes );
	currentIndex += sizeOfBytes;
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
