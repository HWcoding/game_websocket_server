#define TEST_FILE_LINK_DEPENDENCIES "source/data_types/byte_array.cpp, \
									source/server/socket/file_descriptor.cpp \
									source/server/socket/system_wrapper.cpp"

#include "source/server/socket/file_descriptor.h"
#include "tests/test_lib/mocks_stubs/mock_system_wrapper.h"

#include "tests/test.h"

class FD_TestFactory{
	MockSystemWrapper systemWrap;
public:
	FileDescriptor FD;
	FD_TestFactory(int index): systemWrap(), FD(&systemWrap,index){
		FD.setIP("testIP");
		FD.setPort("testPort");
		FD.setCSRFkey("testCSRFkey");
	}
	FileDescriptor* operator->(){
		return &FD;
	}

	FileDescriptor& operator*(){
		return FD;
	}
};



TEST(FileDescriptorTest, MoveConstructor)
{
	FD_TestFactory testDescriptor(1);

	FileDescriptor movedCopy(std::move( *testDescriptor ));

	EXPECT_EQ( movedCopy.getIP().toString(), "testIP");
	EXPECT_EQ( testDescriptor->getIP().toString(), "");

	EXPECT_EQ( movedCopy.getPort().toString(), "testPort");
	EXPECT_EQ( testDescriptor->getPort().toString(), "");

	EXPECT_EQ( movedCopy.getCSRFkey().toString(), "testCSRFkey");
	EXPECT_EQ( testDescriptor->getCSRFkey().toString(), "");

	EXPECT_EQ( movedCopy.getFD(), 1);
	EXPECT_EQ( testDescriptor->getFD(), -1);
}



TEST(FileDescriptorTest, CopyConstructor)
{
	FD_TestFactory testDescriptor(1);

	FileDescriptor copyDescriptor(*testDescriptor );

	EXPECT_EQ( copyDescriptor.getIP().toString(), "testIP");
	EXPECT_EQ( testDescriptor->getIP().toString(), "testIP");

	EXPECT_EQ( copyDescriptor.getPort().toString(), "testPort");
	EXPECT_EQ( testDescriptor->getPort().toString(), "testPort");

	EXPECT_EQ( copyDescriptor.getCSRFkey().toString(), "testCSRFkey");
	EXPECT_EQ( testDescriptor->getCSRFkey().toString(), "testCSRFkey");

	EXPECT_EQ( copyDescriptor.getFD(), 1);
	EXPECT_EQ( testDescriptor->getFD(), 1);
}



TEST(FileDescriptorTest, MoveAssignment)
{
	MockSystemWrapper systemWrap;
	FD_TestFactory testDescriptor(1);
	FileDescriptor movedCopy(&systemWrap,-1);

	movedCopy = std::move( *testDescriptor );

	EXPECT_EQ( movedCopy.getIP().toString(), "testIP");
	EXPECT_EQ( testDescriptor->getIP().toString(), "");

	EXPECT_EQ( movedCopy.getPort().toString(), "testPort");
	EXPECT_EQ( testDescriptor->getPort().toString(), "");

	EXPECT_EQ( movedCopy.getCSRFkey().toString(), "testCSRFkey");
	EXPECT_EQ( testDescriptor->getCSRFkey().toString(), "");

	EXPECT_EQ( movedCopy.getFD(), 1);
	EXPECT_EQ( testDescriptor->getFD(), -1);
}


TEST(FileDescriptorTest, CopyAssignment)
{
	FD_TestFactory testDescriptor(1);
	FD_TestFactory copyDescriptor(-1);

	copyDescriptor.FD = testDescriptor.FD;

	EXPECT_EQ( copyDescriptor->getIP().toString(), "testIP");
	EXPECT_EQ( testDescriptor->getIP().toString(), "testIP");

	EXPECT_EQ( copyDescriptor->getPort().toString(), "testPort");
	EXPECT_EQ( testDescriptor->getPort().toString(), "testPort");

	EXPECT_EQ( copyDescriptor->getCSRFkey().toString(), "testCSRFkey");
	EXPECT_EQ( testDescriptor->getCSRFkey().toString(), "testCSRFkey");

	EXPECT_EQ( copyDescriptor->getFD(), 1);
	EXPECT_EQ( testDescriptor->getFD(), 1);
}



TEST(FileDescriptorTest, Setters)
{
	FD_TestFactory testDescriptor(1);
	std::string test = "";
	test.reserve(100);
	for(int i = 0; i<100; i++) {
		test.append("a");
	}
	testDescriptor->setIP(test);
	testDescriptor->setPort(test);
	testDescriptor->setCSRFkey(test);

	EXPECT_EQ( testDescriptor->getIP().toString(), test);
	EXPECT_EQ( testDescriptor->getPort().toString(), test);
	EXPECT_EQ( testDescriptor->getCSRFkey().toString(), test);
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	STAY_SILENT_ON_SUCCESS();
	return RUN_ALL_TESTS();
}
