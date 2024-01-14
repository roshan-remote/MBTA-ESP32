#ifndef ETH_SETUP_H
#define ETH_SETUP_H

#include <ArduinoJson.h>
#include <ETH.h>
#include "config.h"

// Layers stack
extern WiFiClient client;
extern int queue;
extern char readBuffer[READ_BUF_SIZE];

extern char logsQueue[LOGS_QUOTA][MAX_LOG_LENGTH];
extern uint8_t logsCounter;

void connectServer(void);
void readEthernet(void);
void ethHandleLogs(const char *message, bool send);

#endif