#ifndef __HEADERS_H__
#define __HEADERS_H__

// g++ -D_REENTRANT -I/usr/include/libxml2 -I/usr/include/lua5.1 headers.h

//linux
#include <sys/types.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

//linux extras
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_fddi.h>
#include <linux/sockios.h>

//c
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <csignal>
#include <cctype>
#include <cassert>
#include <cerrno>

//std
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <utility>
#include <iomanip>
#include <bitset>

//boost
#include <boost/cstdint.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

//libxml
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/threads.h>

//lua
extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

//local
#include "tools.h"
#include "singleton.h"
#include "logger.h"

//ncurses
#include <ncurses.h>

#endif
