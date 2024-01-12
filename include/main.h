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
uint8_t watchDogCount = 0;

bool msgReceived = false;
bool pauseKeepAlive = false;

bool completeSysList = false;
uint16_t sysCount = 0;
extern uint16_t systemInfoSize;
extern SystemInfo systemInfo[MAX_SYSTEMS];

// variables for EthernetClient interface and handlers

static QueueHandle_t ethReadQueue;

// char readBuffer[READ_BUF_SIZE];
// const static int queue = 0;
// extern ParsedData parsedData[3];

char logsQueue[LOGS_QUOTA][MAX_LOG_LENGTH];
uint8_t logsCounter = 0;

// Mutex serialMonMutex, ethClientMutex;

SemaphoreHandle_t xserialMonMutex; // Declare a mutex variable

#endif