#include "sendTCP.h"
#include "config.h"

extern SemaphoreHandle_t xserialMonMutex;

void turnRedLED()
{
    digitalWrite(LEDG, LOW);  // turn off green
    digitalWrite(LEDR, HIGH); // turn on LED
}

bool sendTCPMsg(JsonDocument &rriJson)
{

    // Populate the JSON document with authorization data
    const size_t maxJsonLength = 2048;
    char jsonBuffer[maxJsonLength];

    // Serialize the JSON document to the char array (JSON string)
    size_t jsonLength = serializeJson(rriJson, jsonBuffer, maxJsonLength);

    // Create a buffer to hold the header and JSON data
    uint8_t buffer[4 + jsonLength]; // 4 bytes for the header

    // Convert priority to network byte order (big-endian)
    uint16_t priority = 0x0001;

    // Copy the header bytes to the buffer
    buffer[0] = (priority >> 8) & 0xFF;
    buffer[1] = priority & 0xFF;
    buffer[2] = (jsonLength >> 8) & 0xFF;
    buffer[3] = jsonLength & 0xFF;

    // Copy the JSON data to the buffer
    memcpy(&buffer[4], jsonBuffer, jsonLength);
    // Send the buffer to the radio

    client.write(buffer, sizeof(buffer));

    delay(10);

    // Dump the buffer in serial monitor
    if (xSemaphoreTake(xserialMonMutex, portMAX_DELAY))
    {
        // debug(strlen(msg));
        debug(F("JSON TX::"));
        debugln(jsonBuffer);
        xSemaphoreGive(xserialMonMutex);
    }
#ifdef WEB_LOG
    ethHandleLogs(jsonBuffer, true);
#endif

    /*Orange color to send*/
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDR, HIGH);

    rriJson.clear(); // clear JSON document
    return true;
}