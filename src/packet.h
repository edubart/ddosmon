#ifndef PACKET_H
#define PACKET_H

#include "headers.h"

#define LINK_ETHERNET        1
#define LINK_PPP            2
#define LINK_SLIP           3
#define LINK_PLIP           4
#define LINK_LOOPBACK        5
#define LINK_ISDN_RAWIP        6
#define LINK_ISDN_CISCOHDLC    7
#define LINK_CISCOHDLC      7
#define LINK_FDDI           8
#define LINK_FRAD           9
#define LINK_DLCI           10
#define LINK_TR             11
#define LINK_IPIP           12
#define LINK_VLAN           13
#define LINK_INVALID        0

#define PACKET_BUFFER_SIZE  16384

struct Packet
{
    union {
        struct tcphdr *tcp;
        struct udphdr *udp;
    } in_ip;
    struct iphdr *ip;
    uint16_t len;
    uint16_t linktype;
    uint8_t type;
    char buf[PACKET_BUFFER_SIZE];
    char *packetbuf;
};

bool adjustPacketBuffer(Packet *packet);
uint16_t getFamilyLinkType(uint16_t family);

#endif // PACKET_H
