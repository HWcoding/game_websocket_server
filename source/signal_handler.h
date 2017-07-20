#ifndef SOURCE_SIGNAL_HANDLER_H_
#define SOURCE_SIGNAL_HANDLER_H_
//#include "source/signal_handler.h"

#include <signal.h>
#include <atomic>

class SignalHandler
{
public:
	static void setSignalHandler(std::atomic<bool> *run);
	~SignalHandler();

private:
	static void recievedInteruptSignal(int);
	void SetStopFlag();
	void initialize(std::atomic<bool> *run);
	void revertToDefault();

	SignalHandler() = default;
	struct sigaction sigAction {};
	std::atomic<bool> *ptr_run {nullptr};

	SignalHandler& operator=(const SignalHandler& s) = delete;
	SignalHandler(const SignalHandler&) = delete;

	static SignalHandler handler;
};

#endif /* SOURCE_SIGNAL_HANDLER_H_ */
