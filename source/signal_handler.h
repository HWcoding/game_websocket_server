#ifndef SOURCE_SIGNAL_HANDLER_H_
#define SOURCE_SIGNAL_HANDLER_H_
//#include "source/signal_handler.h"

#include <signal.h>
#include <atomic>

class SignalHandler
{
public:
	static SignalHandler *getSignalHandler(std::atomic<bool> *run = nullptr);
	void SetStopFlag();
	~SignalHandler();
private:
	SignalHandler(std::atomic<bool> *run);
	struct sigaction sigAction {};
	std::atomic<bool> *ptr_run {nullptr};

	SignalHandler& operator=(const SignalHandler&)=delete;
	SignalHandler(const SignalHandler&)=delete;
};

#endif /* SOURCE_SIGNAL_HANDLER_H_ */
