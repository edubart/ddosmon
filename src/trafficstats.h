#ifndef TRAFFICSTATS_H
#define TRAFFICSTATS_H

#include "headers.h"

struct Packet;

class TrafficStats
{
public:
    TrafficStats(std::string name);
    ~TrafficStats();

    void reset();
    void processPacket(Packet *packet);

    void logStats(const char *prefix);
    void render();

    uint32_t getBytes() const { return m_bytes; }
    uint32_t getKBits() const { return ((m_bytes*8)/1000); }
    uint32_t getPackets() const { return m_packets; }
    std::string getPossibleFloodType() const;

private:
    uint32_t m_bytes;
    uint32_t m_packets;
    uint32_t m_udpPackets;
    uint32_t m_tcpPackets;
    uint32_t m_icmpPackets;
    uint32_t m_synPackets;
    std::string m_name;
};

#endif // TRAFFICSTATS_H
