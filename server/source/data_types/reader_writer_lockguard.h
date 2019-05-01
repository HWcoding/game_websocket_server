#ifndef SOURCE_DATA_TYPES_READER_WRITER_LOCKGUARD_H
#define SOURCE_DATA_TYPES_READER_WRITER_LOCKGUARD_H
//#include "source/data_types/reader_writer_lockguard.h"

#include <shared_mutex>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <atomic>


/**
 * Allows multiple readers to lock the resource simultaneously. Writers lock
 * the mutex exclusively. Maintains fairness even when a large number of reader
 * and writer threads try to access the resource at the same time by using a
 * queue to order locks in the order they were requested.
 */

class Lock;

/**
 * holds common state shared by WriterLockGuard and ReaderLockGuard
 */
class ReaderWriterLockState {
	friend class Lock;
public:
	ReaderWriterLockState();
private:
	std::shared_mutex mut;
	std::mutex queue_mut;
	std::deque<Lock*> waiting_queue;
	std::atomic<int> readers;
};

/**
 * A generic lock that can be a reader or writer lock depending on the value of reader
 */
class Lock {
protected:
	ReaderWriterLockState &state;

	std::condition_variable condition;
	std::atomic<bool> running;
	std::atomic<bool> locked;
	bool reader;
	std::mutex cond_mut;

	std::shared_lock<std::shared_mutex> reader_lock;
	std::unique_lock<std::shared_mutex> writer_lock;

	Lock (ReaderWriterLockState &lk, bool _reader);
	void notifyNextLock(std::unique_lock<std::mutex> &qlock);
	void notifyReaderLock(std::unique_lock<std::mutex> &qlock);
	void notifyLock(Lock* lock, std::unique_lock<std::mutex> &qlock);
public:
	void lock();
	void unlock();
	virtual ~Lock();
};

/**
 * A Writer lock that locks itself in the constructor and unlocks itself in the
 * destructor. It can also be unlocked or locked using the lock() and unlock()
 * functions of the parent class. The destructor only unlocks the lock if it
 * was locked at the time of destruction.
 *
 * The same ReaderWriterLockState must be used for all locks protecting the
 * same resource.
 */
class WriterLockGuard : public Lock {
public:
	WriterLockGuard(ReaderWriterLockState &lk);
	~WriterLockGuard();

private:
	WriterLockGuard(const WriterLockGuard& ) = delete; // copy
	WriterLockGuard(WriterLockGuard&& ) = delete; // move
	WriterLockGuard& operator=(const WriterLockGuard ) = delete; // assignment
	WriterLockGuard& operator=(WriterLockGuard&& ) = delete; // move assignment
};

/**
 * A Reader lock that locks itself in the constructor and unlocks itself in the
 * destructor. It can also be unlocked or locked using the lock() and unlock()
 * functions of the parent class. The destructor only unlocks the lock if it
 * was locked at the time of destruction. Multiple reader locks can be locked
 * at the same time.
 *
 * The same ReaderWriterLockState must be used for all locks protecting the
 * same resource.
 */
class ReaderLockGuard : public Lock {
public:
	ReaderLockGuard(ReaderWriterLockState &lk);
	~ReaderLockGuard();

private:
	ReaderLockGuard(const ReaderLockGuard& ) = delete; // copy
	ReaderLockGuard(ReaderLockGuard&& ) = delete; // move
	ReaderLockGuard& operator=(const ReaderLockGuard ) = delete; // assignment
	ReaderLockGuard& operator=(ReaderLockGuard&& ) = delete; // move assignment
};


#endif /* SOURCE_DATA_TYPES_READER_WRITER_LOCKGUARD_H */