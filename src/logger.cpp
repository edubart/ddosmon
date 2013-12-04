#include "headers.h"
#include "logger.h"

Logger Logger::m_instance __attribute__((init_priority(102)));

Logger::Logger() : m_verbosity(Logger::INFO)
{

}

Logger::~Logger()
{
	if(m_logFile.is_open()) {
		m_logFile.close();
	}
}

void Logger::setLogFile(std::string const &logFile)
{
	m_logMutex.lock();

	if(m_logFile.is_open()) {
		m_logFile.close();
	}

	if(!logFile.empty()) {
		bool fileEmpty = true;

		std::ifstream file;
		file.open(logFile.c_str());
		if(file.is_open()) {
			file.seekg(0, std::ios_base::end);
			if(file.tellg() > 1) {
				fileEmpty = false;
			}
			file.close();
		}

		m_logFile.open(logFile.c_str(), std::ios::out | std::ios::app);

		if(!m_logFile.good()) {
			std::cout << "FATAL LOGGER ERROR: Unable to open " + logFile + " for writing!" << std::endl;

			m_logMutex.unlock();
			exit(-1);
		} else if(!fileEmpty) {
			m_logFile << std::endl << std::endl;
			m_logFile.flush();
		}

	}

	m_logMutex.unlock();
}

void Logger::output(const std::string& msg, Level verbosity)
{
	m_logMutex.lock();

    static char const *prefixes[] = { "FATAL ERROR: ", "ERROR: ", "WARNING: ", "", "", "", "" };
    static char const *colors[] = { "\033[01;31m ", "\033[01;31m", "\033[01;33m", "\033[0;32m", "\033[1;32m", "\033[01;34m" };

    if(m_verbosity >= verbosity) {
		if(m_logFile.is_open()) {
			time_t rawtime;
			struct tm* timeinfo;

			time(&rawtime);
			timeinfo = localtime(&rawtime);

			m_logFile << std::setw(2) << std::setfill('0') << timeinfo->tm_mday
					<< "/" << std::setw(2) << std::setfill('0') << timeinfo->tm_mon
					<< " " << std::setw(2) << std::setfill('0') << timeinfo->tm_hour
					<< ":" << std::setw(2) << std::setfill('0') << timeinfo->tm_min
					<< ":" << std::setw(2) << std::setfill('0') << timeinfo->tm_sec
                    << " ";

			m_logFile << prefixes[verbosity] << msg;
			m_logFile.flush();
		}

        if(verbosity != INFO2) {
            std::cout << colors[verbosity] << prefixes[verbosity] << msg << "\033[0m" << std::flush;
        }
	}

	m_logMutex.unlock();

	if(verbosity == FATAL) {
		exit(-1);
	}
}
