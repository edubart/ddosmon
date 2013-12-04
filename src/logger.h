#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "headers.h"

class Logger
{
public:
	enum Level {
		FATAL = 0,
		ERROR,
		WARN,
        INFO,
        INFO2,
        DEBUG
	};

	Logger();
	~Logger();

	void setLogFile(const std::string &logFile);
	void setVerbosity(Level verbosity) { m_verbosity = verbosity; }

	bool isLogFileSet() const { return m_logFile.is_open();	}

    void output(std::string const &msg, Level verbosity);

	static Logger* instance() {
		return &m_instance;
	}

private:
	Level m_verbosity;

	std::ofstream m_logFile;
	boost::mutex m_logMutex;

	static Logger m_instance;
};

#define _LOG(level, msg)                                                          \
{                                                                                 \
		std::ostringstream os;                                                    \
		os << msg << "\n";                                                        \
		Logger::instance()->output(os.str(), Logger::level);                      \
}

#define LOG_INFO(msg)   _LOG(INFO, msg)
#define LOG_INFO2(msg)   _LOG(INFO2, msg)
#define LOG_DEBUG(msg)  _LOG(DEBUG, msg)
#define LOG_WARN(msg)   _LOG(WARN, msg)
#define LOG_ERROR(msg)  _LOG(ERROR, msg)
#define LOG_FATAL(msg)  _LOG(FATAL, msg)

#endif
