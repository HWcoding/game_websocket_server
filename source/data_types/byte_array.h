#ifndef SOURCE_DATA_TYPES_BYTE_ARRAY_H
#define SOURCE_DATA_TYPES_BYTE_ARRAY_H
//#include "source/data_types/byte_array.h"

#include <vector>
#include <string>
#include <stdint.h>

/**
 * Vector to hold serialized data structures for network transmission. The data
 * is stored in little-endian byte order. When accessing data, seek(size_t pos)
 * can be used to set the position (currentIndex) you begin reading from (each
 * increment is one byte starting at zero for the first byte) and tell() is used
 * to get the current position. The position is set to zero (first byte) at
 * initialization. The various getters will return the data in the format
 * of the return type and then advance the position to the byte following the
 * last retrieved data. You can step through the array and retrieve all the data
 * by calling the appropriate getter for the data in the order that it was
 * originaly added.
 * appendBytes() and getBytes() are unsafe low-level functions and are only
 * intended to be used by a child or wrapper class adding custom functionality.
 *
 */
class ByteArray : public std::vector<uint8_t>
{
public:
	/************************************************************************/
	/** @name Appenders
	 * These functions append data to the end of the data buffer. If there is
	 * not enough space, the buffer is recreated with a larger capacity.
	 * If the buffer is recreated, all iterators, pointers, and references
	 * related to the container are invalidated.
	 *
	 * @{
	 */

	/*
	 * Adds another ByteArray onto the end of the buffer. The rules for
	 * iterator, pointer, and reference validity listed under Appenders apply.
	 *
	 * @param arr ByteArray to add onto the container.
	 */
	void append(const ByteArray &arr);

	/*
	 * Adds another bool onto the end of the buffer. The boolean take 1 byte of
	 * space. The rules for iterator, pointer, and reference validity listed
	 * under Appenders apply.
	 *
	 * @param boolean boolean to add onto the container.
	 */
	void append(const bool &boolean);

	/*
	 * Adds another string onto the end of the buffer. The unsigned size of the
	 * string is stored in the first 2 bytes in little-endian byte order. The
	 * contents of the string are stored in the bytes that follow. It does NOT
	 * end in a null byte. This is a usefull way to store the data if you want
	 * to know the size of the string before reading it. Use getNextString() to
	 * retrieve the sting when the currentIndex is at the beginning of the size.
	 * The rules for iterator, pointer, and reference validity listed under
	 * Appenders apply.
	 *
	 * @param str String to add onto the container.
	 *
	 * @throws std::runtime_error if the string size is larger than 65535 bytes (can't
	 *                            fit size in 2 bytes).
	 */
	void append(const std::string &str);

	/*
	 * Adds another string onto the end of the buffer. The contents
	 * of the string are stored in raw format. It does NOT end in a null byte.
	 * The caller has to keep track of the size of the string. This is useful
	 * for when you want to just store a single string in the buffer and nothing
	 * else. If the string is the only thing in the container it can be retrieved
	 * with toString(). Otherwise, the caller will have to extract it with
	 * getBytes(void* bytes, size_t sizeOfBytes). The rules for iterator,
	 * pointer, and reference validity listed under Appenders apply.
	 *
	 * @param str String to add onto the container.
	 */
	void appendWithNoSize(const std::string &str);

	/**
	 * Adds a variable of type T to the end of the buffer. Use the appropriate
	 * getter or getNext<T>() to retrieve the value.The variable's memory is
	 * memcpy'ed into the buffer. This means that only a shallow copy is stored
	 * and storing objects may have strange behavior if they have virtual
	 * methods since an implementaion may store vtable pointers in the instance.
	 * It is recommended to only store plain old structures and built-in types.
	 * The rules for iterator, pointer, and reference validity listed under
	 * Appenders apply.
	 *
	 * @param num Variable of type T to add to the array. bools will be
	 *            converted to int8_t. false will be stored as 0, and true -1
	 */
	template<class T>
	void append(const T &num);

