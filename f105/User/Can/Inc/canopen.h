#ifndef CAN_CANOPEN_H_
#define CAN_CANOPEN_H_

#include <stdint.h>

uint8_t canopenCheckSDO( uint16_t* id );
uint8_t canopenCheckPDO( uint16_t* id );
uint8_t canopenCheckHeartBeat( uint16_t* id );

uint8_t canopenProcessSDO( uint16_t* id, uint8_t* dlc, uint8_t* dataPtr );


#endif /* CAN_CANOPEN_H_ */
