#define TEST_FILE_LINK_DEPENDENCIES
//"source/images/png.cpp"

//#include "source/images/png.h"
#include "tests/test.h"


TEST(MessageQueueTest, getNextMessage)
{
	EXPECT_EQ(true, false);
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}