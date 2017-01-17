#ifdef DEBUG
#include "source/logging/exception_handler.h"
#include <dlfcn.h>
#include <execinfo.h>
#include <typeinfo>
#include <string>
#include <memory>
#include <cxxabi.h>
#include <cstdlib>

namespace {

const int frameSize = 100;
void * last_frames[frameSize];
int last_size = 0;
std::string exception_name;
std::string demangle(const char *name)
{
	int status;
	std::unique_ptr<char,void(*)(void*)> realname(abi::__cxa_demangle(name, 0, 0, &status), &std::free);
	return status ? std::string("failed") : std::string( realname.get() );
}

} // namespace


namespace DEBUG_BACKTRACE{

void debugBackTrace()
{
	std::unique_ptr<char*,void(*)(void*)> backTraceArray( backtrace_symbols(::last_frames, ::last_size), &std::free);
	if( backTraceArray.get() != nullptr ){
		std::string debugBacktrace;
		int i = ::last_size;
		while(--i > 0) { // i == 0 is __cxa_throw
			Dl_info info;
			dladdr(::last_frames[i], &info);

			int status;
			std::unique_ptr<char,void(*)(void*)> demangled( abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status), &std::free);

			std::string functionInfo("\t");

			if( demangled.get() != nullptr ){
				if(status == 0) functionInfo.append( demangled.get() );
				else{ functionInfo.append(info.dli_sname); }
			}
			else{ functionInfo.append(backTraceArray.get()[i]); }
			functionInfo.append("\n");
			debugBacktrace.insert(0,functionInfo);

		}
		LOG_ERROR("Exception of type "<<exception_name<<" caught. Stack trace: \n"<<debugBacktrace);
	}
}

} // DEBUG_BACKTRACE


extern "C" {

// Imposter __cxa_throw function. Called when an exception is thrown
void __cxa_throw(void *ex, void *info, void (*dest)(void *))
{
	//save exception name and backtrace in globals
	//TODO: there is a race condition if a second thread
	//throws before DEBUG_BACKTRACE::debugBackTrace() completes
	::exception_name = ::demangle(reinterpret_cast<const std::type_info*>(info)->name());
	::last_size = backtrace(::last_frames, ::frameSize);

	// get a pointer to the real __cxa_throw function
	static void (*const real_cxa_throw)(void*,void*,void(*)(void*)) __attribute__ ((noreturn)) = (void (*)(void*,void*,void(*)(void*)))dlsym(RTLD_NEXT, "__cxa_throw");
	// call the function so it can do default behavior
	real_cxa_throw(ex,info,dest);
}

} // extern "C"

#endif