	/**
	 * Adds bytes of length sizeOfBytes to end of vector. This is an unsafe low
	 * level API and should only be used if the other functions cannot be used.
	 * There is no bounds checking on the size of the variable being read,
	 * so if you specify sizeOfBytes that is larger than the variable,
	 * the function will try to read past the end of the variable with
	 * undefined behavior.
	 *
	 * @param bytes pointer to the beginning of the variable you wish to store
	 *              in the array.
	 * @param sizeOfBytes the size of the variable in bytes.
	 *
	 */
	void appendBytes(const void* bytes, size_t sizeOfBytes);

	/**@}*/ //end Appenders**************************************************






	/************************************************************************/
	/** @name Getters
	 * These functions (except for toString) return a value from the data array
	 * starting at currentIndex and then advance currentIndex to the byte
	 * following the end of the value read. An exception is thrown if the
	 * operation tries to read past the end of the buffer.
	 *
	 * @{
	 */

	/**
	 * Returns a value of type T from the buffer starting at currentIndex. The
	 * currentIndex is then set to the byte after the value read. It is used
	 * like getNext<TYPE>(). getNext<bool>() is a wrapper around getNext<int_8>()
	 * and follows the same rules as append(bool): 0 returns false, -1 true.
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	template<class T>
	T getNext() const;

	/**
	 * A wrapper around getNext<int8_t>()
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	int8_t getNextInt8()   const;

	/**
	 * A wrapper around getNext<int16_t>()
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	int16_t getNextInt16()  const;

	/**
	 * A wrapper around getNext<int32_t>()
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	int32_t getNextInt32()  const;

	/**
	 * A wrapper around getNext<int64_t>()
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	int64_t getNextInt64()  const;

	/**
	 * A wrapper around getNext<uint8_t>()
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	uint8_t getNextUint8()  const;

	/**
	 * A wrapper around getNext<uint16_t>()
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	uint16_t getNextUint16() const;

	/**
	 * A wrapper around getNext<uint32_t>()
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	uint32_t getNextUint32() const;

	/**
	 * A wrapper around getNext<uint64_t>()
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	uint64_t getNextUint64() const;

	/**
	 * A wrapper around getNext<float>()
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	float getNextFloat()  const;

	/**
	 * A wrapper around getNext<double>()
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	double getNextDouble() const;

	/**
	 * A wrapper around getNext<int8_t>()
	 * bools are stored in an int8_t
	 *
	 * @return false if the int8_t is 0, true otherwise
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	bool getNextBool()   const;

	/**
	 * Returns a string that was stored using append(std::string). This will not
	 * work for strings stored using appendWithNoSize() or any other method that
	 * did not store the string size as the first 2 bytes in little-endian
	 * byte order.
	 *
	 * @return A string that was originally stored useing append(std::string)
	 *
	 * @throws std::runtime_error if the operation would read past the end of
	 *                            the buffer.
	 */
	std::string getNextString() const;

	/**
	 * Returns the contents of the entire buffer as a string. currentIndex is
	 * ignored and not updated. The read always starts at the beginning. No
	 * conversions are done. Its just a byte for byte trasfer to extended ascii
	 * and may result in unprintable characters if the buffer stored something
	 * other than a string. An empty buffer will result in an empty string.
	 *
	 * @return the contents of the buffer in a std::string
	 *
	 */
	std::string toString() const;

	/**
	 * Copies data from the buffer into the memory pointed to by bytes. This is
	 * an unsafe low level API and should only be used if the other functions
	 * cannot be used. The copy begins at currentIndex and continues for
	 * sizeOfBytes bytes. The value of currentIndex is then advanced to the byte
	 * following the end of the data read. It throws an exception if an attempt
	 * is made to read past the end of the buffer. However, no bounds check is
	 * done on bytes. If the variable pointed to by bytes is smaller than
	 * sizeOfBytes, the the function will attempt to write past the end of
	 * the variable with undefined behavior.
	 *
	 * @throws std::runtime_error if an attempt is made to read past the end of
	 *                            the buffer.
	 */
	void getBytes(void* bytes, size_t sizeOfBytes) const;
	/**@}*/ //end Getters****************************************************






