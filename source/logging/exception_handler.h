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


#define BACKTRACE_PRINT()	LOG_ERROR("Caught Exception on line: "<<__LINE__<<" in file: " __FILE__<<" in function: "<<__PRETTY_FUNCTION__) || true ? DEBUG_BACKTRACE::debugBackTrace() : (void)0

#else
#define BACKTRACE_PRINT() IM_AN_EMPTY_STATEMENT
#endif


#define throwRuntime(type, text)	LOG_ERROR("errno: "<<std::strerror(errno)<<" in file: " __FILE__<<" in function: "<<__PRETTY_FUNCTION__<<" line: "<<__LINE__) || true ? throw type(text) : (void)0
#define throwInt(text)	LOG_EXCEPTION(text) || true ? throw -1 : 0

#endif /* SOURCE_LOGGING_EXCEPTION_HANDLER_H */
