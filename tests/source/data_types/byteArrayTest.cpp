#define TEST_FILE_LINK_DEPENDENCIES "source/data_types/byte_array.cpp"

#include "source/data_types/byte_array.h"

#include "tests/test.h"


TEST(ByteArrayTest, TestSeek)
{
	ByteArray test;
	test.resize(8);
	size_t testSize = test.size();

	//try to seek past last element
	EXPECT_ANY_THROW(test.seek(testSize));
	//seek to last element
	EXPECT_NO_THROW(test.seek(testSize-1));
	EXPECT_EQ(test.tell(), (size_t)testSize-1);

}


TEST(ByteArrayTest, TestInt8Input)
{
	ByteArray test;
	int8_t a(1);
	test.append(a);

	int8_t b(2);
	test.append(b);

	int8_t c(3);
	test.append(c);

	EXPECT_EQ(test.getNextInt8(), a);
	EXPECT_EQ(test.getNextInt8(), b);
	EXPECT_EQ(test.getNextInt8(), c);
}

TEST(ByteArrayTest, TestInt16Input)
{
	ByteArray test;
	int16_t a(1);
	test.append(a);

	int16_t b(2);
	test.append(b);

	int16_t c(3);
	test.append(c);

	EXPECT_EQ(test.getNextInt16(), a);
	EXPECT_EQ(test.getNextInt16(), b);
	EXPECT_EQ(test.getNextInt16(), c);
}

TEST(ByteArrayTest, TestInt32Input)
{
	ByteArray test;
	int32_t a(1);
	test.append(a);

	int32_t b(2);
	test.append(b);

	int32_t c(3);
	test.append(c);

	EXPECT_EQ(test.getNextInt32(), a);
	EXPECT_EQ(test.getNextInt32(), b);
	EXPECT_EQ(test.getNextInt32(), c);
}

TEST(ByteArrayTest, TestInt64Input)
{
	ByteArray test;
	int64_t a(1);
	test.append(a);

	int64_t b(2);
	test.append(b);

	int64_t c(3);
	test.append(c);

	EXPECT_EQ(test.getNextInt64(), a);
	EXPECT_EQ(test.getNextInt64(), b);
	EXPECT_EQ(test.getNextInt64(), c);
}

TEST(ByteArrayTest, TestUint8Input)
{
	ByteArray test;
	uint8_t a(1);
	test.append(a);

	uint8_t b(2);
	test.append(b);

	uint8_t c(3);
	test.append(c);

	EXPECT_EQ(test.getNextUint8(), a);
	EXPECT_EQ(test.getNextUint8(), b);
	EXPECT_EQ(test.getNextUint8(), c);
}

TEST(ByteArrayTest, TestUint16Input)
{
	ByteArray test;
	uint16_t a(1);
	test.append(a);

	uint16_t b(2);
	test.append(b);

	uint16_t c(3);
	test.append(c);

	EXPECT_EQ(test.getNextUint16(), a);
	EXPECT_EQ(test.getNextUint16(), b);
	EXPECT_EQ(test.getNextUint16(), c);
}

TEST(ByteArrayTest, TestUint32Input)
{
	ByteArray test;
	uint32_t a(1);
	test.append(a);

	uint32_t b(2);
	test.append(b);

	uint32_t c(3);
	test.append(c);

	EXPECT_EQ(test.getNextUint32(), a);
	EXPECT_EQ(test.getNextUint32(), b);
	EXPECT_EQ(test.getNextUint32(), c);
}

TEST(ByteArrayTest, TestUint64Input)
{
	ByteArray test;
	uint64_t a(1);
	test.append(a);

	uint64_t b(2);
	test.append(b);

	uint64_t c(3);
	test.append(c);

	EXPECT_EQ(test.getNextUint64(), a);
	EXPECT_EQ(test.getNextUint64(), b);
	EXPECT_EQ(test.getNextUint64(), c);
}


TEST(ByteArrayTest, TestFloatInput)
{
	ByteArray test;
	float a(1);
	test.append(a);

	float b(2);
	test.append(b);

	float c(3);
	test.append(c);

	EXPECT_EQ(test.getNextFloat(), a);
	EXPECT_EQ(test.getNextFloat(), b);
	EXPECT_EQ(test.getNextFloat(), c);
}


TEST(ByteArrayTest, TestDoubleInput)
{
	ByteArray test;
	double a(1);
	test.append(a);

	double b(2);
	test.append(b);

	double c(3);
	test.append(c);

	EXPECT_EQ(test.getNextDouble(), a);
	EXPECT_EQ(test.getNextDouble(), b);
	EXPECT_EQ(test.getNextDouble(), c);
}


TEST(ByteArrayTest, TestBoolInput)
{
	ByteArray test;
	bool a(true);
	test.append(a);

	bool b(false);
	test.append(b);

	bool c(true);
	test.append(c);

	EXPECT_EQ(test.getNextBool(), a);
	EXPECT_EQ(test.getNextBool(), b);
	EXPECT_EQ(test.getNextBool(), c);
}


TEST(ByteArrayTest, TestStringInput)
{
	ByteArray test;
	std::string string1("test1");
	test.append(string1);

	std::string string2("test2");
	test.append(string2);

	std::string string3("test3");
	test.append(string3);

	EXPECT_STREQ(test.getNextString().c_str(), string1.c_str());
	EXPECT_STREQ(test.getNextString().c_str(), string2.c_str());
	EXPECT_STREQ(test.getNextString().c_str(), string3.c_str());
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	STAY_SILENT_ON_SUCCESS;
	return RUN_ALL_TESTS();
}
