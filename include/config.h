#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define FIMWARE_VER "V2-C3-231218"

/*
    0 - Disable Serial Monitor
    1 - Enable Serial Monitor
*/
#define DEBUG 1
#define PROD

// Server params
#define BTN_UP_PIN BTN_USER
#define RED_LED_PIN LEDR
// const PROGMEM uint8_t BTN_DOWN_PIN = D7;

// define pinouts for button and the LED
#ifdef PROD
#define ip "192.168.250.178"
#define server "192.168.250.2"
#define port 61001
#else
#define ip "192.168.10.168"
#define server "192.168.10.140"
#define port 55555
#endif

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

// white list IPs for web server connections

#define WEB_SERVER_CRED "YWRtaW46YWRtaW4=" // YWRtaW46YWRtaW4= = 'admin:admin' (user:password) base64 encoded

#define LOGS_QUOTA 16
#define MAX_LOG_LENGTH 2048

#define RADIO_CMDS_DELAY 800 // Time distance between the radio commands

/*These Values are in miliseconds Used for thread sleep */
#define KEEP_ALIVE_TIM 12000
#define WATCH_DOG_TIM 60000
#define STATUS_LED_TIM 250
#define PROCESS_TCP_TIM 250
#define PANEL_TIM 250

#define READ_ETH_TIM 50
#define WEB_SERVE_TIM 50

/*These Values are in Miliseconds used for other purposes */
#define DEBOUNCE_TIME 500
#define STARTUP_DELAY 10000 // STARTUP_DELAY for radio
#define WEB_UPDATE_VAL 1500 //  1500
#define SERVER_CONN_TIMEOUT 3000
#define ETH_CLENT_TIMEOUT 100
#define BROWSER_REC_TIME 1

#define READ_BUF_SIZE 5000

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