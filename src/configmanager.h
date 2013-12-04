#ifndef __CONFIGMANAGER_H__
#define __CONFIGMANAGER_H__

#include "headers.h"

class ConfigManager : public Singleton<ConfigManager>
{
public:
    ConfigManager();
    virtual ~ConfigManager();

    enum string_config_t {
        CONFIG_FILE,
        INTERFACE,
        LOG_FILE,
        ONBLOCKIP_COMMAND,
        ONUNBLOCKIP_COMMAND,
        ONNETWORK_COMPROMISE_COMMAND,
        ONNETWORK_UNCOMPROMISE_COMMAND,
        NOTIFICATION_COMMAND,
        WATCHEDIPS_XML,
        NOTIFICATION_SUBJECT,
        LAST_STRING_CONFIG
    };

    enum integer_config_t {
        GLOBAL_TRAFFIC_THRESHOLD,
        GLOBAL_PACKETS_THRESHOLD,
        IP_TRAFFIC_THRESHOLD,
        IP_PACKETS_THRESHOLD,
        NOTIFICATION_TRAFFIC_THRESHOLD,
        NOTIFICATION_PACKETS_THRESHOLD,
        IPBLOCK_RETRY_TICKS,
        NETWORK_UNCOMPROMISE_TICKS,
        LAST_INTEGER_CONFIG
    };

    void loadFile();
    const std::string& getString(uint32_t _what) const;
    int64_t getNumber(uint32_t _what) const;
    bool setNumber(uint32_t _what, int64_t _value);
    bool setString(uint32_t _what, const std::string& _value);

private:
    std::string getGlobalString(lua_State* _luaHandle, const std::string& _identifier, const std::string& _default="");
    int64_t getGlobalNumber(lua_State* _luaHandle, const std::string& _identifier, int64_t _default=0);
    bool getGlobalBoolean(lua_State* _luaHandle, const std::string& _identifier, bool _default=false);

    bool m_isLoaded;
    std::string m_confString[LAST_STRING_CONFIG];
    int64_t m_confInteger[LAST_INTEGER_CONFIG];
};

#endif
