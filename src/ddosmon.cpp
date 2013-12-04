#include "headers.h"
#include "sniffer.h"
#include "monitor.h"
#include "dispatcher.h"
#include "scheduler.h"
#include "configmanager.h"
#include "screen.h"

void signalHandler(int sig)
{
    if(sig == SIGINT)
        std::cout << std::endl;
    LOG_WARN("got signal = " << strsignal(sig));

    static bool stopSent = false;
    if(!stopSent) {
        stopSent = true;
        Monitor::instance()->stop();
    }
}


int main(int argc, char *argv[])
{
    // setup the logger
    signal(SIGTERM, &signalHandler);
    signal(SIGINT, &signalHandler);
    signal(SIGHUP, &signalHandler);
    signal(SIGKILL, &signalHandler);

    if(argc != 2) {
        std::cout << "Usage: " << argv[0] << " <config>" << std::endl;
        return -1;
    }

    // setup confs
    ConfigManager::instance()->setString(ConfigManager::CONFIG_FILE, argv[1]);
    ConfigManager::instance()->loadFile();
    Logger::instance()->setVerbosity(Logger::DEBUG);
    Logger::instance()->setLogFile(ConfigManager::instance()->getString(ConfigManager::LOG_FILE));

    // read watchedips
    Monitor::instance()->loadWatchedIps(ConfigManager::instance()->getString(ConfigManager::WATCHEDIPS_XML));

    // setup sniffer
    Sniffer *sniffer = new Sniffer();
    sniffer->setIface(ConfigManager::instance()->getString(ConfigManager::INTERFACE).c_str());
    sniffer->setReadPacketSize(128);
    sniffer->init();

    // start core threads
    Dispatcher::instance()->start();
    Scheduler::instance()->start();

    // setup screen
    Screen::instance()->setupScreen();

    Monitor::instance()->setupSniffer(sniffer);
    Monitor::instance()->run();


    Screen::instance()->destroyScreen();
    Scheduler::instance()->stop();
    Dispatcher::instance()->stop();

    delete sniffer;

    return 0;
}
