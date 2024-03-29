#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "ethSetup.h"
#include "systems.h"

extern uint8_t heartBeatStat;

using namespace std;

extern uint32_t KASent, connect, radioACK, radioNACK, radioServerConn;

extern SystemInfo systemInfo[16];
extern uint16_t systemInfoSize;

void setWebServer(void);
void showWebPage(void);

#endif