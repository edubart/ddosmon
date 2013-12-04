#include "headers.h"
#include "ipswitcher.h"
#include "monitor.h"
#include "configmanager.h"
#include "scheduler.h"
#include "screen.h"

#define CHECK_IP_INTERVAL 5*60*1000

IpSwitcher::IpSwitcher()
{
    Scheduler::instance()->addEvent(createSchedulerTask(CHECK_IP_INTERVAL, boost::bind(&IpSwitcher::checkIps, this)));
}

IpSwitcher::~IpSwitcher()
{
    for(BlockedIpsMap::iterator it =m_blockedIps.begin(); it != m_blockedIps.end();++it) {
        unblockIp(it->first);
    }
}

void IpSwitcher::unblockIp(uint32_t ip)
{
    system(boost::str(boost::format(ConfigManager::instance()->getString(ConfigManager::ONUNBLOCKIP_COMMAND)) % ipToString(ip)).c_str());
    m_blockedIps.erase(ip);
}

void IpSwitcher::blockIp(uint32_t ip)
{
    system(boost::str(boost::format(ConfigManager::instance()->getString(ConfigManager::ONBLOCKIP_COMMAND)) % ipToString(ip)).c_str());
    m_blockedIps[ip] = getSystemTime();
}

bool IpSwitcher::isBlocked(uint32_t ip) const
{
    return (m_blockedIps.find(ip) != m_blockedIps.end());
}

void IpSwitcher::checkIps()
{
    if(!m_blockedIps.empty()) {
        int64_t timeNow = getSystemTime();
        int retryTime = ConfigManager::instance()->getNumber(ConfigManager::IPBLOCK_RETRY_TICKS);

        BlockedIpsMap mapBackup = m_blockedIps;
        for(BlockedIpsMap::iterator it = mapBackup.begin(); it != mapBackup.end(); ++it) {
            const uint32_t &ip = (it->first);
            const int64_t &timeBlocked = (it->second);

            if(timeNow - timeBlocked >= retryTime) {
                unblockIp(ip);
                Screen::instance()->notificate(boost::str(boost::format("Ip %1% offlined is back after %2% minutes")
                                                          % ipToString(ip) % ((timeNow - timeBlocked)/(60*1000))));
            }
        }
    }

    Scheduler::instance()->addEvent(createSchedulerTask(CHECK_IP_INTERVAL, boost::bind(&IpSwitcher::checkIps, this)));
}
