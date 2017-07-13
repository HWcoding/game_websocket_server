#ifndef SOURCE_LOGGING_EXCEPTION_HANDLER_H
#define SOURCE_LOGGING_EXCEPTION_HANDLER_H
//#include "source/logging/exception_handler.h"

#include "source/logging/logger.h"
#include <typeinfo>

#ifdef DEBUG

namespace DEBUG_BACKTRACE{
void debugBackTrace();
}

extern "C" {
	void __cxa_throw(void *ex, void* info, void (*dest)(void *)) __attribute__ ((noreturn));
}


#define BACKTRACE_PRINT()	LOG_ERROR("Caught Exception on line: "<<__LINE__<<" in file: " __FILE__<<" in function: "<<__PRETTY_FUNCTION__), DEBUG_BACKTRACE::debugBackTrace()

#else
#define BACKTRACE_PRINT() IM_AN_EMPTY_STATEMENT
#endif


#define throwRuntime(type, text)	LOG_ERROR("errno: "<<std::strerror(errno)<<" in file: " __FILE__<<" in function: "<<__PRETTY_FUNCTION__<<" line: "<<__LINE__), throw type(text)
#define throwInt(text)	LOG_EXCEPTION(text), throw -1

#endif /* SOURCE_LOGGING_EXCEPTION_HANDLER_H */
