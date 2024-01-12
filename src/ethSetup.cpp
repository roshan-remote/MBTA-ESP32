#include "ethSetup.h"
#include "config.h"

#include "Arduino.h"

IPAddress clientIp;
IPAddress serverIp;

uint32_t insideBraces = 0;
uint32_t contentIndex = 0;

ParsedData parsedData[4];

static bool eth_connected = false;

// WARNING: WiFiEvent is called from a separate FreeRTOS task (thread)!
void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case ARDUINO_EVENT_ETH_START:
        Serial.println("ETH Started");
        // The hostname must be set after the interface is started, but needs
        // to be set before DHCP, so set it from the event handler thread.
        ETH.setHostname("esp32-ethernet");
        break;
    case ARDUINO_EVENT_ETH_CONNECTED:
        Serial.println("ETH Connected");
        break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        Serial.println("ETH Disconnected");
        eth_connected = false;
        break;
    case ARDUINO_EVENT_ETH_STOP:
        Serial.println("ETH Stopped");
        eth_connected = false;
        break;
    default:
        break;
    }
}

void beginConnect()
{
    debugln("[1.1 ETH::]Begin Connect");
    delay(2000);
    WiFi.onEvent(WiFiEvent); // Will call WiFiEvent() from another thread.
    ETH.begin();
}

void connectServer(void)
{
    clientIp.fromString(ip);
    serverIp.fromString(server);

    beginConnect();
    while (!client.connect(serverIp, port))
    {
        client.stop();
        beginConnect();
        debugln(F("[1.3 ERROR::]Cannot connect to host radio/server, Retrying...."));
        delay(SERVER_CONN_TIMEOUT);
    }
    debugln("[1.3 ETH::]Connected To: " + String(server) + ":" + String(port));

    client.setTimeout(ETH_CLENT_TIMEOUT);
}

// void readEthernet()
// {
//     while (client.available())
//     {
//         char c = client.read();
//         // debug(c);
//         if (c == '{')
//             insideBraces++;
//         if (insideBraces > 0)
//         {
//             // debug(c);
//             readBuffer[contentIndex++] = c;
//             if (c == '}')
//             {
//                 insideBraces--;
//                 if (insideBraces <= 0)
//                 {
//                     readBuffer[contentIndex] = '\0';
//                     // add buffer to the queue
//                     strncpy(parsedData[queue].parsedMsg, readBuffer, contentIndex);
//                     parsedData[queue].parselength = contentIndex;
//                     // debugln(parsedData[queue].parsedMsg);
//                     queue++;
//                     // clear buffer
//                     memset(readBuffer, 0, sizeof(readBuffer));
//                     contentIndex = 0;
//                 }
//             }
//         }
//     }
// }

void ethHandleLogs(const char *message, bool send)
{
    if (logsCounter >= LOGS_QUOTA)
        return;

    char prefix[8] = "RX::";
    if (send)
    {
        strcpy(prefix, "TX::");
    }
    uint32_t currentTime = millis() / 1000;
    snprintf(logsQueue[logsCounter], MAX_LOG_LENGTH, "%lu -> %s %s", currentTime, prefix, message);
    logsCounter++;
}