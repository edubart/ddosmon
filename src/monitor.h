#ifndef MONITOR_H
#define MONITOR_H

#include "headers.h"

struct Packet;

class Ddos;
class TrafficStats;
class Sniffer;

class Monitor : public Singleton<Monitor>
{
public:
    Monitor();
    virtual ~Monitor();

    void loadWatchedIps(std::string filename);

    void run();
    void stop();
    void update();
    void render();

    void setupSniffer(Sniffer *sniffer) { m_sniffer = sniffer; }
    int getWatchedIpsCount() const { return (int)m_watchedIps.size(); }

private:
    void processPacket(Packet *packet);

    boost::mutex m_monitorMutex;
    Sniffer *m_sniffer;
    bool m_running;
    TrafficStats *m_incoming;
    TrafficStats *m_outgoing;

    typedef std::list<Ddos*> DdosList;
    DdosList m_ddoses;
    DdosList m_oldDdoses;

    typedef std::map<uint32_t, TrafficStats*> WatchedIpList;
    WatchedIpList m_watchedIps;

    bool m_underMassiveDdos;
    int m_massiveDdosPassTicks;
};

#endif // MONITOR_H
