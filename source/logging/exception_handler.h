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
	void __cxa_throw(void *ex, void *info, void (*dest)(void *));
}

#define BACKTRACE_PRINT()	(PRINT_ERROR("","Caught Exception in file: " __FILE__<<" in function: "<<__PRETTY_FUNCTION__<<" line: "<<__LINE__));((DEBUG_BACKTRACE::debugBackTrace()), (void)0)

#else
#define BACKTRACE_PRINT() IM_AN_EMPTY_STATEMENT
#endif


#define throwRuntime(type, text)	((PRINT_ERROR("ERROR ","errno: "<<std::strerror(errno)<<" in file: " __FILE__<<" in function: "<<__PRETTY_FUNCTION__<<" line: "<<__LINE__)), (throw type(text)),(void)0)
#define throwInt(text)	((PRINT_ERROR("ERROR ","errno: "<<text<<" in file: " __FILE__<<" in function: "<<__PRETTY_FUNCTION__<<" line: "<<__LINE__)), (throw -1),(void)0)

#endif /* SOURCE_LOGGING_EXCEPTION_HANDLER_H */
