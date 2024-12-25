#include "canopen.h"

#include <stddef.h>
#include "od.h"

#define MASK_SIGN					0xFFFFFF80
#define MASK_USER_ADR				0x7F

#define MASTER_SIGN					0x77F

#define SDO_SIGN					0x600
#define SDO_SIGN_WRITE				0x02
#define SDO_SIGN_READ				0x04
#define SDO_WRITE_SUCCES_RESULT		0x06
#define SDO_READ_SUCCES_RESULT		0x04
#define SDO_ERROR_RESULT			0x08
#define SDO_SIGN_ANSWER				0x580

typedef struct
{
	uint8_t other	: 2;
	uint8_t nonByte	: 2;
	uint8_t css		: 4;
} CanopenControlByte;

uint8_t canopenCheckSDO( uint16_t* id )
{
	uint16_t sdoCmd = *id & MASK_SIGN;
	if( sdoCmd == SDO_SIGN )
	{
		return 1;
	}
	return 0;
}

uint8_t canopenCheckHeartBeat( uint16_t* id )
{
	if( *id == MASTER_SIGN ) { return 1; }
	return 0;
}

uint8_t canopenProcessSDO( uint16_t* id, uint8_t* dlc, uint8_t* dataPtr )
{
	/* Передаем указалье на первый байт пакета CAN */
	CanopenControlByte* control = (CanopenControlByte*)dataPtr;
	/* Получаем адрес индекса */
	uint16_t index = ( dataPtr[2] << 8 ) | ( dataPtr[1] );
	/* Получаем адрес допольнительного индекса */
	uint8_t subIndex = ( (dataPtr[3] >> 4) * 10 ) + ( dataPtr[3] & 0x0F );
	/* Зполняем неиспользуемые биты */
	control->other = 3;

	*id = *id & MASK_USER_ADR;
	*id += SDO_SIGN_ANSWER;

	uint16_t sizeData = 0;

	if( control->css == SDO_SIGN_READ )
	{
		sizeData = odGetData( index, subIndex, &dataPtr[4] );
		/* Читаем данные через функцию словаря */
		if( sizeData )
		{
			/* Записываем кол-во НЕ байт данных */
			control->nonByte = 4 - sizeData;
			/* Если запрос на чтение прошел */
			control->css = SDO_READ_SUCCES_RESULT;
		}
		else
		{
			/* Если запрос на чтение не прошел */
			control->css = SDO_ERROR_RESULT;
		}
		return 1;
	}
	else
	if( control->css == SDO_SIGN_WRITE )
	{
		sizeData = odSetData(index, subIndex, &dataPtr[4]);
		/* Записываем данные через функцию словаря */
		if( sizeData )
		{
			/* Записываем кол-во НЕ байт данных */
			control->nonByte = 4 - sizeData;
			/* Если запрос на запись прошел */
			control->css = SDO_WRITE_SUCCES_RESULT;
		}
		else
		{
			/* Если запрос на запись не прошел */
			control->css = SDO_ERROR_RESULT;
		}
		return 1;
	}

	return 0;
}

