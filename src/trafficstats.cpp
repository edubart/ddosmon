#include "trafficstats.h"
#include "packet.h"

TrafficStats::TrafficStats(std::string name) :
        m_bytes(0),
        m_packets(0),
        m_udpPackets(0),
        m_tcpPackets(0),
        m_icmpPackets(0),
        m_synPackets(0),
        m_name(name)
{
}


TrafficStats::~TrafficStats()
{

}

void TrafficStats::reset()
{
    m_bytes = 0;
    m_packets = 0;
    m_udpPackets = 0;
    m_tcpPackets = 0;
    m_icmpPackets = 0;
    m_synPackets = 0;
}

void TrafficStats::processPacket(Packet *packet)
{
    m_bytes += packet->len;
    m_packets++;

    switch(packet->ip->protocol) {
        case IPPROTO_TCP:
            m_tcpPackets++;
            if(packet->in_ip.tcp->syn) {
                m_synPackets++;
            }
            break;
        case IPPROTO_UDP:
            m_udpPackets++;
            break;
        case IPPROTO_ICMP:
            m_icmpPackets++;
            break;
    }
}

void TrafficStats::logStats(const char *prefix)
{
    using namespace std;
    LOG_INFO2(prefix << " => " << bytesToKBits(m_bytes) << " kbits/s  "
             << m_packets << " pkt/s " << "  ("
             << setprecision(3) << (m_packets > 0 ? ((m_tcpPackets*100)/(float)m_packets) : 0) << "% TCP | "
             << setprecision(3) << (m_packets > 0 ? ((m_udpPackets*100)/(float)m_packets) : 0) << "% UDP | "
             << setprecision(3) << (m_packets > 0 ? ((m_icmpPackets*100)/(float)m_packets) : 0) << "% ICMP | "
             << setprecision(3) << (m_tcpPackets > 0 ? ((m_synPackets*100)/(float)m_tcpPackets) : 0) << "% SYN)");
}


void TrafficStats::render()
{
    using namespace std;
    stringstream out;
    out << setfill(' ') << setw(15) << m_name << "  "
         << setw(8) << setfill(' ') << bytesToKBits(m_bytes) << " kbits/s" << "  "
         << setw(8) << setfill(' ') << m_packets << " pkt/s " << "  "
         << setw(6) << setfill(' ') << setprecision(3) << (m_packets > 0 ? ((m_tcpPackets*100)/(float)m_packets) : 0) << "%" << "  "
         << setw(6) << setfill(' ') << setprecision(3) << (m_packets > 0 ? ((m_udpPackets*100)/(float)m_packets) : 0) << "%" << "  "
         << setw(6) << setfill(' ') << setprecision(3) << (m_packets > 0 ? ((m_icmpPackets*100)/(float)m_packets) : 0) << "%" << "  "
         << setw(6) << setfill(' ') << setprecision(3) << (m_tcpPackets > 0 ? ((m_synPackets*100)/(float)m_tcpPackets) : 0) << "%" << "  "
         << "\n";
    printw("%s", out.str().c_str());
}


std::string TrafficStats::getPossibleFloodType() const
{
    std::string floodType = "not detected";
    std::map<uint32_t, uint8_t> sortedList;
    sortedList[m_udpPackets] = 1;
    sortedList[m_synPackets] = 2;
    sortedList[m_icmpPackets] = 3;
    sortedList[m_tcpPackets] = 4;
    if(sortedList.rbegin()->first > 0) {
        switch(sortedList.rbegin()->second) {
        case 1:
            floodType = "UDP flood";
            break;
        case 2:
            floodType = "SYN flood";
            break;
        case 3:
            floodType = "ICMP flood";
            break;
        case 4:
            floodType = "TCP flood";
            break;
        }
    }
    return floodType;
}
