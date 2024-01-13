#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "webServer.h"
#include "config.h"
#include "webHandler.h"
#include "images/logo_L3H.h"
#include "images/logo_MBTA.h"
// #include "images/logo_SST.h"
// #include "images/logo_SST2.h"
#include "images/logo_SST3.h"
#include "images/button_1.h"
#include "images/switch_1.h"
#include "images/switch_2.h"
#include "images/favicon.h"

#include "web/css.h"
#include "web/script.h"
#include "web/webPageStatic.h"
#include "fileHandle.h"

#ifdef SERVE_WIFI
#include <WiFi.h>
#endif

extern uint16_t maxChannelSize;

WiFiServer webServer(80);

extern char logsQueue[LOGS_QUOTA][MAX_LOG_LENGTH];
extern uint8_t logsCounter;

extern bool completeSysList;
extern char mMissionName[64];

const uint32_t MAX_REQUEST_SIZE = 1024;
char HTTP_req[MAX_REQUEST_SIZE] = "";

uint32_t HTTP_req_count = 0;
#define MAX_VALUE_LENGTH 50

uint8_t channel = 1;
uint32_t resetCount;

uint16_t sysChannel = 2;

void setWebServer(void)
{
#ifdef SERVE_WIFI
    WiFi.softAP(WIFI_SSID, WIFI_PASS, 1, true, 4);
    debugln("Soft AP started");
    debug("IP Address: ");
    debugln(WiFi.softAPIP());
#endif
    webServer.begin();
    loadConfigFile();
}

void parseRequest(const char *request, char delimiter, char *result)
{
    // Find the position of '='
    char *equalSignPosition = strchr(request, '=');

    if (equalSignPosition != NULL)
    {
        // Find the position of ' 'or given delimiter after '='
        char *endPosition = strchr(equalSignPosition, delimiter);

        if (endPosition != NULL)
        {
            int start_index = (int)(equalSignPosition - HTTP_req) + 1;
            int end_index = (int)(endPosition - HTTP_req) - 1;

            int slice_length = end_index - start_index + 1;

            if (slice_length >= MAX_VALUE_LENGTH)
            {
                debugln("Value length exceeds maximum allowed.");
                result[0] = '\0'; // Return an empty string in case of an error
                return;
            }
            // Allocate space for the sliced string

            // Copy the sliced portion using memcpy
            memcpy(result, HTTP_req + start_index, slice_length);
            result[slice_length] = '\0'; // Null-terminate the sliced string

            // Print or use the extracted value
            // debugln(String(result));
        }
        else
        {
            // Handle the case where ' ' is not found after '='
            debugln("Invalid or incomplete system parameter in the URL.");
            result[0] = '\0'; // Return an empty string in case of an error
        }
    }
    else
    {
        // Handle the case where '=' is not found
        debugln("Delimiter sign not found in the URL.");
        result[0] = '\0'; // Return an empty string in case of an error
    }
}

void showSystemReport(WiFiClient &cl)
{
    cl.print(" <div class='spinner-2'>Loading...</div>");
}

// logging data to the browser
void logger(WiFiClient &cl)
{
    for (uint8_t i = 0; i < logsCounter; i++)
    {
        cl.print(logsQueue[i]);
        cl.print("<br>");
    }
    logsCounter = 0;
}

