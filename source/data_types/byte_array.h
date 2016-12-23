#ifndef SOURCE_DATA_TYPES_BYTE_ARRAY_H
#define SOURCE_DATA_TYPES_BYTE_ARRAY_H
//#include "source/data_types/byte_array.h"

#include <vector>
#include <string>
#include <stdint.h>

/// Vector to hold serialized data structures for network transmission.
class ByteArray : public std::vector<uint8_t>
{
public:
	/** @name append
	 * These functions append data to the end of data array.
	 */
	///@{
	void append(const ByteArray &arr);
	void append(const bool &num);
	void append(const std::string &str);

	template<class T>
	void append(const T &num);
	void appendWithNoSize(const std::string &str);
	///@}

	/// Sets currentIndex to pos
	/** It throws an exception if an attempt is made to set it past the end
	of the buffer. */
	void seek(size_t pos) const;

	/// returns currentIndex
	size_t tell() const;

	/** @name getters
	 *These functions return a value from the data array starting at
	 *currentIndex and then advance currentIndex to the byte
	 *following the end of the value read.
	 */
	///@{
	int8_t   getNextInt8()   const;
	int16_t  getNextInt16()  const;
	int32_t  getNextInt32()  const;
	int64_t  getNextInt64()  const;
	uint8_t  getNextUint8()  const;
	uint16_t getNextUint16() const;
	uint32_t getNextUint32() const;
	uint64_t getNextUint64() const;
	float    getNextFloat()  const;
	double   getNextDouble() const;
	bool     getNextBool()   const;
	std::string getNextString() const;
	///@}
	/// @name Constructors
	///@{
	ByteArray() noexcept;

	explicit ByteArray(std::string str);

	explicit ByteArray( const std::allocator<int8_t>& alloc );

	ByteArray( size_type count, const uint8_t& value,
	           const std::allocator<int8_t>& alloc = std::allocator<int8_t>());

	explicit ByteArray( size_type count,
	           const std::allocator<int8_t>& alloc = std::allocator<int8_t>());

	template< class InputIt >
	ByteArray( InputIt first, InputIt last,
	           const std::allocator<int8_t>& alloc = std::allocator<int8_t>());

	ByteArray( std::initializer_list<uint8_t> &init,
	           const std::allocator<int8_t>& alloc = std::allocator<int8_t>());
	///@}
	/// @name Copy Constructors
	///@{
	ByteArray( const ByteArray& other );
	ByteArray( const ByteArray& other, const std::allocator<int8_t>& alloc );
	///@}
	/// @name Move Constructors
	///@{
	ByteArray( ByteArray&& other );
	ByteArray( ByteArray&& other, const std::allocator<int8_t>& alloc );
	///@}
	/// @name Assignment
	///@{
	ByteArray& operator=( const ByteArray& other );
	ByteArray& operator=( ByteArray&& other );
	ByteArray& operator=( std::initializer_list<uint8_t> ilist );
	///@}

	/// Adds bytes of length sizeOfBytes to end of vector
	void appendBytes(const void* bytes, size_t sizeOfBytes);

	/// Copies data from vector starting at currentIndex and ending at
	/// currentIndex+sizeOfBytes-1 and stores it in bytes. The value of
	/// currentIndex is then advanced to the byte following the end of the data
	/// read. It throws an exception if an attempt is made to read past the end
	/// of the buffer.
	void getBytes(void* bytes, size_t sizeOfBytes) const;

	std::string toString() const;
private:
	/// Position in array from which values will be read.
	mutable size_t currentIndex {0};
};








inline void ByteArray::append(const ByteArray &arr)
{
	appendBytes(&arr[0], arr.size());
}

inline void ByteArray::append(const bool &num)
{
	int8_t flag = 0;
	if(num) flag = ~0;
	appendBytes(&flag, sizeof (flag));
}

template<class T>
inline void ByteArray::append(const T &num)
{
	static_assert(std::is_arithmetic<T>::value, "attempted to store incompatable type");
	appendBytes(&num, sizeof (T));
}

inline int8_t ByteArray::getNextInt8() const
{
	int8_t output;
	getBytes(&output, sizeof (output));
	return output;
}

