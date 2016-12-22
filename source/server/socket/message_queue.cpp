#include "source/server/socket/message_queue.h"
#include "source/data_types/socket_message.h"

MessageQueue::MessageQueue(bool *run) : mut(), Queue(), bufferReady(), running(run){}
MessageQueue::~MessageQueue(){
	shutdown(); //sets running to false and unblocks threads waiting on getNextMessage_Blocking()
	std::lock_guard<std::mutex> lck(mut); //wait for threads to finish
}

SocketMessage MessageQueue::getNextMessage_Blocking(){ //blocks thread on empty buffer
	std::unique_lock<std::mutex> lck(mut);
	bufferReady.wait(lck,[this]{
		bool notRunning = !*(this->running);
		bool notEmpty = !(this->Queue.empty());
		if (notRunning || notEmpty)return true;
		else return false;
	});
	try{
		if(*running){
			SocketMessage nextMessage( std::move(Queue.front()) );
			Queue.pop();
			lck.unlock();
			return nextMessage;
		}
	}
	catch(...){
		lck.unlock();
		throw;
	}
	lck.unlock();
	SocketMessage temp;
	return temp;
}

bool MessageQueue::isEmpty(){
	return Queue.empty();
}

SocketMessage MessageQueue::getNextMessage(){
	std::lock_guard<std::mutex> lck(mut);
	if(*running && !Queue.empty()){
		SocketMessage nextMessage( std::move(Queue.front()) );
		Queue.pop();
		return nextMessage;
	}
	else{
		SocketMessage temp;
		return temp;
	}
}

int MessageQueue::pushMessage(SocketMessage &message){
	try{
		std::lock_guard<std::mutex> lck(mut);
		Queue.push( std::move(message) );
		bufferReady.notify_one();
		return 0;
	}
	catch(...){
		//BACKTRACE_PRINT();
		return -1;
	}
}

void MessageQueue::shutdown(){
	*running = false;
	std::lock_guard<std::mutex> lck(mut);
	bufferReady.notify_one(); //unblocks getNextMessage
}