// send the state of the switch to the web browser
void GetAjaxData(WiFiClient &cl)
{
    TaskStatus_t *taskStatusArray;
    volatile UBaseType_t taskCount, i;

    // Allocate an array to hold the task status information
    taskStatusArray = (TaskStatus_t *)pvPortMalloc(sizeof(TaskStatus_t) * uxTaskGetNumberOfTasks());

    char ajaxBuffer[2048];

    sprintf(ajaxBuffer,
            "<b><center><h2>Run<span id='red'>Time </span> %lu seconds</h2></center></b>"
            "<h3><center><span id='green'>Radio Parameters</span></center></h3>"
            "<center><table border='2'>"
            "<tr><th>SN</th><th>Params</th><th>Value</th></tr>"
            "<tr><td>1</td><td>ACKs</td><td>%lu</td></tr>"
            "<tr><td>2</td><td>NACKs</td><td>%lu</td></tr>"
            "<tr><td>3</td><td>TX KeepAlives</td><td>%lu</td></tr>"
            "<tr><td>4</td><td>RX KeepAlives</td><td>%lu</td></tr>"
            "<tr><td>5</td><td>Radio-Server Conn</td><td>%lu</td></tr>"
            "</center></table>"
            "<br><h3><span id='green'><center>Heap Parameters</center></span></h3>"
            "<center><table border='2'>"
            "<tr><th>SN</th><th>Params</th><th>Value</th></tr>"
            "<tr><td>1</td><td> <span id='red'>Available Heap</span></td><td>%lu</td></tr>"
            "<tr><td>2</td><td>Total Heap Size</td><td>%lu</td></tr>"
            "<tr><td>3</td><td>Lowest Free Heap since boot</td><td>%lu</td></tr>"
            "</center></table><br><br",
            (millis() / 1000), radioACK, radioNACK, KASent, connect, radioServerConn, ESP.getFreeHeap(),
            ESP.getHeapSize(), ESP.getMinFreeHeap(), ESP.getMaxAllocHeap());

    // if (taskStatusArray != NULL)
    // {
    //     // Get the task status information
    //     taskCount = uxTaskGetSystemState(taskStatusArray, uxTaskGetNumberOfTasks(), NULL);
    //     strcat(ajaxBuffer,
    //            "<br><h3>Task<span id='red'>Monitor</span></span></h3>"
    //            "<table><tr><th>SN</th><th>Task</th><th>Status</th><th>Priority</th>"
    //            "<th>Stack Size</th><th>Num</th></tr>");

    //     for (i = 0; i < taskCount; i++)
    //     {
    //         sprintf(ajaxBuffer + strlen(ajaxBuffer),
    //                 "<tr><td>%s</td>"
    //                 "<td>%lu</td>"
    //                 "<td>%lu</td>"
    //                 "<td>%d</td>"
    //                 "<td>%lu</td>"
    //                 "</tr>",
    //                 i + 1,
    //                 taskStatusArray[i].pcTaskName,
    //                 taskStatusArray[i].xHandle,
    //                 taskStatusArray[i].uxCurrentPriority,
    //                 taskStatusArray[i].usStackHighWaterMark);
    //     }
    //     vPortFree(taskStatusArray);
    // }
    cl.write(ajaxBuffer, strlen(ajaxBuffer));
}

