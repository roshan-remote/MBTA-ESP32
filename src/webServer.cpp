
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

void setWebServer(void)
{
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
    char ajaxBuffer[2048];

    sprintf(ajaxBuffer,
            "<b><center><h3><span id='blue'>Runtime Stats @</span> %lu seconds</h3></center></b>"
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
            "<tr><td>4</td><td>PSRAM Size</td><td>%lu</td></tr>"
            "<tr><td>5</td><td>Free PSRAM Size</td><td>%lu</td></tr>"
            "<tr><td>6</td><td>Minimum Free PSRAM </td><td>%lu</td></tr>"
            "<tr><td>7</td><td>Max Allocated PSRAM</td><td>%lu</td></tr>"
            "</center></table><br><br",
            (millis() / 1000), radioACK, radioNACK, KASent, connect, radioServerConn, ESP.getFreeHeap(),
            ESP.getHeapSize(), ESP.getMinFreeHeap(), ESP.getMaxAllocHeap(), ESP.getPsramSize(),
            ESP.getFreePsram(), ESP.getMinFreePsram(), ESP.getMaxAllocPsram());

    cl.write(ajaxBuffer, strlen(ajaxBuffer));
}

void sendWebContent(WiFiClient &cl)
{
    // HTTP request for web page
    // send web page - contains JavaScript with AJAX calls
    cl.println("<!DOCTYPE html>");
    cl.println("<html>");
    cl.println("<head>");
    cl.println("<title>MBTA RailNet</title>");
    cl.println("<link rel = 'icon' type = 'image/png' sizes = '32x32' href =");
    cl.println(FAVICON);
    cl.println("/>");
    cl.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'/>");

    cl.println("<style>");
    cl.write(webCSS, sizeof(webCSS));
    cl.println("</style></head>");

    cl.println("<body><div class='header'>");

    cl.println("<img src=");
    cl.write(MBTA_LOGO);
    cl.println(">");
    cl.println("<h1><center><b>Welcome to Rail<span id='red'>Net</span></b></center></h1>");
    cl.println("<img src=");
    cl.write(L3H_LOGO);
    cl.println("></div>");

    cl.println("<img align='right' class='mirrored' id='switch=2' src=");
    cl.write(SWITCH_2);
    cl.println(">");

    cl.println("<img align='right' class='mirrored' id='switch=1' src=");
    cl.write(SWITCH_1);
    cl.println(">");

    cl.println("<img align='right' class='t-image'  id='channel=down' src=");
    cl.write(BUTTON_1);
    cl.println(">");

    cl.println("<img align='right' class='t-image'  id='channel=up' src=");
    cl.write(BUTTON_1);
    cl.println(">");

    cl.println("<br><br><br><b><button id='reset' class='custom-button custom-red'>Reset</button></b>");

    if (completeSysList == false)
        cl.println("<b><button id='systemList' class='custom-button'>List Systems</button></b>");

    cl.println("<b><button id='subscribeRSSI' class='custom-button'>Subscribe to RSSI</button></b>");
    cl.println("<b><button id='heartbeatStatus' class='custom-button'>Heartbeat status</button></b>");
    cl.print("<br><br><br></h3><hr>");

    cl.println("<body onload='initPage()'>");
    cl.print("<center>");
    cl.print("<h3><span id='green'>Fimware Version: </span> ");
    cl.print(FIMWARE_VER);
    cl.print("<br>Reset Count: ");
    cl.print(resetCount);
    cl.println("</h3><hr>");

    cl.println("<h3><span id='green'>Chip Parameters</span></h3><table border='2'>");
    cl.print("<tr><th>SN</th><th>Params</th><th>Value</th></tr>");
    cl.print("<tr><td>1</td><td>Flash Size</td><td>");
    cl.print((ESP.getFlashChipSize() / 1024.0) / 1024.0);
    cl.println("MB</td></tr>");
    cl.print("<tr><td>2</td><td>Flash Speed</td><td>");
    cl.print(ESP.getFlashChipSpeed() / 1000000);
    cl.println("MHz</td></tr>");
    cl.print("<tr><td>3</td><td>Sketch Size</td><td>");
    cl.print((ESP.getSketchSize() / 1024.0) / 1024.0);
    cl.println("MB</td></tr>");
    cl.print("<tr><td>4</td><td>Free Sketch Size</td><td>");
    cl.print((ESP.getFreeSketchSpace() / 1024.0) / 1024.0);
    cl.println("MB</td></tr>");
    cl.print("<tr><td>5</td><td>CPU Frequency</td><td>");
    cl.print(ESP.getCpuFreqMHz());
    cl.println("MHz</td></tr>");
    cl.print("<tr><td>6</td><td>Chip Model</td><td>");
    cl.print(ESP.getChipModel());
    cl.println("</td></tr>");
    cl.print("<tr><td>7</td><td>SDK version</td><td>");
    cl.print(ESP.getSdkVersion());
    cl.println("</td></tr>");
    cl.println("</table><br>");

    if (completeSysList == false)
    {
        cl.println("<div id='systemTable'></div>");
    }
    else
    {
        completeSysList = false;
        cl.print("<h3><span id='green'>System Report list</span></h3>"
                 "<b>Mission Name:</b>");
        cl.print(mMissionName);
        cl.print("<table class='transposed-table'><tr>"
                 "<br><th>System Index</th><th>Type</th><th>ID</th><th>Short Alias</th>");
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
            cl.print("<tr><td>");
            cl.print(systemInfo[i].systemIndex);
            cl.print("</td><td>");
            cl.print(systemInfo[i].systemType);
            cl.print("</td><td>");
            cl.print(systemInfo[i].unitID);

            cl.print("</td><td><button class='custom-button' id='system=");
            cl.print(systemInfo[i].shortAlias);
            cl.print("&channel=1'>");
            cl.print(systemInfo[i].shortAlias);
            cl.print("</button></td>");

            for (uint16_t j = 0; j < systemInfo[i].channelSize; j++) // This is for the channels data
            {
                // debugln(systemInfo[i].channels[j]);
                cl.print("<td>"
                         "<button  class='custom-button' id='system=");
                cl.print(systemInfo[i].shortAlias);
                cl.print("&channel=");
                cl.print(j + 1);
                cl.print("'>");
                cl.print(systemInfo[i].channels[j]);
                cl.print("</button></td>");
            }
            cl.print("</tr>");
        }
        cl.print("</table>");
    }

    cl.println("<div id='sw_an_data'></div>");
    cl.println("<hr><h2>Logs</h2></center>");
    cl.println("<hr><div class='loggerBox' id='webLogger'></div><hr>");

    cl.println("<script>");
    cl.write(webScript, strlen(webScript));
    cl.println("</script>");

    cl.println("</body></br><div class='header'>");
    cl.println("<h3><left><span id='red'>Thank You</span> for visiting</left></h3>");
    cl.print("<img src=");
    cl.write(SST3_LOGO);
    cl.println(">");
    cl.println("</html>");
}

void handleAuthorizedRequest(WiFiClient &cl)
{
    // send a standard http response header
    cl.println("HTTP/1.1 200 OK");
    cl.println("Content-Type: text/html");
    cl.println("Connection: keep-alive");
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
                channel = ++channel > maxChannelSize ? 1 : channel;
                webChannelChange(channel);
            }
            else if (strcmp(value, "down") == 0)
            {
                channel = --channel <= 0 ? maxChannelSize : channel;
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
                    webClient.println("HTTP/1.1 401 Unauthorized");
                    webClient.println("WWW-Authenticate: Basic realm='Secure'");
                    webClient.println("Content-Type: text/html");
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
