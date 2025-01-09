#ifndef CAN_HEART_H_
#define CAN_HEART_H_

#include <stdint.h>

void heartbeatProcess( uint8_t status, uint8_t nodeid, uint16_t* id, uint8_t* dlc, uint8_t* dataPtr );

#endif /* CAN_HEART_H_ */
