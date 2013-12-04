#ifndef LOGGER_H
#define LOGGER_H

#include "headers.h"

// xml helpers
bool readXMLInteger(xmlNodePtr node, const char* tag, int& value);
bool readXMLInteger64(xmlNodePtr node, const char* tag, uint64_t& value);
bool readXMLFloat(xmlNodePtr node, const char* tag, float& value);
bool readXMLString(xmlNodePtr node, const char* tag, std::string& value);

// time helpers
inline int64_t getSystemTime() {
	struct timeval t;
	gettimeofday(&t, NULL);
    return ((int64_t)t.tv_sec) * 1000 + (int64_t)(t.tv_usec/1000);
}

std::string unixTimeToDateString(time_t unixtime);
std::string unixTimeToTimeString(time_t unixtime);
std::string unixTimeToString(time_t unixtime);

inline std::string getCurrentTimeString() {	return unixTimeToTimeString(time(NULL)); }
inline std::string getCurrentDateString() {	return unixTimeToDateString(time(NULL)); }
inline std::string getCurrentDateTimeString() {	return unixTimeToString(time(NULL)); }

// net tools
std::string ipToString(uint32_t ip);

inline uint32_t bytesToKBits(uint32_t bytes) {
    return (bytes*8)/1000;
}


#endif // LOGGER_H
