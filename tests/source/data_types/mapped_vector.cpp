#define TEST_FILE_LINK_DEPENDENCIES ""
#include "source/data_types/mapped_vector.h"

#include "tests/test.h"

MappedVector<int,char> createTestVector()
{
	MappedVector<int,char> test;
	test.set(4, 'f');
	test.set(3, 'i');
	test.set(2, 'r');
	test.set(1, 's');
	test.set(0, 't');
	return test;
}

TEST(MappedVectorTest, TestSet)
{
	MappedVector<int,char> test = createTestVector();

	EXPECT_EQ(test[0], 't');
	EXPECT_EQ(test[1], 's');
	EXPECT_EQ(test[2], 'r');
	EXPECT_EQ(test[3], 'i');
	EXPECT_EQ(test[4], 'f');
}

TEST(MappedVectorTest, TestErase)
{
	MappedVector<int,char> test = createTestVector();

	test.erase(2);

	EXPECT_EQ(test.getRawData(2), 't');
	EXPECT_EQ(test.size(), 4ULL);
	EXPECT_EQ(test[0], 't');
}

TEST(MappedVectorTest, TestAccess)
{
	MappedVector<int,char> test = createTestVector();

	char testchar = test[47];
	EXPECT_EQ(test.size(), 6ULL);
	test[47] = testchar = 't';
	EXPECT_EQ(test[47], testchar);
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	STAY_SILENT_ON_SUCCESS;
	return RUN_ALL_TESTS();
}
