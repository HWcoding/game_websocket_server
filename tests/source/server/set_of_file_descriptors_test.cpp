/*#define TESTING //to activate conditional macros for test logging and access
#include "tests/test.h"

#include <iostream>
#include <string>
#include "tests/server/mocks_stubs/mock_system_wrapper.h"
#include "tests/server/set_of_file_descriptors_test/set_of_file_descriptors_test.h"
#include "server/socket/set_of_file_descriptors.h"
#include "main/includes.h"


#include <cgreen/cgreen.h>
using namespace cgreen;

Describe(SetOfFileDescriptorsTest)
BeforeEach(SetOfFileDescriptorsTest) {}
AfterEach(SetOfFileDescriptorsTest) {}



namespace SetOfFileDescriptors_Test{

class FDsTestFactory{
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs;
public:
	FDsTestFactory(): systemWrap(), FDs(&systemWrap){}
	SetOfFileDescriptors* get(){
		return &FDs;
	}
};


void set_TEST_GLOBAL_INT_ToInput(int input);
int TEST_GLOBAL_INT;

void set_TEST_GLOBAL_INT_ToInput(int input){
	TEST_GLOBAL_INT=input;
}



*/
/*void test_RemoveFD(){
	FDsTestFactory FD_fact;
	SetOfFileDescriptors * FDs = FD_fact.get();
	FDs->addFD(1);
	TEST_GLOBAL_INT = 4;
	FDs->addCloseFDCallback(&set_TEST_GLOBAL_INT_ToInput); //set_TEST_GLOBAL_INT_ToInput should be called with the FD (1) as its input when the FD is closed

	FDs->removeFD(1);

	if(TEST_GLOBAL_INT != 1){
		TEST_PRINT(redTestText("closeCallback not called"));
		throw 1;
	}
	if(FDs->isFDOpen(1)){
		TEST_PRINT(redTestText("File Descriptor was not removed from list"));
		throw 1;
	}
}

void test_TellServerAboutNewConnection(){

}

void test_GetAndLockFD(){

}*//*

Ensure(SetOfFileDescriptorsTest, calls_callback_when_FD_is_removed) {
	FDsTestFactory FD_fact;
	SetOfFileDescriptors * FDs = FD_fact.get();
	FDs->addFD(1);
	TEST_GLOBAL_INT = 4;
	FDs->addCloseFDCallback(&set_TEST_GLOBAL_INT_ToInput); //set_TEST_GLOBAL_INT_ToInput should be called with the FD (1) as its input when the FD is closed

	FDs->removeFD(1);

	assert_that(TEST_GLOBAL_INT, is_equal_to(1));// check to see that TEST_GLOBAL_INT was changed from 4 to 1 - indicating that the function was called
}

Ensure(SetOfFileDescriptorsTest, FD_is_removed_from_list) {
	FDsTestFactory FD_fact;
	SetOfFileDescriptors * FDs = FD_fact.get();
	FDs->addFD(1);

	FDs->removeFD(1);

	assert_that(FDs->isFDOpen(1), is_equal_to(false));
}


int test(){
	TestSuite *suite = create_named_test_suite("Set Of File Descriptors");
	add_test_with_context(suite, SetOfFileDescriptorsTest, calls_callback_when_FD_is_removed);
	add_test_with_context(suite, SetOfFileDescriptorsTest, FD_is_removed_from_list);
	return run_test_suite(suite, create_text_reporter());
}

}

int main(){
	return SetOfFileDescriptors_Test::test();
}*/
int main(){return 0;}
