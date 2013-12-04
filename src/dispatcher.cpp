#include "headers.h"
#include "dispatcher.h"

Dispatcher::Dispatcher() :
		m_threadState(STATE_TERMINATED),
		m_thread(NULL)
{

}

Dispatcher::~Dispatcher()
{
	if(m_thread) {
		delete m_thread;
	}
}

void Dispatcher::start()
{
	m_threadState = STATE_RUNNING;
	m_thread = new boost::thread(boost::bind(&Dispatcher::dispatcherThread, this));
}

void Dispatcher::dispatcherThread()
{
	srand(time(NULL));

	boost::unique_lock<boost::mutex> taskLockUnique(m_taskLock, boost::defer_lock);

	Task* task;

	bool running = true;
	while(running) {

		taskLockUnique.lock();

		if(m_taskList.empty()) {
			m_taskSignal.wait(taskLockUnique);
		}

		if(m_threadState != STATE_RUNNING) {
			running = false;
		}

		if(!m_taskList.empty() && running) {
			task = m_taskList.front();
			m_taskList.pop_front();
		} else {
			task = NULL;
		}

		taskLockUnique.unlock();

		if(task) {
			if(!task->hasExpired()) {
				(*task)();
			}
			delete task;
		}
	}

	m_taskLock.lock();

	flush();
	m_threadState = STATE_TERMINATED;

	m_taskLock.unlock();
}

void Dispatcher::addTask(Task* task, bool pushFront /*= false*/)
{
	bool doSignal = false;

	m_taskLock.lock();

	if(m_threadState == STATE_RUNNING) {
		doSignal = m_taskList.empty();
		if(pushFront) {
			m_taskList.push_front(task);
		} else {
			m_taskList.push_back(task);
		}
	}

	m_taskLock.unlock();

	if(doSignal) {
		m_taskSignal.notify_one();
	}
}

void Dispatcher::flush()
{
	while(!m_taskList.empty()) {
		Task* task = m_taskList.front();
		m_taskList.pop_front();
		if(!task->hasExpired()) {
			(*task)();
		}
		delete task;
	}
}

void Dispatcher::stop()
{
	m_taskLock.lock();
	m_threadState = STATE_TERMINATING;
	m_taskLock.unlock();

	m_taskSignal.notify_one();
	m_thread->join();
}
