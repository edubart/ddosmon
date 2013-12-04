#ifndef DDOS_H
#define DDOS_H

#include "headers.h"

class TrafficStats;

class Ddos
{
public:
    enum DdosState {
        ACTIVE_NOT_CONFIRMED,
        ACTIVE_CONFIRMED,
        STOPED
    };

    Ddos(uint32_t ip);
    ~Ddos();

    void update(TrafficStats *trafficStats);
    void render();

    void setState(DdosState state) { m_state = state; }
    DdosState getState() const { return m_state; }

    uint32_t getIp() const { return m_ip; }
    uint32_t getKBitsTraffic() const { return (((m_totalBytes/m_duration)*8)/1000); }

private:
    uint32_t m_ip;
    uint64_t m_startedTime;
    uint32_t m_duration;
    uint32_t m_totalBytes;
    uint32_t m_activeTicks;
    uint16_t m_stopTicks;
    std::string m_ddosType;
    DdosState m_state;
    bool m_offlined;
};

#endif // DDOS_H
