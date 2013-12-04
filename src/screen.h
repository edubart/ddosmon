#ifndef SCREEN_H
#define SCREEN_H

#include "headers.h"

class Screen : public Singleton<Screen>
{
public:
    Screen();
    virtual ~Screen();

    void setupScreen();
    void destroyScreen();

    void render();
    void notificate(std::string message, bool sendMail = false);

    void queueClear() { m_clearScreen = true; }

private:
    boost::mutex m_screenMutex;
    typedef std::list<std::string> NotificationList;
    NotificationList m_notifications;

    bool m_clearScreen;
};

#endif // SCREEN_H
