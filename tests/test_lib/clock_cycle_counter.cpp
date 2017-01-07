#include "tests/test_lib/clock_cycle_counter.h"

namespace {
	void emptyFunction();
	int64_t getFunctionCycleCount(void (*func)(void), int64_t iterations);
	int64_t getCpuCycles();
}

namespace profiling {

//calculate the minimum number of cpu cycles required to run func
int64_t countCpuCycles(void (*func)(void), int64_t iterations)
{
	volatile int64_t baseCycleCount = ::getFunctionCycleCount(::emptyFunction,iterations);
	volatile int64_t cycleCount = ::getFunctionCycleCount(func,iterations);
	return (cycleCount-baseCycleCount);
}

} // profiling



//defined here to prevent inlining above
namespace {

//runs func through a number of iterations and
//return the quickest time to complete a single call
int64_t getFunctionCycleCount(void (*func)(void), int64_t iterations)
{
	volatile int64_t cycleCount = INT64_MAX;
	getCpuCycles();
	getCpuCycles();

	for(volatile int64_t i = iterations; i!=0; --i) {
		volatile int64_t beginCycleCount = getCpuCycles();
		func();
		volatile int64_t endCycleCount = getCpuCycles();
		endCycleCount = endCycleCount-beginCycleCount;
		if(endCycleCount < cycleCount) cycleCount = endCycleCount;
	}
	return cycleCount;
}

void emptyFunction()
{
	volatile int i = 0;
	(void)i;
	return;
}

int64_t getCpuCycles()
{
	volatile uint64_t rax,rdx,rcx;
	__asm__ volatile ( "rdtscp\n" : "=a" (rax), "=d" (rdx), "=c" (rcx) : : );
	return static_cast<int64_t>( (rdx << 32) | rax );
}

}
