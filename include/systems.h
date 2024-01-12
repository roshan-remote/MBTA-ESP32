#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <Arduino.h>

struct SystemInfo
{
    int systemIndex;
    int systemType;
    String shortAlias;
    uint16_t channelSize;
    String* channels;
};

#endif