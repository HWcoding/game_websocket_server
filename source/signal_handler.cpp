#include "source/signal_handler.h"
#include "source/logging/exception_handler.h"
#include <cstring>

namespace {

void RecievedInteruptSignal(int)
{
	SignalHandler *sigHandler = nullptr;
	try {
		sigHandler = SignalHandler::getSignalHandler();
		sigHandler->SetStopFlag();
	}
	catch(...){ abort(); }
}

} //namespace


//statics
SignalHandler * SignalHandler::getsetSignalHandler(std::atomic<bool> *run)
{
	static bool init = false;
	if( init == false && run == nullptr) {
		throwInt("SignalHandler is not initialized and run is null");
		return nullptr;
	}
	else if(run != nullptr && init != false) {
		throwInt("non-null run used when SignalHandler is already initialized");
		return nullptr;
	}
	else {
		static SignalHandler handler(run);
		init = true;
		return &handler;
	}
}

SignalHandler * SignalHandler::getSignalHandler()
{
	return getsetSignalHandler(nullptr);
}

SignalHandler * SignalHandler::setSignalHandler(std::atomic<bool> *run)
{
	return getsetSignalHandler(run);
}

SignalHandler * SignalHandler::resetSignalHandler(std::atomic<bool> *run)
{
	SignalHandler *sigHandler = SignalHandler::getSignalHandler();
	sigHandler->revertToDefault();
	sigHandler->initialize(run);
	return sigHandler;
}



//non statics
void SignalHandler::SetStopFlag()
{
	ptr_run->store(false);
}

void SignalHandler::initialize(std::atomic<bool> *run)
{
	if(run == nullptr) throwInt("null run used to initialize SignalHandler");
	ptr_run = run;
	memset( &sigAction, 0, sizeof(sigAction) );
	sigAction.sa_handler = ::RecievedInteruptSignal;
	sigfillset(&sigAction.sa_mask);
	sigaction(SIGINT,&sigAction, nullptr);
}

SignalHandler::SignalHandler(std::atomic<bool> *run)
{
	initialize(run);
}

void SignalHandler::revertToDefault()
{
	memset( &sigAction, 0, sizeof(sigAction) );
	sigAction.sa_handler = SIG_DFL;
	sigfillset(&sigAction.sa_mask);
	sigaction(SIGINT,&sigAction, nullptr);
	ptr_run = nullptr;
}

SignalHandler::~SignalHandler()
{
	revertToDefault();
}
