#include "source/data_types/byte_array.h"
#include <cstring> //memcopy
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
		throw std::runtime_error(LOG_EXCEPTION(std::string() + "ByteArray::getBytes tried to read past end of buffer of size " + \
			std::to_string(std::vector<uint8_t>::size()) + \
			" using value of " + std::to_string(currentIndex + sizeOfBytes)));
	}
}

void ByteArray::append(const std::string &str)
{
	if(str.size() > 65535 ){
		throw std::runtime_error(LOG_EXCEPTION("string is too large"));
	}
	auto strSize = static_cast<uint16_t>( str.size() );
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
	auto strSize = static_cast<size_t>(getNextUint16());
	output.resize(strSize);
	getBytes(&output[0], strSize);
	return output;
}


std::string ByteArray::toString() const
{
	std::string output;
	if(std::vector<uint8_t>::size() > 0) {
		size_t pos = tell();
		seek(0);
		output.resize(std::vector<uint8_t>::size());
		getBytes(&output[0], std::vector<uint8_t>::size());
		seek(pos);
	}
	return output;
}

void ByteArray::seek(size_t pos) const
{
	if( pos < std::vector<uint8_t>::size() ) currentIndex = pos;
	else {
		throw std::runtime_error(LOG_EXCEPTION(std::string() + "ByteArray::seek tried to seek to: " + \
			std::to_string(pos) + ", which is outside bounds of buffer with size: " + \
			std::to_string(std::vector<uint8_t>::size())));
	}
}
