#define TEST_FILE_LINK_DEPENDENCIES "source/data_types/reader_writer_lockguard.cpp"

#include "source/data_types/reader_writer_lockguard.h"

#include "tests/test.h"
#include <vector>
#include <thread>

class ThreadWrapper
{
public:
	template< class Function, class... Args >
	explicit ThreadWrapper( Function f, Args&&... args ): Thread(f, args...){}

	~ThreadWrapper()
	{
		if(Thread.joinable()) {
			Thread.join(); //wait for thread to finish returning
		}
	}

	// move
	ThreadWrapper(ThreadWrapper&& other): Thread(std::move(other.Thread)) {}
private:
	std::thread Thread {};

};

void addFirst(int &num, int &num2, ReaderWriterLockState &lockState) {
	ReaderLockGuard rlk(lockState);
	int orig = num;
	rlk.unlock();
	WriterLockGuard wlk(lockState);
	num2 += orig;
	wlk.unlock();
	rlk.lock();
	orig = num;
	rlk.unlock();
	wlk.lock();
	num2 -= orig;
}

void subFirst(int &num, int &num2, ReaderWriterLockState &lockState) {
	ReaderLockGuard rlk(lockState);
	int orig = num;
	rlk.unlock();
	WriterLockGuard wlk(lockState);
	num2 -= orig;
	wlk.unlock();
	rlk.lock();
	orig = num;
	rlk.unlock();
	wlk.lock();
	num2 += orig;
}



TEST(ReaderWriterLock, addAndSubIsZero) {
	ReaderWriterLockState lockState;

	int inc = 2;
	int total = 0;

	{
		std::vector<ThreadWrapper> test;
		for(int i = 0; i < 200; i++) {
			test.emplace_back(addFirst, std::ref(inc), std::ref(total), std::ref(lockState));
		}
		for(int i = 0; i < 200; i++) {
			test.emplace_back(subFirst, std::ref(inc), std::ref(total), std::ref(lockState));
		}
		for(int i = 0; i < 200; i++) {
			test.emplace_back(addFirst, std::ref(inc), std::ref(total), std::ref(lockState));
		}
		for(int i = 0; i < 200; i++) {
			test.emplace_back(subFirst, std::ref(inc), std::ref(total), std::ref(lockState));
		}

	}
	EXPECT_EQ(total, 0);



}
int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
