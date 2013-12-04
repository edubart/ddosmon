#include "headers.h"
#include "configmanager.h"

ConfigManager::ConfigManager() : m_isLoaded(false)
{
}

ConfigManager::~ConfigManager()
{

}

void ConfigManager::loadFile()
{
    lua_State* luaHandle = lua_open();

    if(!luaHandle) {
        LOG_FATAL("Could not create lua handle.");
    }

    if(luaL_dofile(luaHandle, m_confString[CONFIG_FILE].c_str())) {
        lua_close(luaHandle);
        LOG_FATAL("Could not load " <<  m_confString[CONFIG_FILE]);
    }

    m_confString[INTERFACE] = getGlobalString(luaHandle, "interface","eth0");
    m_confString[LOG_FILE] = getGlobalString(luaHandle, "log","ddosmon.log");
    m_confString[WATCHEDIPS_XML] = getGlobalString(luaHandle, "watchedips","watchedips.xml");
    m_confString[NOTIFICATION_COMMAND] = getGlobalString(luaHandle, "notification_command","");
    m_confString[ONBLOCKIP_COMMAND] = getGlobalString(luaHandle, "onblockip_command","");
    m_confString[ONUNBLOCKIP_COMMAND] = getGlobalString(luaHandle, "onunblockip_command", "");
    m_confString[ONNETWORK_COMPROMISE_COMMAND] = getGlobalString(luaHandle, "onnetwork_compromise_command","");
    m_confString[ONNETWORK_UNCOMPROMISE_COMMAND] = getGlobalString(luaHandle, "onnetwork_uncompromise_command", "");
    m_confString[NOTIFICATION_SUBJECT] = getGlobalString(luaHandle, "notificationsubject", "DDOS Monitor notification");
    m_confInteger[NOTIFICATION_TRAFFIC_THRESHOLD] = getGlobalNumber(luaHandle, "notification_traffic_threshold", 20000);
    m_confInteger[NOTIFICATION_PACKETS_THRESHOLD] = getGlobalNumber(luaHandle, "notification_packets_threshold", 15000);
    m_confInteger[IP_TRAFFIC_THRESHOLD] = getGlobalNumber(luaHandle, "ip_traffic_threshold", 500000);
    m_confInteger[IP_PACKETS_THRESHOLD] = getGlobalNumber(luaHandle, "ip_packets_threshold", 125000);
    m_confInteger[GLOBAL_TRAFFIC_THRESHOLD] = getGlobalNumber(luaHandle, "global_traffic_threshold", 500000);
    m_confInteger[GLOBAL_PACKETS_THRESHOLD] = getGlobalNumber(luaHandle, "global_packets_threshold", 125000);
    m_confInteger[IPBLOCK_RETRY_TICKS] = getGlobalNumber(luaHandle, "ipblock_retry_ticks", 14400000);
    m_confInteger[NETWORK_UNCOMPROMISE_TICKS] = getGlobalNumber(luaHandle, "network_uncompromise_ticks", 30);
    m_isLoaded = true;

    lua_close(luaHandle);
}

const std::string& ConfigManager::getString(uint32_t _what) const
{
    if(m_isLoaded && _what < LAST_STRING_CONFIG)
        return m_confString[_what];
    else {
        LOG_WARN("Failed to read the configuration" << _what);
        static std::string dummyStr = "";
        return dummyStr;
    }
}

int64_t ConfigManager::getNumber(uint32_t _what) const
{
    if(m_isLoaded && _what < LAST_INTEGER_CONFIG)
        return m_confInteger[_what];
    else {
        LOG_WARN("Failed to read the configuration" << _what);
        return 0;
    }
}

bool ConfigManager::setNumber(uint32_t _what, int64_t _value)
{
    if(_what < LAST_INTEGER_CONFIG) {
        m_confInteger[_what] = _value;
        return true;
    } else {
        LOG_WARN("Failed to write the configuration" << _what);
        return false;
    }
}

bool ConfigManager::setString(uint32_t _what, const std::string& _value)
{
    if(_what < LAST_STRING_CONFIG) {
        m_confString[_what] = _value;
        return true;
    } else {
        LOG_WARN("Failed to write the configuration" << _what);
        return false;
    }
}

std::string ConfigManager::getGlobalString(lua_State* _luaHandle, const std::string& _identifier, const std::string& _default)
{
    lua_getglobal(_luaHandle, _identifier.c_str());

    if(!lua_isstring(_luaHandle, -1)) {
        lua_pop(_luaHandle, 1);
        return _default;
    }

    int len = (int)lua_strlen(_luaHandle, -1);
    std::string ret(lua_tostring(_luaHandle, -1), len);
    lua_pop(_luaHandle,1);

    return ret;
}

int64_t ConfigManager::getGlobalNumber(lua_State* _luaHandle, const std::string& _identifier, int64_t _default)
{
    lua_getglobal(_luaHandle, _identifier.c_str());

    if(!lua_isnumber(_luaHandle, -1)) {
        lua_pop(_luaHandle, 1);
        return _default;
    }

    int64_t val = (int64_t)lua_tonumber(_luaHandle, -1);
    lua_pop(_luaHandle,1);

    return val;
}

bool ConfigManager::getGlobalBoolean(lua_State* _luaHandle, const std::string& _identifier, bool _default)
{
    lua_getglobal(_luaHandle, _identifier.c_str());

    if(lua_isnumber(_luaHandle, -1)) {
        int val = (int)lua_tonumber(_luaHandle, -1);
        lua_pop(_luaHandle, 1);
        return val != 0;
    } else if(lua_isstring(_luaHandle, -1)) {
        std::string val = lua_tostring(_luaHandle, -1);
        lua_pop(_luaHandle, 1);
        return val == "yes";
    } else if(lua_isboolean(_luaHandle, -1)) {
        bool v = lua_toboolean(_luaHandle, -1) != 0;
        lua_pop(_luaHandle, 1);
        return v;
    }

    return _default;
}
