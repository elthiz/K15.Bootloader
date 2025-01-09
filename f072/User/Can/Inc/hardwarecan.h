#ifndef CAN_HARDWARECAN_H_
#define CAN_HARDWARECAN_H_

#include <stdint.h>

/* Структура CAN-пакета
 * id: 		id пакета
 * dlc: 	количество байт данных
 * data:	данные */
typedef struct
{
	uint16_t id;
	uint8_t dlc;
	uint8_t data[8];
} HardwareCanFrame;

/* Перечисление скоростей CAN-шины */
typedef enum
{
	CAN_SPEED_1000K	= 0,
	CAN_SPEED_800K	= 1,
	CAN_SPEED_500K	= 2,
	CAN_SPEED_250K	= 3,
	CAN_SPEED_125K	= 4,
	CAN_SPEED_100K	= 5,
} HardwareCanSpeed;

void hardwarecanInit( HardwareCanSpeed canSpeed );
void hardwarecanFilterInit( uint8_t nodeID );

uint8_t hardwarecanCheckNewMessage();
void hardwarecanReceiveMessage( HardwareCanFrame* frame );
void hardwarecanTransmiteMessage( HardwareCanFrame* frame );

#endif /* CAN_HARDWARECAN_H_ */
