#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H
#include <Arduino.h>

extern bool leadCar;
extern bool ATO;

void webSystemChange(const char sys[]);
void webChannelChange(uint16_t channel);
void handleSubscribeRSSI(void);
void handleHeartbeatStatus(void);
void handleSystemList(void);
void resetOPTA(void);
void switcher(uint8_t type);

#endif