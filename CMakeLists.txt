CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(ddosmon)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug)
ENDIF(NOT CMAKE_BUILD_TYPE)

FIND_PACKAGE(Boost COMPONENTS thread system REQUIRED)
INCLUDE_DIRECTORIES(/usr/include/libxml2 /usr/include/lua5.1)
ADD_DEFINITIONS(-D_REENTRANT)

SET(CMAKE_CXX_FLAGS_DEBUG "-O1 -g -ggdb -fno-inline ")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -rdynamic")
SET(CMAKE_CXX_FLAGS "-Wall -fmessage-length=0 -include ${PROJECT_SOURCE_DIR}/src/headers.h")

ADD_EXECUTABLE(ddosmon
src/configmanager.cpp  src/ipswitcher.h   src/screen.cpp
src/configmanager.h    src/logger.cpp     src/screen.h
src/ddos.cpp           src/logger.h       src/singleton.h
src/ddos.h             src/monitor.cpp    src/sniffer.cpp
src/ddosmon.cpp        src/monitor.h      src/sniffer.h
src/dispatcher.cpp     src/packet.cpp     src/tools.cpp
src/dispatcher.h       src/packet.h       src/tools.h
src/headers.h          src/scheduler.cpp  src/trafficstats.cpp
src/ipswitcher.cpp     src/scheduler.h    src/trafficstats.h
)

TARGET_LINK_LIBRARIES(ddosmon ${Boost_LIBRARIES} lua5.1 xml2 ncurses)
