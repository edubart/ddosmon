#include "headers.h"
#include "packet.h"

bool adjustPacketBuffer(Packet *packet)
{
    switch(packet->linktype) {
    case LINK_ETHERNET:
    case LINK_LOOPBACK:
    case LINK_PLIP:
        packet->packetbuf = packet->buf + ETH_HLEN;
        return true;
    case LINK_PPP:
    case LINK_SLIP:
    case LINK_ISDN_RAWIP:
    case LINK_IPIP:
        packet->packetbuf = packet->buf;
        return true;
    case LINK_ISDN_CISCOHDLC:
    case LINK_FRAD:
    case LINK_DLCI:
        packet->packetbuf = packet->buf + 4;
        return true;
    case LINK_FDDI:
        packet->packetbuf = packet->buf + sizeof(struct fddihdr);
        return true;
    case LINK_VLAN:
        packet->packetbuf = packet->buf + 18;
        return true;
    default:
        packet->packetbuf = NULL;
        return false;
    }
}

uint16_t getFamilyLinkType(uint16_t family)
{
    uint16_t result = 0;
    switch (family) {
    case ARPHRD_ETHER:
        result = LINK_ETHERNET;
        break;
    case ARPHRD_LOOPBACK:
        result = LINK_LOOPBACK;
        break;
    case ARPHRD_SLIP:
    case ARPHRD_CSLIP:
    case ARPHRD_SLIP6:
    case ARPHRD_CSLIP6:
        result = LINK_SLIP;
        break;
    case ARPHRD_PPP:
        result = LINK_PPP;
        break;
    case ARPHRD_FDDI:
        result = LINK_FDDI;
        break;
    case ARPHRD_IEEE802:
    case ARPHRD_IEEE802_TR:
        result = LINK_TR;
        break;
    case ARPHRD_FRAD:
        result = LINK_FRAD;
        break;
    case ARPHRD_DLCI:
        result = LINK_DLCI;
        break;
    case ARPHRD_HDLC:
        result = LINK_CISCOHDLC;
        break;
    case ARPHRD_TUNNEL:
        result = LINK_IPIP;
        break;
    default:
        result = LINK_INVALID;
        break;
    }
    return result;
}
