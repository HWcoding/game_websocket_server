#ifndef SOURCE_LOGGING_EXCEPTION_HANDLER_H
#define SOURCE_LOGGING_EXCEPTION_HANDLER_H
//#include "source/logging/exception_handler.h"

#include "source/logging/logger.h"
#include <typeinfo>
#include <exception>

#ifdef DEBUG

namespace DEBUG_BACKTRACE{
void debugBackTrace();
}

extern "C" {
	void __cxa_throw(void *ex, void* info, void (*dest)(void *)) __attribute__ ((noreturn));
}


#define BACKTRACE_PRINT()	LOG_ERROR("Caught Exception on line: "+std::to_string(__LINE__)+" in file: "+__FILE__+" in function: "+__PRETTY_FUNCTION__), DEBUG_BACKTRACE::debugBackTrace()

#else
#define BACKTRACE_PRINT() IM_AN_EMPTY_STATEMENT
#endif

#endif /* SOURCE_LOGGING_EXCEPTION_HANDLER_H */
