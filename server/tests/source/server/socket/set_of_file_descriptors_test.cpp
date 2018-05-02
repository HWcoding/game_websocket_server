#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/file_descriptor.cpp, \
									source/logging/exception_handler.cpp, \
									source/server/socket/set_of_file_descriptors.cpp"

#include "tests/test_lib/mocks_stubs/mock_system_wrapper.h"
#include "source/server/socket/set_of_file_descriptors.h"

#include "tests/test.h"


class FDsTestFactory{
	MockSystemWrapper &systemWrap;
	SetOfFileDescriptors FDs;
public:
	FDsTestFactory() :
		systemWrap(MockSystemWrapper::getMockSystemInstance(true)),
		FDs()
	{}

	SetOfFileDescriptors* get(){
		return &FDs;
	}
};





/*
void test_TellServerAboutNewConnection(){

}

void test_GetAndLockFD(){

}*/


void set_TEST_GLOBAL_INT_ToInput(int input);
int TEST_GLOBAL_INT;

void set_TEST_GLOBAL_INT_ToInput(int input){
	TEST_GLOBAL_INT=input;
}

TEST(SetOfFileDescriptorsTest, CallsCallbackWhenFDISRemoved)
{
	FDsTestFactory FD_fact;
	SetOfFileDescriptors * FDs = FD_fact.get();
	FDs->addFD(1);
	TEST_GLOBAL_INT = 4;
	FDs->addCloseFDCallback(&set_TEST_GLOBAL_INT_ToInput); //set_TEST_GLOBAL_INT_ToInput should be called with the FD (1) as its input when the FD is closed

	FDs->removeFD(1);

	EXPECT_EQ(TEST_GLOBAL_INT, 1);// check to see that TEST_GLOBAL_INT was changed from 4 to 1 - indicating that the function was called
}


TEST(SetOfFileDescriptorsTest, FDIsRemovedFromList) {
	FDsTestFactory FD_fact;
	SetOfFileDescriptors * FDs = FD_fact.get();
	FDs->addFD(1);

	FDs->removeFD(1);

	EXPECT_EQ(FDs->isFDOpen(1), false);
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
