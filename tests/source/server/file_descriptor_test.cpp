//#include "tests/mock_header.h"
/*#define TESTING //to activate conditional macros for test logging and access
#include "tests/test.h"

#include "tests/server/mocks_stubs/mock_system_wrapper.h"
#include "tests/server/mocks_stubs/socket_test_helpers.h"
#include "server/socket/file_descriptor.h"
#include "tests/server/file_descriptor_test/file_descriptor_test.h"

#include <cgreen/cgreen.h>
#include <string>
using namespace cgreen;

Describe(FileDescriptorTest)
BeforeEach(FileDescriptorTest) {}
AfterEach(FileDescriptorTest) {}


namespace FileDescriptor_Test{


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


Ensure(FileDescriptorTest, move_constructor_test) {
	FD_TestFactory testDescriptor(1);

	FileDescriptor MovedCopy(std::move( *testDescriptor ));

	assert_that( MovedCopy.getIP().toString().c_str(),is_equal_to_string("testIP") );
	assert_that( testDescriptor->getIP().toString().c_str(),is_equal_to_string("") );

	assert_that( MovedCopy.getPort().toString().c_str(),is_equal_to_string("testPort") );
	assert_that( testDescriptor->getPort().toString().c_str(),is_equal_to_string("") );

	assert_that( MovedCopy.getCSRFkey().toString().c_str(),is_equal_to_string("testCSRFkey") );
	assert_that( testDescriptor->getCSRFkey().toString().c_str(),is_equal_to_string("") );

	assert_that( MovedCopy.getFD(), is_equal_to(1));
	assert_that( testDescriptor->getFD(), is_equal_to(-1));
}



Ensure(FileDescriptorTest, copy_constructor_test) {
	FD_TestFactory testDescriptor(1);

	FileDescriptor CopyDescriptor( *testDescriptor );

	assert_that( CopyDescriptor.getIP().toString().c_str(),is_equal_to_string("testIP") );
	assert_that( testDescriptor->getIP().toString().c_str(),is_equal_to_string("testIP") );

	assert_that( CopyDescriptor.getPort().toString().c_str(),is_equal_to_string("testPort") );
	assert_that( testDescriptor->getPort().toString().c_str(),is_equal_to_string("testPort") );

	assert_that( CopyDescriptor.getCSRFkey().toString().c_str(),is_equal_to_string("testCSRFkey") );
	assert_that( testDescriptor->getCSRFkey().toString().c_str(),is_equal_to_string("testCSRFkey") );

	assert_that( CopyDescriptor.getFD(), is_equal_to(1));
	assert_that( testDescriptor->getFD(), is_equal_to(1));
}



Ensure(FileDescriptorTest, move_assignment_test) {
	MockSystemWrapper systemWrap;
	FD_TestFactory testDescriptor(1);
	FileDescriptor MovedCopy(&systemWrap,-1);

	MovedCopy = std::move( *testDescriptor );

	assert_that( MovedCopy.getIP().toString().c_str(),is_equal_to_string("testIP") );
	assert_that( testDescriptor->getIP().toString().c_str(),is_equal_to_string("") );

	assert_that( MovedCopy.getPort().toString().c_str(),is_equal_to_string("testPort") );
	assert_that( testDescriptor->getPort().toString().c_str(),is_equal_to_string("") );

	assert_that( MovedCopy.getCSRFkey().toString().c_str(),is_equal_to_string("testCSRFkey") );
	assert_that( testDescriptor->getCSRFkey().toString().c_str(),is_equal_to_string("") );

	assert_that( MovedCopy.getFD(), is_equal_to(1));
	assert_that( testDescriptor->getFD(), is_equal_to(-1));
}



Ensure(FileDescriptorTest, copy_assignment_test) {
	FD_TestFactory testDescriptor(1);
	FD_TestFactory CopyDescriptor(-1);
//std::cout<<"calling copy assignment"<<std::endl;
	*CopyDescriptor = *testDescriptor;

	assert_that( CopyDescriptor->getIP().toString().c_str(),is_equal_to_string("testIP") );
	assert_that( testDescriptor->getIP().toString().c_str(),is_equal_to_string("testIP") );

	assert_that( CopyDescriptor->getPort().toString().c_str(),is_equal_to_string("testPort") );
	assert_that( testDescriptor->getPort().toString().c_str(),is_equal_to_string("testPort") );

	assert_that( CopyDescriptor->getCSRFkey().toString().c_str(),is_equal_to_string("testCSRFkey") );
	assert_that( testDescriptor->getCSRFkey().toString().c_str(),is_equal_to_string("testCSRFkey") );

	assert_that( CopyDescriptor->getFD(), is_equal_to(1));
	assert_that( testDescriptor->getFD(), is_equal_to(1));
}



int test(){
	TestSuite *suite = create_named_test_suite("File Descriptor");
	add_test_with_context(suite, FileDescriptorTest, move_constructor_test);
	add_test_with_context(suite, FileDescriptorTest, copy_constructor_test);
	add_test_with_context(suite, FileDescriptorTest, move_assignment_test);
	add_test_with_context(suite, FileDescriptorTest, copy_assignment_test);
	return run_test_suite(suite, create_text_reporter());
}



}
int main(){
	return FileDescriptor_Test::test();
}*/

int main(){return 0;}
