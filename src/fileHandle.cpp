#include "fileHandle.h"
#include "config.h"
#include "ArduinoJson.h"

bool saveConfigFile(void)
{
    debugln("Saving configuration...");

    DynamicJsonDocument configJson(128);
    configJson["reset_count"] = resetCount;

    File configFile = SPIFFS.open(CONFIG_FILE, "w");

    if (!configFile)
    {
        debugln("\nfailed to open config file for writing");
        return 0;
    }

    if (serializeJson(configJson, configFile) == 0)
    {
        debugln("\nFailed to write to file");
        return 0;
    }

    debugln("\nWrote Sucessfully!");

    // Close file
    configFile.close();
    return 1;
}

bool loadConfigFile(void)
{

    debugln("\nMounting File System...");

    if (!(SPIFFS.begin(false) || SPIFFS.begin(true)))
    {
        debugln("\nFailed to mount FS");
        return 0;
    }

    if (!(SPIFFS.exists(CONFIG_FILE)))
    {
        debugln("Failed to load json config");
        return 0;
    }

    debugln("\nThe file exists and reading config file\n");

    File configFile = SPIFFS.open(CONFIG_FILE, "r");

    if (!configFile)
        return 0;

    DynamicJsonDocument configJson(128);
    DeserializationError error = deserializeJson(configJson, configFile);

    if (error)
    {
        debugln("Failed to parse JSON: " + String(error.c_str()));
        return 0;
    }
    resetCount = configJson["reset_count"].as<int>();
    return 1;
}
