#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define FIMWARE_VER "V5"

/*
    0 - Disable Serial Monitor
    1 - Enable Serial Monitor
*/

// define pinouts for button and the LED
// const PROGMEM uint8_t BTN_DOWN_PIN = D7;

// Server params
#ifdef PROD
#define IP "192.168.250.178"
#define SERVER "192.168.250.2"
#define GATEWAY "192.168.250.1"
#define port 61001
#else
#define IP "192.168.10.168"
#define SERVER "192.168.10.140"
#define GATEWAY "192.168.10.1"
#define port 55555
#endif

#define SUBNET "255.255.0.0"

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

// white list IPs for web server connections

// WiFi  IP address 192.168.4.1
#define WIFI_SSID "MBTA-ESP"
#define WIFI_PASS "HackMeNot"

// #define WEB_SERVER_CRED "YWRtaW46YWRtaW4=" // YWRtaW46YWRtaW4= = 'admin:admin' (user:password) base64 encoded
#define WEB_SERVER_CRED "UmFpbENvbW06SGFja01lTm90" // 'RailComm:HackMeNot' (user:password) base64 encoded

#define MAX_SYSTEMS 16

#define WEB_LOG

#ifdef WEB_LOG
#define LOGS_QUOTA 10
#define MAX_LOG_LENGTH 2048
#else
#define LOGS_QUOTA 1
#define MAX_LOG_LENGTH 1
#endif

#define RADIO_CMDS_DELAY 500 // Time distance between the radio commands

/*These Values are in miliseconds Used for thread sleep */
#define KEEP_ALIVE_TIM 12000
#define WATCH_DOG_TIM 60000
#define STATUS_LED_TIM 250
#define PROCESS_TCP_TIM 250
#define PANEL_TIM 250

#define READ_ETH_TIM 100
#define WEB_SERVE_TIM 100

/*These Values are in Miliseconds used for other purposes */
#define DEBOUNCE_TIME 500
#define STARTUP_DELAY 10000 // STARTUP_DELAY for radio
#define WEB_UPDATE_VAL 1500 //  1500
#define SERVER_CONN_TIMEOUT 3000
#define ETH_CLENT_TIMEOUT 100
#define BROWSER_REC_TIME 1

#define READ_BUF_SIZE 7000

#define LED_USER 12
#define LEDG 13
#define LEDR 14

/*Radio Params*/
#define RRI_VERSION 0.1
#define RRI_OPTION 2
#define ESN "A40200000003"
const PROGMEM bool useAudio = false;

#define KACOUNT 3 // Keep Alive maximum Counter

/*States with Radio*/
enum
{
    DEFAULT_1,
    RADIO_ACK,
    RADIO_NACK,
    PING,
    CONNECT,
    AUTH,
    AUDIO_SET,
    VCH_OK,
    VCH_FAIL,
    LISTEN,
    SYS_LIST
};

#endif