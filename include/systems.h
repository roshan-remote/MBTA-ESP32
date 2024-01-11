#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <Arduino.h>

struct SystemInfo
{
    int systemIndex;
    int systemType;
    String shortAlias;
    uint16_t channelSize = 0;
    String channels[64];
};

#endif