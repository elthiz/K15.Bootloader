#ifndef CAN_NMT_H_
#define CAN_NMT_H_

#include <stdint.h>

#define CANOPEN_WAIT			0xFF
#define	CANOPEN_BOOTUP			0x00
#define CANOPEN_OPERATION		0x05
#define CANOPEN_STOPPED			0x04
#define CANOPEN_PREOPERATION	0x7F
#define CANOPEN_RESET			0x81

uint8_t nmtCheckSign( uint16_t id, uint8_t* dataPtr, uint8_t nodeID );
uint8_t nmtGetStatus( uint8_t* dataPtr );

#endif /* CAN_NMT_H_ */
