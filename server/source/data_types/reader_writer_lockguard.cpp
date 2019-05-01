#include "source/data_types/reader_writer_lockguard.h"

ReaderWriterLockState::ReaderWriterLockState() : mut(), queue_mut(), waiting_queue(), readers(0) {}


Lock::Lock (ReaderWriterLockState &lk, bool _reader) : state(lk), condition(), running(false), locked(false), reader(_reader),
     cond_mut(), reader_lock(lk.mut, std::defer_lock), writer_lock(lk.mut, std::defer_lock) {}

void Lock::notifyNextLock(std::unique_lock<std::mutex> &qlock) {
	if( !state.waiting_queue.empty() ) {
		auto next = state.waiting_queue.front();
		if( next->reader || state.readers == 0 ) {
			notifyLock(next, qlock);
		}
	}
}

void Lock::notifyReaderLock(std::unique_lock<std::mutex> &qlock) {
	if( !state.waiting_queue.empty() ) {
		auto next = state.waiting_queue.front();
		if( next->reader ) {
			notifyLock(next, qlock);
		}
	}
}

void Lock::notifyLock(Lock* lock, std::unique_lock<std::mutex> &qlock) {
	state.waiting_queue.pop_front();
	if(lock->reader) {
		++state.readers;
	}
	std::unique_lock<std::mutex> cond(lock->cond_mut);
	qlock.unlock();
	lock->running.store(true);
	cond.unlock();
	lock->condition.notify_one();
}

Lock::~Lock() = default;

void Lock::lock() {
	std::unique_lock<std::mutex> queue_lock(state.queue_mut);
	if( state.waiting_queue.empty() ) {
		if(reader ) {
			locked = reader_lock.try_lock();
		} else {
			locked = writer_lock.try_lock();
		}
	}

	if( !locked ) {
		std::unique_lock<std::mutex> cond(cond_mut);
		state.waiting_queue.push_back(this);
		queue_lock.unlock();
		condition.wait(cond, [this]{return running.load();});

		queue_lock.lock();
		if( reader ) {
			reader_lock.lock();
			locked = true;
			notifyReaderLock(queue_lock);
		} else {
			writer_lock.lock();
			locked = true;
		}

	} else if( reader ) {
		++state.readers;
	}
}

void Lock::unlock() {
	std::unique_lock<std::mutex> queue_lock(state.queue_mut);
	std::unique_lock<std::mutex> cond(cond_mut);
	if(locked.exchange(false)) {
		running.store(false);
		cond.unlock();
		if(reader) {
			--state.readers;
			reader_lock.unlock();
		} else {
			writer_lock.unlock();
		}
		notifyNextLock(queue_lock);
	}
}


WriterLockGuard::WriterLockGuard(ReaderWriterLockState &lk) : Lock(lk, false) {
	lock();
}

WriterLockGuard::~WriterLockGuard() {
	unlock();
}


ReaderLockGuard::ReaderLockGuard(ReaderWriterLockState &lk): Lock(lk, true) {
	lock();
}

ReaderLockGuard::~ReaderLockGuard() {
	unlock();
}
