#ifndef MAIN_H
#define MAIN_H

#include "readTCP.h"
#include "sendTCP.h"
#include "radio.h"
#include "webServer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

WiFiClient client;

uint16_t channelID = 0;
uint8_t disconn = 0;
uint8_t state;
bool pingReceived = false;
bool leadCar = LOW; // for lead car and remote car states

bool channelUp = false;
bool channelDown = false;
bool statusLed = false;
uint32_t KASent = 0;
uint32_t connect = 0;
uint32_t radioACK = 0;
uint32_t radioNACK = 0;
uint32_t radioServerConn = 0;

uint8_t KATaskCount = 0;
uint8_t watchDogCount = 1;

bool msgReceived = false;
bool pauseKeepAlive = true;

bool completeSysList = false;
uint16_t sysCount = 0;
extern uint16_t systemInfoSize;
extern SystemInfo systemInfo[MAX_SYSTEMS];

QueueHandle_t ethReadQueue;

char logsQueue[LOGS_QUOTA][MAX_LOG_LENGTH];
uint8_t logsCounter = 0;

SemaphoreHandle_t xserialMonMutex; // Declare a mutex variable

#endif