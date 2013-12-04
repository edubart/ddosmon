#ifndef IPSWITCHER_H
#define IPSWITCHER_H

#include "headers.h"

class IpSwitcher : public Singleton<IpSwitcher>
{
    typedef std::map<uint32_t, int64_t> BlockedIpsMap;

public:
    IpSwitcher();
    virtual ~IpSwitcher();

    void unblockIp(uint32_t ip);
    void blockIp(uint32_t ip);
    bool isBlocked(uint32_t ip) const;

    void checkIps();

private:
    BlockedIpsMap m_blockedIps;
};

#endif // IPSWITCHER_H
