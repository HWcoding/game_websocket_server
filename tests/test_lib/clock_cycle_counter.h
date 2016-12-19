#ifndef TESTS_TEST_LIB_CLOCK_CYCLE_COUNTER_H
#define TESTS_TEST_LIB_CLOCK_CYCLE_COUNTER_H
//#include "tests/test_lib/clock_cycle_counter.h"

#include <cstdint>

namespace profiling {

int64_t countCpuCycles(void (*func)(void), int64_t iterations);

} // profiling

#endif /* TESTS_TEST_LIB_CLOCK_CYCLE_COUNTER_H */
