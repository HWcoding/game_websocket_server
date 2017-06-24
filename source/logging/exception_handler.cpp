#ifdef DEBUG
#include "source/logging/exception_handler.h"
#include <dlfcn.h>
#include <execinfo.h>
#include <typeinfo>
#include <string>
#include <memory>
#include <cxxabi.h>
#include <cstdlib>
#include <thread>
#include <map>
#include <mutex>

namespace {

const int frameSize = 100;
struct ExceptionInfo
{
	void* last_frames[frameSize];
	int last_size {0};
	std::string exception_name {};
};

std::mutex mut;
std::map<std::thread::id, ExceptionInfo> frames;

std::string demangle(const char *name)
{
	int status;
	std::unique_ptr<char,void(*)(void*)> realname(abi::__cxa_demangle(name, 0, 0, &status), &std::free);
	if(status != 0){
		return std::string("unknown");
	}
	else {
		return std::string( realname.get() );
	}
}

} // namespace


namespace DEBUG_BACKTRACE{

void debugBackTrace()
{
	std::unique_lock<std::mutex> lck(::mut);

	std::thread::id this_id = std::this_thread::get_id();
	if(frames.count(this_id)==0) return; //nothing to print

	std::unique_ptr<char*,void(*)(void*)> backTraceArray( backtrace_symbols(::frames[this_id].last_frames, ::frames[this_id].last_size), &std::free);
	if( backTraceArray.get() != nullptr ){
		std::string debugBacktrace;
		int i = ::frames[this_id].last_size;
		while(--i > 0) { // i == 0 is __cxa_throw
			Dl_info info;
			dladdr(::frames[this_id].last_frames[i], &info);

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
		LOG_ERROR("Exception of type "<<frames[this_id].exception_name<<" caught. Stack trace: \n"<<debugBacktrace);
	}
	frames.erase(this_id);
}

} // DEBUG_BACKTRACE


extern "C" {

// Imposter __cxa_throw function. Called when an exception is thrown
void __cxa_throw(void *ex, void* info, void (*dest)(void *))
{
	//save exception name and backtrace in globals
	{ //lock
	std::unique_lock<std::mutex> lck(::mut);
	std::thread::id this_id = std::this_thread::get_id();
	::frames[this_id].exception_name = ::demangle(reinterpret_cast<const std::type_info*>(info)->name());
	::frames[this_id].last_size = backtrace(::frames[this_id].last_frames, ::frameSize);
	} //unlock

	// get a pointer to the real __cxa_throw function
	static void (*const real_cxa_throw)(void*,void*,void(*)(void*)) __attribute__ ((noreturn)) = (void (*)(void*,void*,void(*)(void*)))dlsym(RTLD_NEXT, "__cxa_throw");
	// call the real __cxa_throw so it can do default behavior
	real_cxa_throw(ex, info, dest);
}

} // extern "C"

#endif
