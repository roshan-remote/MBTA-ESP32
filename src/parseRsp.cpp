#include "parseRsp.h"
#include "config.h"

SystemInfo systemInfo[16];
uint16_t systemInfoSize = 0;
uint16_t maxChannelSize = 2;
char mMissionName[32];
bool lastPacket = false;
bool radioAck = false;

int systemInfoCounter = 0;

// Parse a JSON response from XL -200l
int parseResponse(JsonDocument &rriJsonRsp)
{
    // check if it mathces RRI version
    if (!(rriJsonRsp.containsKey("rri") && rriJsonRsp["rri"] == round(RRI_VERSION * 10) / 10.0))
    {
        debugln("RRI Version does not match !");
        return -1;
    }

    // check if it mathces RRI options
    if (!(rriJsonRsp.containsKey("options") && rriJsonRsp["options"] == RRI_OPTION))
    {
        debugln("RRI set option do not match !");
        return -1;
    }

    // For connection and keep alive responses
    if (strcmp(rriJsonRsp["interface"], "RRi_Connection") == 0)
    {
        if (strcmp(rriJsonRsp["method"], "authenticateChallenge") == 0)
        {
            debugln("[2.1::] Greetings from Radio !");
            debugln("[2.2::] Radio challenges for authentication");
            return CONNECT;
        }
        else if (strcmp(rriJsonRsp["method"], "serviceRequest") == 0)
        {
            const char *substringStart = strstr(rriJsonRsp["params"]["mMissionName"], "/etc/mfill/");
            if (substringStart != NULL)
            {
                substringStart += strlen("/etc/mfill/");
                strcpy(mMissionName, substringStart);
            }
            debugln("[3.1::] Authentication successful !");
            debugln("[3.2::] Request for Connection with audio");

            return AUTH;
        }
        else if (strcmp(rriJsonRsp["method"], "connectionAccept") == 0)
        {
            debugln("[4.1::] Connection Accepted");
            return AUDIO_SET;
        }
        else if (strcmp(rriJsonRsp["method"], "keepAliveResponse") == 0)
        {
            // debugln("[Keepalive Response::]");
            return PING;
        }
    }

    // For JSON postive and Negative ACK
    else if (strcmp(rriJsonRsp["interface"], "RRIControlService") == 0)
    {
        if (strcmp(rriJsonRsp["method"], "vchAcceptedConnection") == 0)
        {
            if (rriJsonRsp["params"]["connectionInit"])
            {
                debugln("[5::] VCH ACCEPTED");
                return VCH_OK;
            }
            else
            {
                debugln("[5::] VCH FAILED");
                delay(STARTUP_DELAY); // Give radio time to restart connection
                return VCH_FAIL;
            }
        }
        else if (strcmp(rriJsonRsp["method"], "radioAcknowledge") == 0)
        {
            debugln("[Radio ACK::]");
            radioAck = true;
            if (lastPacket && radioAck)
            {
                debugln("Response Last packet now incrementing!!");
                systemInfoCounter++;
                lastPacket = false;
                radioAck = false;
                return SYS_LIST;
            }
            return RADIO_ACK;
        }
        else if (strcmp(rriJsonRsp["method"], "radioNegativeAcknowledge") == 0)
        {
            debugln("[Radio NAK::]");
            return RADIO_NACK;
        }
        else if (strcmp(rriJsonRsp["method"], "reportForOTARRekeyEvent") == 0)
        {
            debugln("[OTAR Rekey Event::]");
            return 0;
        }
        else if (strcmp(rriJsonRsp["method"], "reportForHeartBeatStatus") == 0)
        {
            heartBeatStat = rriJsonRsp["params"]["Status"];
            debugln("[Heartbeat Report::]");
            return (0);
        }
        else if (strcmp(rriJsonRsp["method"], "reportForSystemList") == 0)
        {
            if (rriJsonRsp["params"]["systemsArray"] != NULL)
            {
                JsonArray systemsArray = rriJsonRsp["params"]["systemsArray"].as<JsonArray>();
                systemInfoSize = systemsArray.size();
                debugln("[System List Report::]");
                for (uint8_t i = 0; i < systemInfoSize; i++)
                {
                    systemInfo[i].systemIndex = systemsArray[i]["System Index"];
                    systemInfo[i].systemType = systemsArray[i]["System Type"];
                    systemInfo[i].shortAlias= systemsArray[i]["Short System Alias"].as<String>();
                    // debugln(systemInfo[i].systemIndex);
                    // debugln(systemInfo[i].systemType);
                    // debugln(systemInfo[i].unitID);
                    debugln(systemInfo[i].shortAlias);
                    systemInfo[i].channelSize = 0;
                }
                systemInfoCounter = 0;
                maxChannelSize = 0;
            }
            if (rriJsonRsp["params"]["Last Packet"])
            {
                lastPacket = true;
                if (lastPacket && radioAck)
                {
                    debugln("Response Last packet now incrementing!!");
                    lastPacket = false;
                    radioAck = false;
                    return SYS_LIST;
                }
                else
                    systemInfoCounter--;
            }
            return LISTEN;
        }
        else if (strcmp(rriJsonRsp["method"], "reportForConventionalFrequencySet") == 0)
        {
            if (rriJsonRsp["params"]["channelsArray"] != NULL)
            {
                JsonArray channelsArray = rriJsonRsp["params"]["channelsArray"].as<JsonArray>();
                for (uint8_t j = 0; j < channelsArray.size(); j++)
                {
                    // strcpy(systemInfo[systemInfoCounter].channels[systemInfo[systemInfoCounter].channelSize + j], channelsArray[j]["Short channel Alias"]);
                    systemInfo[systemInfoCounter].channels[systemInfo[systemInfoCounter].channelSize + j]= channelsArray[j]["Short channel Alias"].as<String>();
                    debug("[Channels::]" + String(systemInfoCounter) + "," + String(systemInfo[systemInfoCounter].channelSize + j) + "->");
                    debugln(systemInfo[systemInfoCounter].channels[systemInfo[systemInfoCounter].channelSize + j]);
                }
                systemInfo[systemInfoCounter].channelSize = systemInfo[systemInfoCounter].channelSize + channelsArray.size();
                maxChannelSize = systemInfo[systemInfoCounter].channelSize > maxChannelSize ? systemInfo[systemInfoCounter].channelSize : maxChannelSize;
                if (rriJsonRsp["params"]["Last Packet"])
                {
                    lastPacket = true;
                    if (lastPacket && radioAck)
                    {
                        debugln("Response Last packet now incrementing!!");
                        systemInfoCounter++;
                        lastPacket = false;
                        radioAck = false;
                        return SYS_LIST;
                    }
                }
            }
            return LISTEN;
        }

        else if (strcmp(rriJsonRsp["method"], "reportForSystemGroupSet") == 0)
        {
            JsonArray groupsArray = rriJsonRsp["params"]["groupsArray"].as<JsonArray>();
            if (rriJsonRsp["params"]["groupsArray"] != NULL)
            {
                for (uint8_t j = 0; j < groupsArray.size(); j++)
                {
                    // strcpy(systemInfo[systemInfoCounter].channels[systemInfo[systemInfoCounter].channelSize + j], groupsArray[j]["Short Group Alias"]);
                    systemInfo[systemInfoCounter].channels[systemInfo[systemInfoCounter].channelSize + j]= groupsArray[j]["Short Group Alias"].as<String>();
                    debug("[Channels::]" + String(systemInfoCounter) + "," + String(systemInfo[systemInfoCounter].channelSize + j) + "->");
                    debugln(systemInfo[systemInfoCounter].channels[systemInfo[systemInfoCounter].channelSize + j]);
                }
                systemInfo[systemInfoCounter].channelSize = systemInfo[systemInfoCounter].channelSize + groupsArray.size();
                maxChannelSize = systemInfo[systemInfoCounter].channelSize > maxChannelSize ? systemInfo[systemInfoCounter].channelSize : maxChannelSize;
                if (rriJsonRsp["params"]["Last Packet"])
                {
                    lastPacket = true;
                    if (lastPacket && radioAck)
                    {
                        debugln("Response Last packet now incrementing!!");
                        systemInfoCounter++;
                        lastPacket = false;
                        radioAck = false;
                        return SYS_LIST;
                    }
                }
            }
            return LISTEN;
        }

        // For error messages
        else if (rriJsonRsp.containsKey("error"))
        {
            int errorCode = rriJsonRsp["error"]["code"].as<int>();
            debug(F("[ERROR::]Error Response"));
            debugln(errorCode);
            return errorCode;
        }
    }

    else
    {
        debugln("[ERROR::]Error Not Defined");
        return -1;
    }
    return DEFAULT;
}