inline int16_t ByteArray::getNextInt16() const
{
	int16_t output;
	getBytes(&output, sizeof (output));
	return output;
}

inline int32_t ByteArray::getNextInt32() const
{
	int32_t output;
	getBytes(&output, sizeof (output));
	return output;
}

inline int64_t ByteArray::getNextInt64() const
{
	int64_t output;
	getBytes(&output, sizeof (output));
	return output;
}

inline uint8_t ByteArray::getNextUint8() const
{
	uint8_t output;
	getBytes(&output, sizeof (output));
	return output;
}

inline uint16_t ByteArray::getNextUint16() const
{
	uint16_t output;
	getBytes(&output, sizeof (output));
	return output;
}

inline uint32_t ByteArray::getNextUint32() const
{
	uint32_t output;
	getBytes(&output, sizeof (output));
	return output;
}

inline uint64_t ByteArray::getNextUint64() const
{
	uint64_t output;
	getBytes(&output, sizeof (output));
	return output;
}

inline float ByteArray::getNextFloat() const
{
	float output;
	getBytes(&output, sizeof (output));
	return output;
}

inline double ByteArray::getNextDouble() const
{
	double output;
	getBytes(&output, sizeof (output));
	return output;
}

inline bool ByteArray::getNextBool() const
{
	int8_t flag;
	getBytes(&flag, sizeof (flag));
	return flag != 0;
}

inline size_t ByteArray::tell() const
{
	return currentIndex;
}

//constructors
inline ByteArray::ByteArray() noexcept :
	std::vector<uint8_t>( std::allocator<int8_t>() ),
	currentIndex(0)
{}

inline ByteArray::ByteArray( const std::allocator<int8_t>& alloc ) :
	std::vector<uint8_t>(alloc),
	currentIndex(0)
{}

inline ByteArray::ByteArray( size_type count, const uint8_t& value,
            const std::allocator<int8_t>& alloc) :
	std::vector<uint8_t>(count, value, alloc),
	currentIndex(0)
{}

inline ByteArray::ByteArray( size_type count,
            const std::allocator<int8_t>& alloc) :
	std::vector<uint8_t>(count, alloc),
	currentIndex(0)
{}

template< class InputIt >
inline ByteArray::ByteArray( InputIt first, InputIt last,
            const std::allocator<int8_t>& alloc) :
	std::vector<uint8_t>(first, last, alloc),
	currentIndex(0)
{}

inline ByteArray::ByteArray( std::initializer_list<uint8_t> &init,
            const std::allocator<int8_t>& alloc ) :
	std::vector<uint8_t>(init, alloc),
	currentIndex(0)
{}

inline ByteArray::ByteArray(std::string str) :
	std::vector<uint8_t>( std::allocator<int8_t>() ), currentIndex(0)
{
	this->append(str);
}

//copy constructors
inline ByteArray::ByteArray( const ByteArray& other ) :
	std::vector<uint8_t>(other),
	currentIndex(other.currentIndex)
{}

inline ByteArray::ByteArray( const ByteArray& other,
	                         const std::allocator<int8_t>& alloc ) :
	std::vector<uint8_t> (other, alloc),
	currentIndex(other.currentIndex)
{}

inline ByteArray::ByteArray( ByteArray&& other ) :
	std::vector<uint8_t>( std::move(other) ),
	currentIndex(other.currentIndex)
{}

inline ByteArray::ByteArray( ByteArray&& other,
	                         const std::allocator<int8_t>& alloc ) :
	std::vector<uint8_t>( std::move(other), alloc ),
	currentIndex(other.currentIndex)
{}



//assignment
inline ByteArray& ByteArray::operator=( const ByteArray& other )
{
	std::vector<uint8_t>::operator=(other);
	currentIndex = other.currentIndex;
	return *this;
}

inline ByteArray& ByteArray::operator=( ByteArray&& other )
{
	std::vector<uint8_t>::operator=( std::move(other) );
	currentIndex = other.currentIndex;
	return *this;
}

inline ByteArray& ByteArray::operator=( std::initializer_list<uint8_t> ilist )
{
	std::vector<uint8_t>::operator=(ilist);
	currentIndex = 0;
	return *this;
}



#endif /* SOURCE_DATA_TYPES_BYTE_ARRAY_H */
