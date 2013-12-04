#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

#include "headers.h"

const int DISPATCHER_TASK_EXPIRATION = 2000;

class Task
{
public:
	Task(uint32_t ms, const boost::function<void (void)>& f) : m_f(f) {
		m_expiration = boost::get_system_time() + boost::posix_time::milliseconds(ms);
	}
	Task(const boost::function<void (void)>& f)
		: m_expiration(boost::date_time::not_a_date_time), m_f(f) { }

	~Task() { }

	void operator()() {
		m_f();
	}

	void setDontExpire() {
		m_expiration = boost::date_time::not_a_date_time;
	}

	bool hasExpired() const {
		if(m_expiration == boost::date_time::not_a_date_time)
			return false;
		return m_expiration < boost::get_system_time();
	}

protected:
	boost::system_time m_expiration;
	boost::function<void (void)> m_f;
};

inline Task* createTask(boost::function<void (void)> f) {
	return new Task(f);
}

inline Task* createTask(uint32_t expiration, boost::function<void (void)> f) {
	return new Task(expiration, f);
}

class Dispatcher : public Singleton<Dispatcher>
{
	enum DispatcherState {
		STATE_RUNNING,
		STATE_TERMINATING,
		STATE_TERMINATED
	};

public:
	Dispatcher();
	virtual ~Dispatcher();

	void addTask(Task* task, bool pushFront = false);

	void start();
	void stop();

protected:
	void flush();

	void dispatcherThread();

	DispatcherState m_threadState;
	boost::thread* m_thread;

	boost::mutex m_taskLock;
	boost::condition_variable m_taskSignal;

	std::list<Task*> m_taskList;
};

#endif
