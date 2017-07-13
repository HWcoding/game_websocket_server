#include "source/signal_handler.h"
#include "source/logging/exception_handler.h"
#include <cstring>
#include <mutex>


namespace {
std::mutex mut;
}

//statics
std::atomic<bool> SignalHandler::init(false);
SignalHandler SignalHandler::handler;

void SignalHandler::recievedInteruptSignal(int)
{
	std::unique_lock<std::mutex> lck(::mut);
	handler.SetStopFlag();
}

void SignalHandler::setSignalHandler(std::atomic<bool> *run)
{
	std::unique_lock<std::mutex> lck(::mut);
	handler.revertToDefault();
	handler.initialize(run);

}



//non statics

void SignalHandler::SetStopFlag()
{
	ptr_run->store(false);
}

void SignalHandler::initialize(std::atomic<bool> *run)
{
	ptr_run = run;
	memset( &sigAction, 0, sizeof(sigAction) );
	sigAction.sa_handler = recievedInteruptSignal;
	sigfillset(&sigAction.sa_mask);
	sigaction(SIGINT,&sigAction, nullptr);
	init.store(true);
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