void sendWebContent(WiFiClient &cl)
{
    // HTTP request for web page
    // send web page - contains JavaScript with AJAX calls
    cl.println(F("<!DOCTYPE html>"));
    cl.println(F("<html>"));
    cl.println(F("<head>"));
    cl.println(F("<title>MBTA RailNet</title>"));
    cl.println(F("<link rel = 'icon' type = 'image/png' sizes = '32x32' href ="));
    cl.println(FAVICON);
    cl.println(F("/>"));
    cl.println(F("<meta name='viewport' content='width=device-width, initial-scale=1.0'/>"));

    cl.println(F("<style>"));
    cl.write(webCSS, sizeof(webCSS));
    cl.println(F("</style></head>"));

    cl.println(F("<body><div class='header'>"));

    cl.println(F("<img src="));
    cl.write(MBTA_LOGO);
    cl.println(F(">"));
    cl.println(F("<h1><center><b>Rail<span id='red'>Comm</span></b></center></h1>"));
    cl.println(F("<h1><center><b>Functional<span id='red'>Protoype</span></b></center></h1>"));
    cl.println(F("<img src="));
    cl.write(L3H_LOGO);
    cl.println(F("></div>"));

    cl.println(F("<img align='right' class='mirrored' id='switch=2' src="));
    cl.write(SWITCH_2);
    cl.println(F(">"));

    cl.println(F("<img align='right' class='mirrored' id='switch=1' src="));
    cl.write(SWITCH_1);
    cl.println(F(">"));

    cl.println(F("<img align='right' class='t-image'  id='channel=down' src="));
    cl.write(BUTTON_1);
    cl.println(F(">"));

    cl.println(F("<img align='right' class='t-image'  id='channel=up' src="));
    cl.write(BUTTON_1);
    cl.println(F(">"));

    cl.println(F("<br><br><br><b><button id='reset' class='custom-button custom-red'>Reset</button></b>"));

    if (completeSysList == false)
        cl.println(F("<b><button id='systemList' class='custom-button'>Read XL-200 Mission</button></b>"));
    cl.println(F("<br><b><button id='subscribeRSSI' class='custom-button'>Subscribe to RSSI</button></b>"));
    cl.println(F("<b><button id='heartbeatStatus' class='custom-button'>Heartbeat status</button></b><br>"));
    for (int i = 0; i < 4; i++)
    {
        cl.print(F("<b><button id='relay="));
        cl.print(i);
        cl.print(F("'class='custom-button'>RELAY "));
        cl.print(i);
        cl.println(F("</button></b>"));
    }
    cl.print(F("<br><br><br></h3><hr>"));

    cl.println(F("<body onload='initPage()'>"));
    cl.print(F("<center>"));
    cl.print(F("<h3>Firmware<span id='red'>Version</span> "));
    cl.print(FIMWARE_VER);
    cl.print(F("<br>Reset<span id='red'>Count</span> "));
    cl.print(resetCount);
    cl.println(F("</h3><hr>"));

    cl.println(F("<h3><span id='green'>Chip Parameters</span></h3><table border='2'>"));
    cl.print(F("<tr><th>SN</th><th>Params</th><th>Value</th></tr>"));
    cl.print(F("<tr><td>1</td><td>Flash Size</td><td>"));
    cl.print((ESP.getFlashChipSize() / 1024.0) / 1024.0);
    cl.println(F("MB</td></tr>"));
    cl.print(F("<tr><td>2</td><td>Flash Speed</td><td>"));
    cl.print(ESP.getFlashChipSpeed() / 1000000);
    cl.println(F("MHz</td></tr>"));
    cl.print(F("<tr><td>3</td><td>Sketch Size</td><td>"));
    cl.print((ESP.getSketchSize() / 1024.0) / 1024.0);
    cl.println(F("MB</td></tr>"));
    cl.print(F("<tr><td>4</td><td>Free Sketch Size</td><td>"));
    cl.print((ESP.getFreeSketchSpace() / 1024.0) / 1024.0);
    cl.println(F("MB</td></tr>"));
    cl.print(F("<tr><td>5</td><td>CPU Frequency</td><td>"));
    cl.print(ESP.getCpuFreqMHz());
    cl.println(F("MHz</td></tr>"));
    cl.print(F("<tr><td>6</td><td>Chip Model</td><td>"));
    cl.print(ESP.getChipModel());
    cl.println(F("</td></tr>"));
    cl.print(F("<tr><td>7</td><td>SDK version</td><td>"));
    cl.print(ESP.getSdkVersion());
    cl.println(F("</td></tr></table><br>"));

    if (completeSysList == false)
    {
        cl.println(F("<div id='systemTable'></div>"));
    }
    else
    {
        completeSysList = false;
        cl.print(F("<h2>Mission<span id='red'>Control</span></h2>"
                   "<b>Mission<span id='red'>Name </span>"));
        cl.print(mMissionName);
        cl.print(F("<br>Total<span id='red'>Systems </span>"));
        cl.print(systemInfoSize);
        cl.print(F("<br>Total<span id='red'>Channels </span>"));
        uint16_t sum = 0;
        for (uint16_t i = 0; i < systemInfoSize; i++)
            sum = sum + systemInfo[i].channelSize;
        cl.print(sum);
        cl.print(F("<br></b>"));

        cl.print("<table class='transposed-table'><tr>"
                 //  "<br><th>System Index</th><th>Type</th><th>ID</th><th>Short Alias</th>");
                 "<br><th>System Index</th><th>Type</th><th>Short Alias</th>");
        debugln("MAXCHAN=" + String(maxChannelSize));
        for (uint16_t i = 0; i < maxChannelSize; i++)
        {
            cl.print("<th>Channel ");
            cl.print(i + 1);
            cl.print("</th>");
        }
        cl.print("</tr>");

        for (uint16_t i = 0; i < systemInfoSize; i++)
        {
            cl.print(F("<tr><td>"));
            cl.print(systemInfo[i].systemIndex);
            cl.print(F("</td><td>"));
            cl.print(systemInfo[i].systemType);
            // cl.print("</td><td>");
            // cl.print(systemInfo[i].unitID);

            cl.print(F("</td><td><button class='custom-button' id='system="));
            cl.print(systemInfo[i].shortAlias);
            // cl.print("&channel=1'>");
            cl.print(F("'>"));
            cl.print(systemInfo[i].shortAlias);
            cl.print(F("</button></td>"));

            for (uint16_t j = 0; j < systemInfo[i].channelSize; j++) // This is for the channels data
            {
                // debugln(systemInfo[i].channels[j]);
                cl.print(F("<td>"
                           "<button  class='custom-button' id='system="));
                cl.print(systemInfo[i].shortAlias);
                cl.print(F("&channel="));
                cl.print(j + 1);
                cl.print(F("'>"));
                cl.print(systemInfo[i].channels[j]);
                cl.print(F("</button></td>"));
            }
            // delete[] systemInfo[i].channels;
            cl.print(F("</tr>"));
        }
        cl.print(F("</table>"));
    }

    cl.println(F("<div id='sw_an_data'></div>"));

    cl.println(F("<hr><h3>XL-Link<span id='red'>Log</span></h3></center>"));
    cl.println(F("<hr><div class='loggerBox' id='webLogger'></div>"));
    cl.println(F("<center><button id='goToBottomBtn' onclick='goToBottom()''>Go to Bottom</button></center><hr>"));

    cl.println(F("<script>"));
    cl.write(webScript, strlen(webScript));
    cl.println(F("</script>"));

    cl.println(F("</body></br><div class='header'>"));
    cl.println(F("<h3><left><span id='red'>Thank You</span> for visiting</left></h3>"));
    cl.print(F("<img src="));
    cl.write(SST3_LOGO);
    cl.println(F(">"));
    cl.println(F("</html>"));
}

