#include "headers.h"
#include "monitor.h"
#include "sniffer.h"
#include "packet.h"
#include "scheduler.h"
#include "configmanager.h"
#include "trafficstats.h"
#include "ddos.h"
#include "screen.h"
#include "ipswitcher.h"

Monitor::Monitor() :
    m_sniffer(NULL),
    m_running(false),
    m_incoming(new TrafficStats("    Incoming")),
    m_outgoing(new TrafficStats("    Outgoing")),
    m_underMassiveDdos(false)
{
}


Monitor::~Monitor()
{
    for(WatchedIpList::iterator it = m_watchedIps.begin(); it != m_watchedIps.end(); ++it) {
        delete it->second;
    }
    m_watchedIps.clear();

    for(DdosList::iterator it = m_ddoses.begin(); it != m_ddoses.end(); ++it) {
        delete (*it);
    }
    m_ddoses.clear();

    for(DdosList::iterator it = m_oldDdoses.begin(); it != m_oldDdoses.end(); ++it) {
        delete (*it);
    }
    m_oldDdoses.clear();

    delete m_incoming;
    delete m_outgoing;

    if(m_underMassiveDdos) {
        system(ConfigManager::instance()->getString(ConfigManager::ONNETWORK_UNCOMPROMISE_COMMAND).c_str());
    }
}

void Monitor::loadWatchedIps(std::string filename)
{
    xmlDocPtr doc = xmlParseFile(filename.c_str());

    if(doc) {
        xmlNodePtr root, p;
        std::string tmp;

        root = xmlDocGetRootElement(doc);
        if(xmlStrcmp(root->name,(const xmlChar*)"watchedips") != 0) {
            xmlFreeDoc(doc);
            LOG_FATAL("invalid file format: " << filename);
        }

        p = root->children;

        while(p) {
            if(xmlStrcmp(p->name, (const xmlChar*)"ip") == 0) {
                if(!readXMLString(p, "ip", tmp)) {
                    LOG_FATAL("missing \'ip\' field in ip node at " << filename);
                }

                uint32_t ip = inet_addr(tmp.c_str());

                if(ip != INADDR_NONE) {
                    m_watchedIps[ip] = new TrafficStats(tmp);
                } else {
                    LOG_FATAL("invalid ip address " << tmp << " at " << filename);
                }
            }

            p = p->next;
        }
        xmlFreeDoc(doc);
    } else {
        LOG_FATAL("Failed to load watched ips file " << filename);
    }
}

void Monitor::run()
{
    Packet packet;

    m_incoming->reset();
    m_outgoing->reset();

    m_running = true;

    Screen::instance()->render();

    Scheduler::instance()->addEvent(createSchedulerTask(1000, boost::bind(&Monitor::update, this)));

    while(m_running) {
        if(m_sniffer->getNextPacket(&packet)) {
            boost::mutex::scoped_lock lockClass(m_monitorMutex);
            processPacket(&packet);
        }
    }
}

void Monitor::processPacket(Packet *packet)
{
    if(packet->type != PACKET_OUTGOING) {
        m_incoming->processPacket(packet);

        WatchedIpList::iterator it = m_watchedIps.find(packet->ip->daddr);
        if(it != m_watchedIps.end()) {
             (it->second)->processPacket(packet);
        }
    } else {
        m_outgoing->processPacket(packet);
    }
}


void Monitor::stop()
{
    m_running = false;
}

