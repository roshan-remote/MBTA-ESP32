#include "readTCP.h"
#include "ethSetup.h"
#include "config.h"
#include "parseRsp.h"

extern SemaphoreHandle_t xserialMonMutex;

uint8_t heartBeatStat = -1;

int readTCPMsg(char *msg, uint32_t len)
{
    DynamicJsonDocument rriJsonRsp(READ_BUF_SIZE);

    if (xSemaphoreTake(xserialMonMutex, portMAX_DELAY))
    {
        // debug(strlen(msg));
        debug(F("JSON RX::"));
        debugln(msg);
        xSemaphoreGive(xserialMonMutex);
    }
    ethHandleLogs(msg, false);

    DeserializationError error = deserializeJson(rriJsonRsp, msg, len);
    if (error)
    {
        debug(F("\n[5.ERROR::]Failed to parse JSON:"));
        debugln(error.c_str());
        return -1;
    }

    /*Turn Green LED ON*/
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDR, LOW);

    return (parseResponse(rriJsonRsp));
}