	/************************************************************************/
	/** @name Constructors
	 * @{
	 */

	/**
	 * Constructs an empty ByteArray. currentIndex is set to 0.
	 */
	ByteArray() noexcept;

	/**
	 * Constructs a ByteArray and sets the buffer's content to that of str. The
	 * string is NOT null terminated, does not store the size, and CANNOT be
	 * retrieved using getNextString(). This is intended to be used if you are
	 * only planning to store concatenated strings in the buffer and only need
	 * to retrieve the concatenated result. currentIndex is set to 0.
	 *
	 * @param str The string to copy into the buffer.
	 *
	 */
	explicit ByteArray(std::string str);

	/**
	 * Constructs an empty ByteArray that uses alloc as the memory allocator.
	 *
	 * @param alloc The internal allocator type used by the container
	 *
	 */
	explicit ByteArray( const std::allocator<int8_t>& alloc );

	/**
	 * Constructs a ByteArray with count elements.
	 *
	 * @param count The number of the elements in the ByteArray.
	 *
	 * @param alloc (optional) The internal allocator type used by the container
	 *
	 */
	explicit ByteArray( size_type count,
	           const std::allocator<int8_t>& alloc = std::allocator<int8_t>());
	/**
	 * Constructs a ByteArray with count elements. Each element is a copy of value.
	 *
	 * @param count The number of the elements in the ByteArray.
	 *
	 * @param value Every element is set to this value.
	 *
	 * @param alloc (optional) The internal allocator type used by the container
	 *
	 */
	ByteArray( size_type count, const uint8_t& value,
	           const std::allocator<int8_t>& alloc = std::allocator<int8_t>());

	/**
	 * Constructs a ByteArray with as many elements as the range [first,last).
	 * Each element is emplace-constructed from the corresponding element in
	 * the range. The range used includes all the elements between first and
	 * last, including the element pointed to by first but not the element
	 * pointed to by last.
	 *
	 * @param first Input iterator to the initial position in a range.
	 *
	 * @param last Input iterator to the position after the range.
	 *
	 * @param alloc (optional) The internal allocator type used by the container
	 *
	 */
	template< class InputIt >
	ByteArray( InputIt first, InputIt last,
	           const std::allocator<int8_t>& alloc = std::allocator<int8_t>());

	/**
	 * Constructs a ByteArray with a copy of each of the corresponding elements
	 * in list.
	 *
	 * @param list An initializer_list object automatically constructed from
	 *             an initializer list declarator.
	 *
	 * @param alloc (optional) The internal allocator type used by the container
	 *
	 */
	ByteArray( std::initializer_list<uint8_t> &list,
	           const std::allocator<int8_t>& alloc = std::allocator<int8_t>());
	/**@}*/ //Constructors****************************************************





	/************************************************************************/
	/** @name Copy Constructors
	 * @{
	 */

	/*
	 * Copy Constructor. Copies all the elements from other into the
	 * ByteArray (with other preserving its contents).
	 *
	 * @param other the ByteArray to copied into this one.
	 *
	 */
	ByteArray( const ByteArray& other );

	/**
	 * Copy Constructor. Copies all the elements from other into the
	 * ByteArray (with other preserving its contents).
	 *
	 * @param other the ByteArray to copied into this one.
	 *
	 * @param alloc The internal allocator type used by the container
	 *
	 */
	ByteArray( const ByteArray& other, const std::allocator<int8_t>& alloc );
	/**@}*/ //****************************************************************





	/************************************************************************/
	/** @name Move Constructors
	 * @{
	 */

	/*
	 * Constructs a ByteArray that acquires the elements of other. other is
	 * left in an unspecified state after the transfer.
	 *
	 * @param other the ByteArray to move into this one.
	 */
	ByteArray( ByteArray&& other );

	/*
	 * Constructs a ByteArray that acquires the elements of other. other is
	 * left in an unspecified state after the transfer. If alloc is different
	 * than other's alloc, each element is moved individually. Otherwise the
	 * whole buffer is transfered from other.
	 *
	 * @param other the ByteArray to move into this one.
	 *
	 * @param alloc The internal allocator type used by the container
	 *
	 */
	ByteArray( ByteArray&& other, const std::allocator<int8_t>& alloc );
	/**@}*/ //****************************************************************





