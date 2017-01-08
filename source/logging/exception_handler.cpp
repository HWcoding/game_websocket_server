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
std::string demangle(const char *name);
std::string demangle(const char *name) {
	int status;
	std::unique_ptr<char,void(*)(void*)> realname(abi::__cxa_demangle(name, 0, 0, &status), &std::free);
	return status ? "failed" : &*realname;
}


//RAII wrapper for pointers to memory allocated using malloc
template <class T>
class mallocSmartPointer
{
public:
	T* pointer;
	mallocSmartPointer(T * p) : pointer(p) {}

	~mallocSmartPointer(){
		if( pointer!=nullptr) free(pointer);
		pointer = nullptr;
	}
	bool isNull(){
		return pointer==nullptr;
	}

	mallocSmartPointer& operator=(const mallocSmartPointer&)=delete;
	mallocSmartPointer(const mallocSmartPointer&)=delete;
};

}


namespace DEBUG_BACKTRACE{

void debugBackTrace()
{
	mallocSmartPointer<char*> backTraceArray( backtrace_symbols(::last_frames, ::last_size) );
	if( ! backTraceArray.isNull() ){
		std::string debugBacktrace;
		int i = ::last_size;
		while(--i > 0){
			Dl_info info;
			std::string functionInfo;
			dladdr(::last_frames[i], &info);
			int status;
			functionInfo.append("\t");

			mallocSmartPointer<char> demangled( abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status) );

			if( !demangled.isNull() ){
				if(status == 0) functionInfo.append(demangled.pointer);
				else{ functionInfo.append(info.dli_sname); }
			}
			else{ functionInfo.append(backTraceArray.pointer[i]); }
			functionInfo.append("\n");
			debugBacktrace.insert(0,functionInfo);

		}
		LOG_ERROR("Exception of type "<<exception_name<<" caught. Stack trace: \n"<<debugBacktrace);
	}
}

}


extern "C" {

void __cxa_throw(void *ex, void *info, void (*dest)(void *))
{
	if(::last_size <= ::frameSize) {
		::exception_name = ::demangle(reinterpret_cast<const std::type_info*>(info)->name());
		::last_size = backtrace(::last_frames, sizeof ::last_frames/sizeof(void*));
	}

	static void (*const rethrow)(void*,void*,void(*)(void*)) __attribute__ ((noreturn)) = (void (*)(void*,void*,void(*)(void*)))dlsym(RTLD_NEXT, "__cxa_throw");
	rethrow(ex,info,dest);
}

}






















#endif
