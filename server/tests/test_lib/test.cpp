#include "tests/test.h"


class SilentOnSuccessListener : public testing::TestEventListener
{

protected:
	testing::TestEventListener* resultPrinter;
public:
	explicit SilentOnSuccessListener(testing::TestEventListener* _resultPrinter) :
		resultPrinter(_resultPrinter)
	{}
	SilentOnSuccessListener(const SilentOnSuccessListener&) = delete;
	SilentOnSuccessListener& operator=(const SilentOnSuccessListener&) = delete;

	void OnEnvironmentsSetUpStart(const testing::UnitTest&) {}
	void OnEnvironmentsSetUpEnd(const testing::UnitTest&) {}
	void OnTestCaseStart(const testing::TestCase&) {}
	void OnTestStart(const testing::TestInfo&) {}
	void OnTestCaseEnd(const testing::TestCase&) {
		//resultPrinter->OnTestCaseEnd(testCase);
	}
	void OnEnvironmentsTearDownStart(const testing::UnitTest&) {}
	void OnEnvironmentsTearDownEnd(const testing::UnitTest&) {}
	void OnTestIterationEnd(const testing::UnitTest&, int) {
		//resultPrinter->OnTestIterationEnd(unitTest, iter);
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
			resultPrinter->OnTestEnd(testInfo);
			std::cout<<"\033[31m======================================="\
			    "=======================================\033[0m\n\n"<<std::endl;
		}
	}

	~SilentOnSuccessListener()
	{
		delete resultPrinter;
	}
};


void StaySilentOnSuccess() {
	testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
	auto resultPrinter = listeners.Release(listeners.default_result_printer());
	listeners.Append(new SilentOnSuccessListener(resultPrinter));
}