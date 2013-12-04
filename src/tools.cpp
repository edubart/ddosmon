#include "headers.h"
#include "tools.h"

bool readXMLInteger(xmlNodePtr node, const char* tag, int& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(nodeValue) {
		value = atoi(nodeValue);
		xmlFree(nodeValue);
		return true;
	}

	return false;
}

bool readXMLInteger64(xmlNodePtr node, const char* tag, uint64_t& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(nodeValue) {
		value = atoll(nodeValue);
		xmlFree(nodeValue);
		return true;
	}

	return false;
}

bool readXMLFloat(xmlNodePtr node, const char* tag, float& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(nodeValue) {
		value = atof(nodeValue);
		xmlFree(nodeValue);
		return true;
	}

	return false;
}

bool readXMLString(xmlNodePtr node, const char* tag, std::string& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(nodeValue) {
		value = nodeValue;
		xmlFree(nodeValue);
		return true;
	}

	return false;
}

std::string unixTimeToDateString(time_t unixtime)
{
    static const char *year[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    struct tm *local;
    std::stringstream ss;

    local = localtime(&unixtime);

    if(local) {
        ss << local->tm_mday << " " << year[local->tm_mon] << " " << (local->tm_year + 1900);
    } else {
        LOG_ERROR("Time conversion for unixtime " << unixtime << " failed.");
        ss << "UNIX time: " << unixtime;
    }

    return ss.str();
}

std::string unixTimeToTimeString(time_t unixtime)
{
    struct tm *local;
    std::stringstream ss;

    local = localtime(&unixtime);

    if(local) {
        ss << std::setw(2) << std::setfill('0') << local->tm_hour
            << ":" << std::setw(2) << std::setfill('0') << local->tm_min
            << ":" << std::setw(2) << std::setfill('0') << local->tm_sec;
    } else {
        LOG_ERROR("Time conversion for unixtime " << unixtime << " failed.");
        ss << "UNIX time: " << unixtime;
    }

    return ss.str();
}

std::string unixTimeToString(time_t unixtime)
{
    struct tm *local;
    std::stringstream ss;

    local = localtime(&unixtime);

    if(local) {
        ss << std::setw(2) << std::setfill('0') << local->tm_mday
            << "/" << std::setw(2) << std::setfill('0') << local->tm_mon
            << "/" << local->tm_year + 1900
            << " " << std::setw(2) << std::setfill('0') << local->tm_hour
            << ":" << std::setw(2) << std::setfill('0') << local->tm_min
            << ":" << std::setw(2) << std::setfill('0') << local->tm_sec;
    } else {
        LOG_ERROR("Time conversion for unixtime " << unixtime << " failed.");
        ss << "UNIX time: " << unixtime;
    }

    return ss.str();
}

std::string ipToString(uint32_t ip)
{
    std::string ret;
    ret = inet_ntoa(*(struct in_addr*)&ip);
    return ret;
}