	/************************************************************************/
	/** @name Assignment Operators
	 * @{
	 */

	/*
	 * Copy assignment operator. Copies all the elements from other into the
	 * ByteArray (with other preserving its contents).
	 *
	 * @param other the ByteArray to copied into this one.
	 *
	 * @return a reference to this instance.
	 *
	 */
	ByteArray& operator=( const ByteArray& other );

	/*
	 * Move assignment operator. Moves all the elements from other into the
	 * ByteArray (other is left in an unspecified state).
	 *
	 * @param other the ByteArray to moved into this one.
	 *
	 * @return a reference to this instance.
	 *
	 */
	ByteArray& operator=( ByteArray&& other );

	/**
	 * Initializer list assignment operator. Copies each of the corresponding
	 * elements from the list.
	 *
	 * @param list An initializer_list object automatically constructed from
	 *             an initializer list declarator.
	 *
	 * @return a reference to this instance.
	 *
	 */
	ByteArray& operator=( std::initializer_list<uint8_t> list );
	/**@}*/ //****************************************************************


	/*
	 * Sets currentIndex to pos. It throws an exception if an attempt is made
	 * to set it past the end of the buffer.
	 *
	 * @param pos the position to set currentIndex to. The position is measured
	 *            in bytes and begins a 0 for the first byte and size()-1 for
	 *            the last.
	 *
	 * @throws std::runtime_error if an attempt is made to set the position past
	 *                            the end of the buffer
	 */
	void seek(size_t pos) const;

	/*
	 * Returns currentIndex.
	 *
	 * @return the position of currentIndex. The position is measured
	 *         in bytes and begins a 0 for the first byte and size()-1 for
	 *         the last.
	 *
	 */
	size_t tell() const;

private:
	/** Position in array from which values will be read. */
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
	appendBytes(&num, sizeof (T));
}

template<>
inline bool ByteArray::getNext<bool>() const
{
	int8_t flag = getNext<int8_t>();
	return flag != 0;
}

template<class T>
inline T ByteArray::getNext() const
{
	T output;
	getBytes(&output, sizeof (T));
	return output;
}

inline int8_t ByteArray::getNextInt8() const
{
	return getNext<int8_t>();
}

inline int16_t ByteArray::getNextInt16() const
{
	return getNext<int16_t>();
}

inline int32_t ByteArray::getNextInt32() const
{
	return getNext<int32_t>();
}

inline int64_t ByteArray::getNextInt64() const
{
	return getNext<int64_t>();
}

inline uint8_t ByteArray::getNextUint8() const
{
	return getNext<uint8_t>();
}

inline uint16_t ByteArray::getNextUint16() const
{
	return getNext<uint16_t>();
}

inline uint32_t ByteArray::getNextUint32() const
{
	return getNext<uint32_t>();
}

inline uint64_t ByteArray::getNextUint64() const
{
	return getNext<uint64_t>();
}

inline float ByteArray::getNextFloat() const
{
	return getNext<float>();
}

inline double ByteArray::getNextDouble() const
{
	return getNext<double>();
}

inline bool ByteArray::getNextBool() const
{
	return getNext<bool>();
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

inline ByteArray::ByteArray( std::initializer_list<uint8_t> &list,
            const std::allocator<int8_t>& alloc ) :
	std::vector<uint8_t>(list, alloc),
	currentIndex(0)
{}

inline ByteArray::ByteArray(std::string str) :
	std::vector<uint8_t>( std::allocator<int8_t>() ), currentIndex(0)
{
	this->appendWithNoSize(str);
}


//copy constructors
inline ByteArray::ByteArray( const ByteArray& ) = default;

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

inline ByteArray& ByteArray::operator=( std::initializer_list<uint8_t> list )
{
	std::vector<uint8_t>::operator=(list);
	currentIndex = 0;
	return *this;
}



#endif /* SOURCE_DATA_TYPES_BYTE_ARRAY_H */
