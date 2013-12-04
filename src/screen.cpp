#include "headers.h"
#include "screen.h"
#include "monitor.h"
#include "configmanager.h"

Screen::Screen() : m_clearScreen(false)
{
}

Screen::~Screen()
{
}

void Screen::setupScreen()
{
    initscr();

    start_color();
    assume_default_colors(COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    attrset(A_BOLD);
    curs_set(0);
}

void Screen::destroyScreen()
{
    endwin();
}

//TODO: render only the necessary
void Screen::render()
{
    boost::mutex::scoped_lock lockClass(m_screenMutex);

    if(m_clearScreen) {
        clear();
        m_clearScreen = false;
    }

    move(0, 0);

    printw("\n");

    Monitor::instance()->render();

    attron(COLOR_PAIR(1));
    printw("   Lastest Actions\n");
    attroff(COLOR_PAIR(1));

    if(m_notifications.size() == 0)
        printw("       None yet\n");
    else
        for(NotificationList::iterator it = m_notifications.begin(); it != m_notifications.end(); ++it)
            printw((boost::str(boost::format("       %1%\n") % (*it))).c_str());

    printw("\n");

    refresh();
}

void Screen::notificate(std::string message, bool sendMail)
{
    boost::mutex::scoped_lock lockClass(m_screenMutex);

    LOG_INFO2("NOTIFICATION: " << message);

    if(sendMail) {
        system(boost::str(
                boost::format(ConfigManager::instance()->getString(ConfigManager::NOTIFICATION_COMMAND))
                % ConfigManager::instance()->getString(ConfigManager::NOTIFICATION_SUBJECT)
                % message
                ).c_str());
    }

    m_notifications.push_front(boost::str(boost::format("%1% %2%") % getCurrentTimeString() % message));
    while(m_notifications.size() > 6) {
        m_notifications.pop_back();
    }

    queueClear();
}
