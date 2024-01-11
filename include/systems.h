#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <Arduino.h>

struct SystemInfo
{
    int systemIndex;
    int systemType;
    // uint64_t unitID;
    char shortAlias[32];
    char channels[32][16];
    uint16_t channelSize = 0;
};

#endif