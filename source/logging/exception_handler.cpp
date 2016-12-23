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
void * last_frames[20];
int last_size = 0;
std::string exception_name;
std::string demangle(const char *name);
std::string demangle(const char *name) {
	int status;
	std::unique_ptr<char,void(*)(void*)> realname(abi::__cxa_demangle(name, 0, 0, &status), &std::free);
	return status ? "failed" : &*realname;
}

class mallocCharArraySmartPointer{
public:
	char* pointer;
	mallocCharArraySmartPointer(char * p) : pointer(p) {}

	~mallocCharArraySmartPointer(){
		if( pointer!=NULL) free(pointer);
		pointer = NULL;
	}
	bool isNull(){
		return pointer==NULL;
	}

	mallocCharArraySmartPointer& operator=(const mallocCharArraySmartPointer&)=delete;
	mallocCharArraySmartPointer(const mallocCharArraySmartPointer&)=delete;
};

}

namespace DEBUG_BACKTRACE{

void debugBackTrace(){
	char ** backTraceArray = backtrace_symbols(::last_frames, ::last_size);
	if(backTraceArray != NULL){
		std::string debugBacktrace;
		int i = ::last_size;
		while(--i){
			Dl_info info;
			dladdr(::last_frames[i], &info);
			int status;
			debugBacktrace.append("\t");
			mallocCharArraySmartPointer demangled( abi::__cxa_demangle(info.dli_sname, NULL, 0, &status) );

			if( !demangled.isNull() ){
				if(status == 0) debugBacktrace.append(demangled.pointer);
				else{ debugBacktrace.append(info.dli_sname); }
			}
			else{ debugBacktrace.append(backTraceArray[i]); }
			debugBacktrace.append("\n");
		}
		free(backTraceArray);
		LOG_ERROR("Exception of type "<<exception_name<<" caught. Backtrace: \n"<<debugBacktrace);
	}
}

}

extern "C" {
	void __cxa_throw(void *ex, void *info, void (*dest)(void *))
	{
		::exception_name = ::demangle(reinterpret_cast<const std::type_info*>(info)->name());
		::last_size = backtrace(::last_frames, sizeof ::last_frames/sizeof(void*));

		static void (*const rethrow)(void*,void*,void(*)(void*)) __attribute__ ((noreturn)) = (void (*)(void*,void*,void(*)(void*)))dlsym(RTLD_NEXT, "__cxa_throw");
		rethrow(ex,info,dest);
	}
}
#endif
