#include "main.h"
#include "systems.h"

// void panelTask(void *parameter);
void procTCPMsgTask(void *parameter);
void sendKATask(void *parameter);
void readEthTask(void *parameter);

void setup()
{
  // put your setup code here, to run once:
#if DEBUG == 1
  Serial.begin(115200);
#endif

  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);

  // Create a mutex
  xserialMonMutex = xSemaphoreCreateMutex();
  connectServer(); // connect to the Radio server
  setWebServer();  // initialise the web server

  /*Start the necessary threads*/
  xTaskCreate(procTCPMsgTask,
              "TCPMsg",
              4096,
              nullptr,
              1,
              nullptr);
  xTaskCreate(sendKATask,
              "KeepAlive",
              4096,
              nullptr,
              1,
              nullptr);
  xTaskCreate(readEthTask,
              "ReadEth",
              4096,
              nullptr,
              1,
              nullptr);
}

void switchStates(int responseCode)
{
  switch (responseCode)
  {
  case CONNECT:
    /*Connection Sucessful*/
    auth();
    state = CONNECT;
    break;

  case AUTH:
    /*Authorisation Sucessful*/
    state = AUTH;
    setAudio(useAudio);
    break;

  case AUDIO_SET:
    /*Audio set sucessful and Connection Accepted*/
    state = AUDIO_SET;
    radioServerConn++;
    vchConnect();

    break;

  case VCH_OK:
    state = LISTEN;
    break;

  case LISTEN:
    /*Single run in LISTEN state*/
    // debugln(F("Listening"));
    break;

  case VCH_FAIL:
    client.stop();
    // reconnect
    state = CONNECT;
    connectServer();
    break;

  case PING:
    /*Ping Response Received */
    pingReceived = true;
    connect++; // keep alive Tx
    disconn = 0;
    if (xSemaphoreTake(xserialMonMutex, portMAX_DELAY))
    {
      debug(F("[KeepAlive Count::] "));
      debugln(connect);
      xSemaphoreGive(xserialMonMutex);
    }
    break;

  case RADIO_ACK:
    /*When radio ACK is received*/
    radioACK++;
    break;

  case RADIO_NACK:
    /*When radio NACK is received*/
    radioNACK++;
    break;

  case SYS_LIST:
    if (sysCount < systemInfoSize)
    {
      pauseKeepAlive =true;
      if (systemInfo[sysCount].systemType == 0 || systemInfo[sysCount].systemType == 5)
        requestForConventionalFrequencySet(systemInfo[sysCount].shortAlias.c_str());
      else
        groupListReport(systemInfo[sysCount].shortAlias.c_str());
      sysCount++;
      // delay(RADIO_CMDS_DELAY);
    }
    else
    {
      pauseKeepAlive =false;
      state = LISTEN;
      sysCount = 0;
      completeSysList = true;
    }
    break;
  }
}

void procTCPMsgTask(void *parameter)
{
  for (;;)
  {
    if (queue > 0)
    {
      // process queue one by one
      --queue;
      int responseCode = readTCPMsg(parsedData[queue].parsedMsg, parsedData[queue].parselength);
      memset(parsedData[queue].parsedMsg, 0, sizeof(parsedData[queue].parsedMsg));
      switchStates(responseCode);
      if (!msgReceived)
        msgReceived = true;
    }
    else
      msgReceived = false;

    vTaskDelay(pdMS_TO_TICKS(PROCESS_TCP_TIM));
  }
}

void loop()
{
  showWebPage();
  vTaskDelay(pdMS_TO_TICKS(WEB_SERVE_TIM));
}

void sendKATask(void *parameter)
{
  for (;;)
  {
    vTaskDelay(pdMS_TO_TICKS(STATUS_LED_TIM));
    KATaskCount++;

    /* 1. Watch Dog Timer*/
    if (!msgReceived)
    {
      watchDogCount++;
      if (WATCH_DOG_TIM / watchDogCount <= 0)
      {
        debugln("[WatchDog::]Count=" + watchDogCount);
        debugln(F("[6.1 ERROR::]Triggering watchdog timer"));
        ESP.restart();
      }
    }

    /* 2. Status LED control*/
    if (state == LISTEN)
    {
      digitalWrite(LED_USER, statusLed);
      statusLed = !statusLed;
    }

    /* 3. Keep-alive logic and control*/
    if (KEEP_ALIVE_TIM / (STATUS_LED_TIM * KATaskCount) <= 0 && !pauseKeepAlive)
    {
      if (!pingReceived)
      {
        disconn++;
        if (disconn >= KACOUNT)
        {
          client.stop();
          digitalWrite(LED_USER, LOW); // Turn off the Status LED
          debugln(F("[6.2 ERROR::]Connection Timed out !"));
          disconn = 0;
          state = CONNECT;
          connectServer();
        }
        else
        {
          keepAlive();
          KASent++;
        }
      }
      else
      {
        pingReceived = false;
        keepAlive();
        KASent++;
      }
      KATaskCount = 0;
    }
  }
} // sendKATask

void readEthTask(void *parameter) // For reading TCP msg from the radio
{
  for (;;)
  {
    readEthernet();
    vTaskDelay(pdMS_TO_TICKS(READ_ETH_TIM));
  }
}
