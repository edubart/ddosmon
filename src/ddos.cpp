#include "headers.h"
#include "ddos.h"
#include "screen.h"
#include "configmanager.h"
#include "ipswitcher.h"
#include "trafficstats.h"

#define DDOS_CONFIRM_SECONDS 5

Ddos::Ddos(uint32_t ip) :
        m_ip(ip),
        m_startedTime(getSystemTime()),
        m_duration(0),
        m_totalBytes(0),
        m_activeTicks(0),
        m_stopTicks(0),
        m_state(ACTIVE_NOT_CONFIRMED),
        m_offlined(false)
{
    m_ddosType = "not detected";
}

Ddos::~Ddos()
{
}


void Ddos::update(TrafficStats *trafficStats)
{
    uint32_t kbits = trafficStats->getKBits();
    uint32_t packets = trafficStats->getPackets();

    m_duration++;
    m_totalBytes += trafficStats->getBytes();

    // force ip offline when threshold > allowed threshold
    if(!m_offlined && (kbits >= ConfigManager::instance()->getNumber(ConfigManager::IP_TRAFFIC_THRESHOLD) ||
                       packets >= ConfigManager::instance()->getNumber(ConfigManager::IP_PACKETS_THRESHOLD))) {
        if(m_state == ACTIVE_NOT_CONFIRMED) {
            m_state = ACTIVE_CONFIRMED;
            Screen::instance()->notificate(boost::str(boost::format("DDoS at %1% instantanealy confirmed with %2% kbits/s, possible type: %3%")
                                                      % ipToString(m_ip)
                                                      % kbits
                                                      % trafficStats->getPossibleFloodType()), true);
        }

        Screen::instance()->notificate(
                boost::str(boost::format("Not allowed threshold at %1% (%2% kbits/s) ip offline forced")
                % ipToString(m_ip) % kbits));
        IpSwitcher::instance()->blockIp(m_ip);
        m_offlined = true;
    }

    // active/stop algorithm
    if(kbits >= ConfigManager::instance()->getNumber(ConfigManager::NOTIFICATION_TRAFFIC_THRESHOLD) ||
       packets >= ConfigManager::instance()->getNumber(ConfigManager::NOTIFICATION_PACKETS_THRESHOLD)) {
        m_stopTicks = 0;

        if(m_state == ACTIVE_NOT_CONFIRMED && ++m_activeTicks >= DDOS_CONFIRM_SECONDS) {
            m_state = ACTIVE_CONFIRMED;
            Screen::instance()->notificate(boost::str(boost::format("DDoS at %1% confirmed with %2% kbits/s, possible type: %3%")
                                                      % ipToString(m_ip)
                                                      % kbits
                                                      % trafficStats->getPossibleFloodType()), true);
        } else {
            m_ddosType = trafficStats->getPossibleFloodType();
        }
    } else {
        m_activeTicks = 0;
        if(++m_stopTicks >= DDOS_CONFIRM_SECONDS) {
            if(m_state == ACTIVE_CONFIRMED) {
                Screen::instance()->notificate(boost::str(boost::format("DDoS at %1% stopped") % ipToString(m_ip)));
            } else {
                Screen::instance()->notificate(boost::str(boost::format("False DDoS alarm at %1%") % ipToString(m_ip)));
            }
            m_state = STOPED;
        }
    }
}


void Ddos::render()
{
    if(m_state != STOPED) {
        if(m_state == ACTIVE_CONFIRMED) {
            printw("       Active ");
        } else {
            printw("       Possible ");
        }

        printw(boost::str(boost::format
                          ("DDOS at %1% during %2% seconds with %3% kbits/s, possible type: %4%\n") %
                         ipToString(m_ip) % m_duration % bytesToKBits(m_totalBytes/m_duration)
                         % m_ddosType).c_str());
    } else {
        printw(boost::str(boost::format
                          ("       %1% At %2% during %3% seconds with %4% kbits/s, possible type: %5%") %
                          unixTimeToTimeString(m_startedTime/1000) % ipToString(m_ip) %
                          (m_duration - 5)  %  bytesToKBits(m_totalBytes/(m_duration-5))
                          % m_ddosType).c_str());

        if(m_offlined) {
            printw(" (offlined)");
        }

        printw("\n");
    }
}
