#ifndef CAN_USERCAN_H_
#define CAN_USERCAN_H_

#include <stdint.h>

#define CANOPEN_SCAN_MASTER			0x10
#define CANOPEN_OPERATION		0x05
#define CANOPEN_STOPPED			0x04
#define CANOPEN_PREOPERATION	0x7F

void usercanInit( uint8_t id );
void usercanProcess();

void usercanSetCanStatus( uint8_t stat );
uint8_t usercanGetCanStatus();


#endif /* CAN_USERCAN_H_ */
