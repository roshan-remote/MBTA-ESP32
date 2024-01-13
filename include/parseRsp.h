#ifndef PARSE_RSP_H
#define PARSE_RSP_H

#include "ArduinoJson.h"
#include "systems.h"

int parseResponse(JsonDocument &doc);
extern uint8_t heartBeatStat;

#endif