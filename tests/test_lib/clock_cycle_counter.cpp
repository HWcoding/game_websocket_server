#include "tests/test_lib/clock_cycle_counter.h"


namespace {
	void emptyFunction();
	int64_t runFunctionTest(void (*func)(void), int64_t iterations);
	int64_t getCpuCycles();
}

namespace profiling{

int64_t cpuCycleTest(void (*func)(void), int64_t iterations)
{
	int64_t rdtscpTime = ::runFunctionTest(::emptyFunction,iterations);
	int64_t cycleCount = ::runFunctionTest(func,iterations);
	return (cycleCount-rdtscpTime)/iterations;
}

} // profiling



//defined here to prevent inlining above
namespace {

int64_t runFunctionTest(void (*func)(void), int64_t iterations)
{
	int count = 100;
	int64_t cycleCount = INT64_MAX;
	getCpuCycles();
	getCpuCycles();
	while(--count) {
		volatile int64_t beginCycleCount = getCpuCycles();
		for(int64_t i = iterations; i!=0; i--){
			func();
		}
		volatile int64_t endCycleCount = getCpuCycles();
		endCycleCount = endCycleCount-beginCycleCount;
		if(endCycleCount < cycleCount) cycleCount = endCycleCount;
	}
	return cycleCount;
}

void emptyFunction(){return;}

//defined here to prevent inlining above
int64_t getCpuCycles()
{
	uint64_t rax,rdx,rcx;
	__asm__ volatile ( "rdtscp\n" : "=a" (rax), "=d" (rdx), "=c" (rcx) : : );
	return static_cast<int64_t>( (rdx << 32) | rax );
}

}
