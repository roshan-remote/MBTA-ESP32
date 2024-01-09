#ifndef FILEHANDLE_H
#define FILEHANDLE_H

#include <FS.h> //This should be at first
#include <LittleFS.h>
#define CONFIG_FILE "/config.json"

bool saveConfigFile(void);
bool loadConfigFile(void);

extern uint32_t resetCount;

#endif