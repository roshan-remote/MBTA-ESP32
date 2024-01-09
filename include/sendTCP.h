#ifndef SEND_TCP_H
#define SEND_TCP_H

#include "ethSetup.h"

/**
 * Function Name: sendTCPMsg
 *
 * Description:
 *   send a TCP message to the Radio
 *
 * Parameters:
 * reference to a JSON document
 *
 * Returns:
 *  boolean indicating success or failure
 *
 **/
bool sendTCPMsg(JsonDocument &rriJson);

#endif