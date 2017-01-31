#ifndef SOURCE_SIGNAL_HANDLER_H_
#define SOURCE_SIGNAL_HANDLER_H_
//#include "source/signal_handler.h"

#include <signal.h>
#include <atomic>

class SignalHandler
{
public:
	static SignalHandler * getSignalHandler();
	static SignalHandler * setSignalHandler(std::atomic<bool> *run);
	static SignalHandler * resetSignalHandler(std::atomic<bool> *run);
	void SetStopFlag();
	~SignalHandler();
private:
	static SignalHandler *getsetSignalHandler(std::atomic<bool> *run = nullptr);
	void initialize(std::atomic<bool> *run);
	void revertToDefault();

	SignalHandler(std::atomic<bool> *run);
	struct sigaction sigAction {};
	std::atomic<bool> *ptr_run {nullptr};

	SignalHandler& operator=(const SignalHandler& s)
	{
		sigAction = s.sigAction;
		ptr_run = s.ptr_run;
		return *this;
	}
	SignalHandler(const SignalHandler&)=delete;
};

#endif /* SOURCE_SIGNAL_HANDLER_H_ */
