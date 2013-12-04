#include "headers.h"
#include "sniffer.h"
#include "packet.h"

Sniffer::Sniffer() : m_rawSocket(-1), m_readPacketSize(16384)
{
    strcpy(m_iface, "lo");
}

Sniffer::~Sniffer()
{
    if(m_rawSocket == -1) {
        close(m_rawSocket);
        m_rawSocket = -1;
    }
}

void Sniffer::init()
{
    m_rawSocket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(m_rawSocket == -1) {
        LOG_FATAL("Unable to create raw socket, maybe you need to be root?");
    }
}

void Sniffer::setIface(const char *iface)
{
    static char ifaces[24][6] =
        { "lo", "eth", "sl", "ppp", "ippp", "plip", "fddi", "dvb",
        "pvc", "hdlc", "ipsec", "sbni", "wvlan", "wlan", "sm2", "sm3",
        "pent", "lec", "brg", "tun", "tap", "cipcb", "tunl", "vlan"};

    for(int i = 0; i <= 24 - 1; i++) {
        if(strncmp(ifaces[i], iface, strlen(ifaces[i])) == 0) {
            strcpy(m_iface, iface);
            return;
        }
    }

    LOG_FATAL("Iface " << iface << " not supported");
}

bool Sniffer::getNextPacket(Packet *packet)
{
    static socklen_t fromlen = sizeof(struct sockaddr_ll);
    struct sockaddr_ll fromaddr;
    struct ifreq ifr;;
    fd_set set;
    struct timeval tv;
    int ss;

    FD_ZERO(&set);
    FD_SET(m_rawSocket, &set);

    tv.tv_sec = 0;
    tv.tv_usec = 50000;

    do {
        ss = select(m_rawSocket + 1, &set, 0, 0, &tv);
    } while ((ss < 0) && (errno == EINTR));

    if(FD_ISSET(m_rawSocket, &set)) {
        if(recvfrom(m_rawSocket, packet->buf, m_readPacketSize, 0, (struct sockaddr *)&fromaddr, &fromlen) == 0) {
            return false;
        }

        ifr.ifr_ifindex = fromaddr.sll_ifindex;
        ioctl(m_rawSocket, SIOCGIFNAME, &ifr);

        if(strcmp(m_iface, ifr.ifr_name) != 0) {
            return false;
        }

        if(ntohs(fromaddr.sll_protocol) != ETH_P_IP) {
            return false;
        }

        packet->type = fromaddr.sll_pkttype;
        packet->linktype = getFamilyLinkType(fromaddr.sll_hatype);

        if(!adjustPacketBuffer(packet)) {
            return false;
        }

        packet->ip = (struct iphdr *) (packet->packetbuf);
        packet->len = htons(packet->ip->tot_len) + (uint16_t)std::abs((int)(packet->packetbuf - packet->buf));
        if(packet->ip->protocol == IPPROTO_TCP) {
            packet->in_ip.tcp = (struct tcphdr *) ((char *) packet->ip + packet->ip->ihl * 4);
        } else if(packet->ip->protocol == IPPROTO_UDP) {
            packet->in_ip.udp = (struct udphdr *) ((char *) packet->ip + packet->ip->ihl * 4);
        }
        return true;
    }
    return false;
}