void Monitor::update()
{
    boost::mutex::scoped_lock lockClass(m_monitorMutex);

    if(m_running) {
        for(WatchedIpList::iterator it = m_watchedIps.begin(); it != m_watchedIps.end(); ++it) {
            uint32_t ip = (it->first);
            TrafficStats *trafficStats = (it->second);

            Ddos *ddos = NULL;

            // find active ddoses on the watched in
            DdosList::iterator it2 = m_ddoses.begin();
            for(; it2 != m_ddoses.end(); ++it2) {
                if((*it2)->getState() != Ddos::STOPED && (*it2)->getIp() == ip) {
                    ddos = (*it2);
                    break;
                }
            }

            // create new ddoses
            if(!ddos && (trafficStats->getKBits() >= ConfigManager::instance()->getNumber(ConfigManager::NOTIFICATION_TRAFFIC_THRESHOLD) ||
                         trafficStats->getPackets() >= ConfigManager::instance()->getNumber(ConfigManager::NOTIFICATION_PACKETS_THRESHOLD))) {
                ddos = new Ddos(ip);
                m_ddoses.push_back(ddos);
                Screen::instance()->notificate(boost::str(boost::format("Possible new DDoS at %1% with %2% kbits/s")
                                                          % ipToString(ip)
                                                          % trafficStats->getKBits()));
            }

            // update active ddos
            if(ddos) {
                trafficStats->logStats(boost::str(boost::format("    Flood incoming to %1%") % ipToString(ip)).c_str());

                Ddos::DdosState prevState = ddos->getState();
                ddos->update(trafficStats);

                if(ddos->getState() == Ddos::STOPED) {
                    if(prevState == Ddos::ACTIVE_NOT_CONFIRMED) {
                        m_ddoses.erase(it2);
                        delete ddos;
                    } else {
                        m_ddoses.erase(it2);
                        m_oldDdoses.push_front(ddos);

                        while(m_oldDdoses.size() > 6) {
                            delete m_oldDdoses.back();
                            m_oldDdoses.pop_back();
                        }
                    }
                }
            }
        }

        if(m_incoming->getKBits() >= ConfigManager::instance()->getNumber(ConfigManager::GLOBAL_TRAFFIC_THRESHOLD) ||
           m_incoming->getPackets() >= ConfigManager::instance()->getNumber(ConfigManager::GLOBAL_PACKETS_THRESHOLD)) {
            if(!m_underMassiveDdos) {
                system(ConfigManager::instance()->getString(ConfigManager::ONNETWORK_COMPROMISE_COMMAND).c_str());
                m_underMassiveDdos = true;
                Screen::instance()->notificate(boost::str(boost::format("Network traffic is compromised, it is under massive DDOS, possible type: %1%")
                                                          % m_incoming->getPossibleFloodType()), true);
            }
            m_massiveDdosPassTicks = 0;
        } else if(m_underMassiveDdos) {
            m_massiveDdosPassTicks++;
            if(m_massiveDdosPassTicks >= ConfigManager::instance()->getNumber(ConfigManager::NETWORK_UNCOMPROMISE_TICKS)) {
                system(ConfigManager::instance()->getString(ConfigManager::ONNETWORK_UNCOMPROMISE_COMMAND).c_str());
                m_underMassiveDdos = false;
                Screen::instance()->notificate("Network traffic is uncompromised now, the massive flood stopped");
            }
        }

        // render screen
        Screen::instance()->render();

        // reset traffic monitors
        m_incoming->reset();
        m_outgoing->reset();
        for(WatchedIpList::iterator it = m_watchedIps.begin(); it != m_watchedIps.end(); ++it) {
            (it->second)->reset();
        }

        // unlock and add next event
        lockClass.unlock();
        Scheduler::instance()->addEvent(createSchedulerTask(1000, boost::bind(&Monitor::update, this)));
    }
}

void Monitor::render()
{
    attron(COLOR_PAIR(1));
    printw("   Interface IP           Bits In      Packets In       TCP      UDP     ICMP      SYN\n");
    attroff(COLOR_PAIR(1));

    m_incoming->render();

    /*
    for(WatchedIpList::iterator it = m_watchedIps.begin(); it != m_watchedIps.end(); ++it)
        (it->second)->render();
    */

    printw("\n");

    attron(COLOR_PAIR(1));
    printw("   Interface IP           Bits Out     Packets Out      TCP      UDP     ICMP      SYN\n");
    attroff(COLOR_PAIR(1));

    m_outgoing->render();

    printw("\n");

    attron(COLOR_PAIR(1));
    printw("   Current Status\n");
    attroff(COLOR_PAIR(1));

    if(m_ddoses.size() == 0) {
        printw("       Network is fine =)\n");
    } else {
        attron(COLOR_PAIR(2));

        for(DdosList::iterator it = m_ddoses.begin(); it != m_ddoses.end(); ++it)
            (*it)->render();


        attroff(COLOR_PAIR(2));
    }

    printw("\n");

    attron(COLOR_PAIR(1));
    printw("   Lastest DDOSes\n");
    attroff(COLOR_PAIR(1));

    if(m_oldDdoses.size() == 0) {
        printw("       None yet\n");
    } else {
        for(DdosList::iterator it = m_oldDdoses.begin(); it != m_oldDdoses.end(); ++it)
            (*it)->render();
    }

    printw("\n");
}
