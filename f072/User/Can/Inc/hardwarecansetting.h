#ifndef CAN_HARDWARECANSETTING_H_
#define CAN_HARDWARECANSETTING_H_

#include "can.h"

/* Настройка скоростей CAN-интерфейса.  */
/* TODO: Необходимо заменить на свои значения prescaler, segment 1, segment 2, исходя из частоты ядра */
/* Скорость 1000Kb */
#define PRESCALER_1000 	3
#define SEGMENT_1_1000 	CAN_BS1_13TQ
#define SEGMENT_2_1000 	CAN_BS2_2TQ
/* Скорость 800Kb */
#define PRESCALER_800 	4
#define SEGMENT_1_800 	CAN_BS1_12TQ
#define SEGMENT_2_800 	CAN_BS2_2TQ
/* Скорость 500Kb */
#define PRESCALER_500 	6
#define SEGMENT_1_500 	CAN_BS1_13TQ
#define SEGMENT_2_500 	CAN_BS2_2TQ
/* Скорость 250Kb */
#define PRESCALER_250 	12
#define SEGMENT_1_250 	CAN_BS1_13TQ
#define SEGMENT_2_250 	CAN_BS2_2TQ
/* Скорость 125Kb */
#define PRESCALER_125 	24
#define SEGMENT_1_125 	CAN_BS1_13TQ
#define SEGMENT_2_125 	CAN_BS2_2TQ
/* Скорость 100Kb */
#define PRESCALER_100 	30
#define SEGMENT_1_100 	CAN_BS1_13TQ
#define SEGMENT_2_100 	CAN_BS2_2TQ

/* Указатель на структуру CAN-интерфейса.  */
/* TODO: Необходимо заменить на свою структуру CAN-интерфейса */
static CAN_HandleTypeDef *hcanl = &hcan;

#endif /* CAN_HARDWARECANSETTING_H_ */
