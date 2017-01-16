#include "source/signal_handler.h"
#include <cstring>

namespace {

void RecievedInteruptSignal(int)
{
	SignalHandler *sigHandler = SignalHandler::getSignalHandler();
	if(sigHandler != nullptr)
		sigHandler->SetStopFlag();
}


} //namespace


SignalHandler *SignalHandler::getSignalHandler(std::atomic<bool> *run)
{
	static bool init = false;
	if( ((!init) && run == nullptr) || (run != nullptr && init) ) {
		return nullptr;
	}
	else {
		static SignalHandler handler(run);
		init = true;
		return &handler;
	}
}

void SignalHandler::SetStopFlag(){
	ptr_run->store(false);
}

SignalHandler::SignalHandler(std::atomic<bool> *run)
{
	if(run == nullptr) throw -1;
	ptr_run = run;
	memset( &sigAction, 0, sizeof(sigAction) );
	sigAction.sa_handler = ::RecievedInteruptSignal;
	sigfillset(&sigAction.sa_mask);
	sigaction(SIGINT,&sigAction, nullptr);
}

SignalHandler::~SignalHandler()
{
	memset( &sigAction, 0, sizeof(sigAction) );
	sigAction.sa_handler = SIG_DFL;
	sigfillset(&sigAction.sa_mask);
	sigaction(SIGINT,&sigAction, nullptr);
	ptr_run = nullptr;
}
