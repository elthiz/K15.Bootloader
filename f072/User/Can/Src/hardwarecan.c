#include "hardwarecan.h"

#include "canopen.h"
#include "can.h"
#include <string.h>
#include "hardwarecansetting.h"

/* Структура делителей CAN-шины */
typedef struct
{
	uint8_t prescaler;
	uint32_t seg1;
	uint32_t seg2;
} CanPrescaler;

/* Заполняем набор делителей для разные скоростей */
CanPrescaler canPrescalers[] =
{
	{ PRESCALER_1000,  	SEGMENT_1_1000,  	SEGMENT_2_1000 },	// Скорость 1000Kb при частоте ядра 40Мгц
	{ PRESCALER_800,  	SEGMENT_1_800,  	SEGMENT_2_800 },	// Скорость 800Kb при частоте ядра 40Мгц
	{ PRESCALER_500,  	SEGMENT_1_500,  	SEGMENT_2_500 },	// Скорость 500Kb при частоте ядра 40Мгц
	{ PRESCALER_250,  	SEGMENT_1_250,  	SEGMENT_2_250 },	// Скорость 250Kb при частоте ядра 40Мгц
	{ PRESCALER_125, 	SEGMENT_1_125,  	SEGMENT_2_125 },	// Скорость 125Kb при частоте ядра 40Мгц
	{ PRESCALER_100, 	SEGMENT_1_100,  	SEGMENT_2_100 }	// Скорость 100Kb при частоте ядра 40Мгц
};

/* Создаем структуры для принятия и отправки пакетов CAN */
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
uint32_t TxMailBox = 0;

/* Инициализируем CAN шину */
void hardwarecanInit( HardwareCanSpeed canSpeed )
{
	//HAL_CAN_Stop(hcanl);

	hcanl->Instance = CAN;
	hcanl->Init.Mode = CAN_MODE_NORMAL;
	hcanl->Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcanl->Init.TimeTriggeredMode = DISABLE;
	hcanl->Init.AutoBusOff = DISABLE;
	hcanl->Init.AutoWakeUp = DISABLE;
	hcanl->Init.AutoRetransmission = DISABLE;
	hcanl->Init.ReceiveFifoLocked = ENABLE;
	hcanl->Init.TransmitFifoPriority = ENABLE;

	hcanl->Init.Prescaler = canPrescalers[canSpeed].prescaler;
	hcanl->Init.TimeSeg1 = canPrescalers[canSpeed].seg1;
	hcanl->Init.TimeSeg2 = canPrescalers[canSpeed].seg2;

	if (HAL_CAN_Init(hcanl) != HAL_OK) { Error_Handler(); }

	HAL_CAN_Start(hcanl);
}

/* Инициализируем фильтра */
void hardwarecanFilterInit( uint8_t nodeID )
{
	/* Обязательно static, чтобы компилятор не затирал оптимизацией */
	static CAN_FilterTypeDef sFilterConfig;

	/* Фильтр на HeartBeat от Master устройства */
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
	sFilterConfig.FilterIdHigh = 0x77F << 5;
	sFilterConfig.FilterIdLow = 0;
	sFilterConfig.FilterMaskIdHigh = 0x77F << 5;
	sFilterConfig.FilterMaskIdLow = 0;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	if (HAL_CAN_ConfigFilter(hcanl, &sFilterConfig) != HAL_OK) { Error_Handler(); }

	/* Фильтр на SDO запросы */
	sFilterConfig.FilterBank = 1;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
	sFilterConfig.FilterIdHigh = ( 0x600 + nodeID ) << 5;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = ( 0x600 + nodeID ) << 5;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	if (HAL_CAN_ConfigFilter(hcanl, &sFilterConfig) != HAL_OK) { Error_Handler(); }

	/* Фильтр на SDO запросы */
	sFilterConfig.FilterBank = 2;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
	sFilterConfig.FilterIdHigh = ( 0x700 + nodeID ) << 5;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = ( 0x700 + nodeID ) << 5;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	if (HAL_CAN_ConfigFilter(hcanl, &sFilterConfig) != HAL_OK) { Error_Handler(); }
}

/* Функция проверки входящих сообщений */
uint8_t hardwarecanCheckNewMessage()
{
	/* Возращаем кол-во сообщенией находящихся в буффере */
	return HAL_CAN_GetRxFifoFillLevel( hcanl, CAN_RX_FIFO0 );
}

/* Процедура приема сообщения */
void hardwarecanReceiveMessage( HardwareCanFrame* frame )
{
	HAL_CAN_GetRxMessage(hcanl, CAN_RX_FIFO0, &RxHeader, frame->data);
	frame->id = (uint16_t)RxHeader.StdId;
	frame->dlc = (uint8_t)RxHeader.DLC;
}

/* Процедура отправки сообщения */
void hardwarecanTransmiteMessage( HardwareCanFrame* frame )
{
	static HardwareCanFrame frameTX = {};
	memcpy( (void*)&frameTX, (void*)frame, sizeof(HardwareCanFrame) );

	/* Ждем пока освободится место для нового сообщения */
	while( HAL_CAN_GetTxMailboxesFreeLevel(hcanl) == 0) {};
	/* Заполняем структуру для отправки кадра CAN */
	TxHeader.StdId = frame->id;
	TxHeader.DLC = frame->dlc;
	/* Добавляем новое сообщение в очередь */
	HAL_CAN_AddTxMessage(hcanl, &TxHeader, frameTX.data, &TxMailBox);
}