void handleAuthorizedRequest(WiFiClient &cl)
{
    // send a standard http response header
    cl.println(F("HTTP/1.1 200 OK"));
    cl.println(F("Content-Type: text/html"));
    cl.println(F("Connection: keep-alive"));
    cl.println();

    // AJAX request for switch state
    if (strstr(HTTP_req, "ajax_switch") != NULL)
    {
        if (completeSysList)
        {
            cl.write(webRefresh);
        }
        else
        {
            GetAjaxData(cl); // read switch state and analog input
        }
    }

    else if (strstr(HTTP_req, "web_logger") != NULL)
    {
        logger(cl);
    }

    else if (strstr(HTTP_req, "systemList") != NULL)
    {
        handleSystemList();
        showSystemReport(cl);
    }
    else if (strstr(HTTP_req, "reset") != NULL)
    {
        resetCount++;
        saveConfigFile();
        resetOPTA();
    }
    else if (strstr(HTTP_req, "subscribeRSSI") != NULL)
    {
        handleSubscribeRSSI();
    }
    else if (strstr(HTTP_req, "heartbeatStatus") != NULL)
    {
        handleHeartbeatStatus();
    }
    else
    {
        const char *result = nullptr;
        char value[MAX_VALUE_LENGTH];

        if ((result = strstr(HTTP_req, "system=")) != NULL)
        {
            parseRequest(result, ' ', value);
            if (strstr(value, "&") != NULL)
            {
                parseRequest(result, '&', value);
                webSystemChange(value);
                delay(RADIO_CMDS_DELAY);
            }
            else
                webSystemChange(value);
            for (uint16_t i = 0; i < systemInfoSize; i++)
            {
                if (systemInfo[i].shortAlias == String(value))
                {
                    sysChannel = systemInfo[i].channelSize;
                    // debugln(value + ':' + String(sysChannel));
                }
            }
        }
        else if ((result = strstr(HTTP_req, "relay=")) != NULL)
        {
            parseRequest(result, ' ', value);
            relayControl(atoi(value));
        }
        else if ((result = strstr(HTTP_req, "switch=")) != NULL)
        {
            parseRequest(result, ' ', value);
            switcher(atoi(value));
        }
        if ((result = strstr(HTTP_req, "channel=")) != NULL)
        {
            parseRequest(result, ' ', value);
            if (strcmp(value, "up") == 0)
            {
                channel = ++channel > sysChannel ? 1 : channel;
                webChannelChange(channel);
            }
            else if (strcmp(value, "down") == 0)
            {
                channel = --channel <= 0 ? sysChannel : channel;
                webChannelChange(channel);
            }
            else
                webChannelChange(atoi(value));
        }
        if (result == nullptr)
        {
            sendWebContent(cl);
        }
    }
}

void showWebPage(void)
{
    WiFiClient webClient = webServer.available();
    if (!webClient)
    {
        return;
    }
    // debugln(webClient.remoteIP());
    bool currentLineIsBlank = true;
    while (webClient.connected())
    {
        // debugln("Running web page");
        if (webClient.available())
        {
            char c = webClient.read();
            // Serial.write(c);
            HTTP_req[HTTP_req_count] = c;
            HTTP_req_count++;
            if (c == '\n' && currentLineIsBlank)
            {
                if (strstr(HTTP_req, WEB_SERVER_CRED) != NULL)
                    handleAuthorizedRequest(webClient);
                else
                {
                    webClient.println(F("HTTP/1.1 401 Unauthorized"));
                    webClient.println(F("WWW-Authenticate: Basic realm='Secure'"));
                    webClient.println(F("Content-Type: text/html"));
                    webClient.println();
                }
                memset(HTTP_req, 0, sizeof(HTTP_req));
                HTTP_req_count = 0;
                break;
            }
            if (c == '\n')
                currentLineIsBlank = true;
            else if (c != '\r')
                currentLineIsBlank = false;
        }
    }
    delay(BROWSER_REC_TIME);
    webClient.stop();
}
