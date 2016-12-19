#ifndef TESTS_TEST_H
#define TESTS_TEST_H
//#include "tests/test.h"

#include "gtest/gtest.h"

#define STAY_SILENT_ON_SUCCESS StaySilentOnSuccess STAY_SILENT_ON_SUCESS_OBJECT

class StaySilentOnSuccess
{
private:
	class SilentOnSuccessListener : public testing::TestEventListener
	{

	protected:
		testing::TestEventListener* testEventListener;
	public:
		explicit SilentOnSuccessListener(testing::TestEventListener* listener) :
			testEventListener(listener)
		{}

		void OnEnvironmentsSetUpStart(const testing::UnitTest&) {}
		void OnEnvironmentsSetUpEnd(const testing::UnitTest&) {}
		void OnTestCaseStart(const testing::TestCase&) {}
		void OnTestStart(const testing::TestInfo&) {}
		void OnTestCaseEnd(const testing::TestCase& testCase) {
			//testEventListener->OnTestCaseEnd(testCase);
		}
		void OnEnvironmentsTearDownStart(const testing::UnitTest&) {}
		void OnEnvironmentsTearDownEnd(const testing::UnitTest&) {}
		void OnTestIterationEnd(const testing::UnitTest& unitTest, int iter) {
			//testEventListener->OnTestIterationEnd(unitTest, iter);
		}
		void OnTestProgramEnd(const testing::UnitTest&) {}
		void OnTestProgramStart(const testing::UnitTest&) {}
		void OnTestIterationStart(const testing::UnitTest&, int) {}

		void OnTestPartResult(const testing::TestPartResult& result)
		{
			std::cout<<".........................................."\
			                   "...................................."<<std::endl;

			std::cout<<"\033[31m                                   FAILED\033[0m"<<std::endl;

			std::cout<<"\033[36m"<<result.file_name()<<"\033[0m"<<": line: "\
			                                 <<result.line_number()<<std::endl;

			std::cout<<result.message()<<std::endl;
		}

		void OnTestEnd(const testing::TestInfo& testInfo)
		{
			if(testInfo.result()->Failed()) {
				testEventListener->OnTestEnd(testInfo);
				std::cout<<"\033[31m======================================="\
				    "=======================================\033[0m\n\n"<<std::endl;
			}


		}

		~SilentOnSuccessListener()
		{
			delete testEventListener;
		}
	};

	testing::TestEventListeners& listeners;
public:
	StaySilentOnSuccess() : listeners(testing::UnitTest::GetInstance()->listeners())
	{
		auto listener = listeners.Release(listeners.default_result_printer());
		listeners.Append(new SilentOnSuccessListener(listener));
	}
};








#endif /* TESTS_TEST_H */
