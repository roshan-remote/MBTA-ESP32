#include "webHandler.h"
#include "radio.h"
#include "config.h"

extern bool pauseKeepAlive;

// Define functions for each request

void webSystemChange(const char sys[])
{
    systemChange(sys);
    debugln("[1 CMD::]System " + String(sys) + " pressed");
}

void webChannelChange(uint16_t channel)
{
    radioChange(channel);
    debugln("[2 CMD::]Channel " + String(channel) + " pressed");
}

void handleSubscribeRSSI(void)
{
    debugln(F("[3.CMD::]Subscribing to RSS"));
    requestRSSI();
}

void handleHeartbeatStatus(void)
{
    debugln(F("[4.CMD::]Subscribing to HeartBeatStatus"));
    heartbeatStatus();
}

void handleSystemList(void)
{
    debugln(F("[5.]System List request"));
    systemListRequest();
}

void resetOPTA(void)
{
    debugln(F("[RESET::]USER"));
    ESP.restart();
}

void switcher(uint8_t type)
{
    if (type == 1)
    {
        debugln(F("[6.]Switch 1 request"));
        leadCar = !leadCar;
        if (leadCar)
            systemChange("ACONV");
        else
            systemChange("P25CONV");
    }
    else if (type == 2)
    {
        debugln(F("[7.]Switch 2 request"));
        systemChange("P25CONV");
    }
}

void relayControl(uint8_t type)
{
    debugln("Relay Control: " + String(type));
    if (type == 0)
        digitalWrite(32, HIGH);
    else if (type == 1)
        digitalWrite(33, HIGH);
    else if (type == 2)
        digitalWrite(32, LOW);
    else if (type == 3)
        digitalWrite(33, LOW);
    // closeRelay = type;
}