#ifndef SNIFFER_H
#define SNIFFER_H

#include "headers.h"

struct Packet;

class Sniffer
{
public:
    Sniffer();
    virtual ~Sniffer();

    void init();
    void setIface(const char *iface);
    void setReadPacketSize(uint16_t size) { m_readPacketSize = size; }

    bool getNextPacket(Packet *packet);

private:
    int m_rawSocket;
    uint32_t m_readPacketSize;
    char m_iface[8];
};

#endif // SNIFFER_H
