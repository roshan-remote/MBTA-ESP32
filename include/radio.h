#ifndef RADIO_H
#define RADIO_H

#include "readTCP.h"
#include "sendTCP.h"

extern uint16_t maxChannelSize;

bool auth(void);
bool setAudio(bool useAudio);
bool vchConnect(void);
bool keepAlive(void);
bool radioChange(uint16_t channelId);
bool systemChange(const char *systemName);
bool requestRSSI(void);
bool heartbeatStatus(void);
bool systemListRequest(void);
bool groupListReport(const char *systemName);
bool requestForConventionalFrequencySet(const char *systemName);
bool requestForVoiceCallEvent(uint8_t subscription);
bool channelGroupChangeEventRequest(uint8_t subscription);

#endif
