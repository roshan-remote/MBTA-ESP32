#ifndef READ_TCP_H
#define READ_TCP_H
#include "Arduino.h"
/**
 * Function Name: readTCPMsg
 *
 * Description:
 *   Read a TCP message from the Radio
 *
 * Parameters:
 *   msg - JSON message to read
 *  len - Number of bytes to read
 *
 * Returns:
 *  int - value based on machine state
 *  OR error codes from the Radio interface
 **/
int readTCPMsg( char *msg, uint32_t len);

#endif