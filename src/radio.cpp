#include "radio.h"
#include "config.h"

StaticJsonDocument<256> rriJson;

volatile uint32_t commandCounter = 0;

void configJsonReq(const char *interface, const char *method)
{
    // set the default parameters
    rriJson["rri"] = round(RRI_VERSION * 10) / 10.0;
    rriJson["options"] = RRI_OPTION;
    rriJson["interface"] = interface;
    rriJson["method"] = method;
    rriJson["id"] = ++commandCounter;
}

bool auth(void)
{
    configJsonReq("RRi_Connection", "authenticateChallenge");
    rriJson["params"]["certificate"] = true;
    rriJson["params"]["esn"] = ESN;
    return (sendTCPMsg(rriJson));
}

bool setAudio(bool useAudio)
{
    configJsonReq("RRi_Connection", "serviceRequest");
    rriJson["params"]["service"] = "RRIAPI";
    rriJson["params"]["audio"] = useAudio;
    return (sendTCPMsg(rriJson));
}

bool vchConnect(void)
{
    configJsonReq("RRIControlService", "requestForVCHConnection");
    return (sendTCPMsg(rriJson));
}

bool radioChange(uint16_t channelId)
{
    configJsonReq("RRIControlService", "changeChannel");
    rriJson["params"]["Channel ID"] = channelId;
    return (sendTCPMsg(rriJson));
}

bool systemChange(const char *systemName)
{
    configJsonReq("RRIControlService", "changeSystem");
    rriJson["params"]["System Name"] = systemName;
    return (sendTCPMsg(rriJson));
}

bool requestRSSI(void)
{
    configJsonReq("RRIControlService", "requestForLmrLteSignalStrength");
    rriJson["params"]["subscription"] = 1;
    return (sendTCPMsg(rriJson));
}

bool heartbeatStatus(void)
{
    configJsonReq("RRIControlService", "requestForHeartBeatStatus");
    rriJson["params"]["subscription"] = 2;
    return (sendTCPMsg(rriJson));
}

bool keepAlive(void)
{
    configJsonReq("RRi_Connection", "keepAlive");
    return (sendTCPMsg(rriJson));
}

bool systemListRequest(void)
{
    configJsonReq("RRIControlService", "systemListRequest");
    return (sendTCPMsg(rriJson));
}

/*Used for reporting system other than conventional*/
bool groupListReport(const char *systemName)
{
    configJsonReq("RRIControlService", "requestForSystemGroupSet");
    rriJson["params"]["System Name"] = systemName;
    return (sendTCPMsg(rriJson));
}

/*Used for reporting Conventional systems such as ACONV and P25CONV*/
bool requestForConventionalFrequencySet(const char *systemName)
{
    configJsonReq("RRIControlService", "requestForConventionalFrequencySet");
    rriJson["params"]["System Name"] = systemName;
    return (sendTCPMsg(rriJson));
}

bool requestForVoiceCallEvent(uint8_t subscription)
{
    configJsonReq("RRIControlService", "requestForVoiceCallEvent");
    rriJson["params"]["subscription"] = subscription;
    return (sendTCPMsg(rriJson));
}

bool channelGroupChangeEventRequest(uint8_t subscription)
{
    configJsonReq("RRIControlService", "channelGroupChangeEventRequest");
    rriJson["params"]["subscription"] = subscription;
    return (sendTCPMsg(rriJson));
}
