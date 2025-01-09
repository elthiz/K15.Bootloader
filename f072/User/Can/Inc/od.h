#ifndef CAN_OD_H_
#define CAN_OD_H_

#include "stdint.h"

uint16_t odGetData( uint16_t index, uint8_t subIndex, uint8_t* data );
uint16_t odSetData(uint16_t index, uint8_t subIndex, uint8_t* data );

#endif /* CAN_OD_H_ */
