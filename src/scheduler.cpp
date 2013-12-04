#include "headers.h"
#include "scheduler.h"

Scheduler::Scheduler() :
		m_lastEventId(0),
		m_threadState(STATE_TERMINATED),
		m_thread(NULL)
{

}

Scheduler::~Scheduler()
{
	if(m_thread) {
		delete m_thread;
	}
}

void Scheduler::start()
{
	m_threadState = STATE_RUNNING;
	m_thread = new boost::thread(boost::bind(&Scheduler::schedulerThread, this));
}

void Scheduler::schedulerThread()
{
	srand(time(NULL));

	Dispatcher *dispatcher = Dispatcher::instance();

	boost::unique_lock<boost::mutex> eventLockUnique(m_eventLock, boost::defer_lock);

	while(m_threadState != STATE_TERMINATED) {
		SchedulerTask* task = NULL;
		bool runTask = false;
		bool ret = true;

		eventLockUnique.lock();

		if(m_eventList.empty()) {
			m_eventSignal.wait(eventLockUnique);
		} else {
			ret = m_eventSignal.timed_wait(eventLockUnique, m_eventList.top()->getCycle());
		}

		if(ret == false && (m_threadState != STATE_TERMINATED)) {
			task = m_eventList.top();
			m_eventList.pop();

			EventIdSet::iterator it = m_eventIds.find(task->getEventId());
			if(it != m_eventIds.end()) {
				runTask = true;
				m_eventIds.erase(it);
			}
		}

		eventLockUnique.unlock();

		if(task) {
			if(runTask) {
				task->setDontExpire();
				dispatcher->addTask(task);
			} else {
				delete task;
			}
		}
	}
}

uint32_t Scheduler::addEvent(SchedulerTask* task)
{
	bool do_signal = false;

	m_eventLock.lock();

	if(Scheduler::m_threadState == Scheduler::STATE_RUNNING) {
		if(task->getEventId() == 0) {
			if(m_lastEventId >= 0xFFFFFFFF) {
				m_lastEventId = 0;
			}
			task->setEventId(++m_lastEventId);
		}
		m_eventIds.insert(task->getEventId());
		m_eventList.push(task);
		do_signal = (task == m_eventList.top());
	}

	m_eventLock.unlock();

	if(do_signal)
		m_eventSignal.notify_one();

	return task->getEventId();
}


bool Scheduler::stopEvent(uint32_t eventid)
{
	if(eventid == 0) {
		return false;
	}

	m_eventLock.lock();

	EventIdSet::iterator it = m_eventIds.find(eventid);
	if(it != m_eventIds.end()) {
		m_eventIds.erase(it);
		m_eventLock.unlock();
		return true;
	} else {
		m_eventLock.unlock();
		return false;
	}
}

void Scheduler::stop()
{
	m_eventLock.lock();

	m_threadState = Scheduler::STATE_TERMINATED;

	while(!m_eventList.empty()) {
		delete m_eventList.top();
		m_eventList.pop();
	}
	m_eventIds.clear();

	m_eventLock.unlock();

	m_eventSignal.notify_one();

	m_thread->join();
}
