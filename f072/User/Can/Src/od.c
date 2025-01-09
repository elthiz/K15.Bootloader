#include <od.h>
/* Инклюдим header где лежат нужные нам данные */
#include <moduledata.h>

#define OD_SIZE		( sizeof( objectDictionaryRecords ) / sizeof( ObjectDictionaryRecord ) )
/* Структура опысывающая запись объктного словаря */
typedef struct ObjectDictionaryRecord
{
	uint16_t index;
	uint8_t subIndex;
	uint8_t size;
	uint8_t* data;
} ObjectDictionaryRecord;

/* Создаем список записей объектного словаря */
const static ObjectDictionaryRecord objectDictionaryRecords[] =
{
	{ 0x1000,	0,		sizeof(canopendata.deviceType),				(uint8_t*)&canopendata.deviceType	},

	{ 0x2000,	0,		sizeof(userData.regControl),				(uint8_t*)&userData.regControl		},

	{ 0x2001,	0,		sizeof(userData.address),					(uint8_t*)&userData.address			},
	{ 0x2001,	1,		sizeof(userData.machineWord),				(uint8_t*)&userData.machineWord		},
};

uint16_t odGetData( uint16_t index, uint8_t subIndex, uint8_t* data )
{
	int left = 0;
	int right = OD_SIZE - 1;

	while ( left <= right )
	{
		int middle = left + ( right - left ) / 2;

		// Сначала сравниваем по `index`
		if ( objectDictionaryRecords[middle].index < index )
		{
			left = middle + 1;
		}
		else
		if ( objectDictionaryRecords[middle].index > index )
		{
			right = middle - 1;
		}
		else
		{
			// Если `index` совпадает, сравниваем `subIndex`
			if ( objectDictionaryRecords[middle].subIndex < subIndex )
			{
				left = middle + 1;
			}
			else
			if ( objectDictionaryRecords[middle].subIndex > subIndex )
			{
				right = middle - 1;
			}
			else
			{
				// Найдено совпадение `index` и `subIndex`
				for ( uint16_t j = 0; j < objectDictionaryRecords[middle].size; j++ )
				{
					data[j] = objectDictionaryRecords[middle].data[j];
				}
				return objectDictionaryRecords[middle].size;
			}
		}
	}
	// Совпадение не найдено
	return 0;
}

uint16_t odSetData( uint16_t index, uint8_t subIndex, uint8_t* data )
{
	int left = 0;
	int right = OD_SIZE - 1;

	while ( left <= right )
	{
		int middle = left + ( right - left ) / 2;

		// Сначала сравниваем по `index`
		if ( objectDictionaryRecords[middle].index < index )
		{
			left = middle + 1;
		}
		else
		if ( objectDictionaryRecords[middle].index > index )
		{
			right = middle - 1;
		}
		else
		{
			// Если `index` совпадает, сравниваем `subIndex`
			if ( objectDictionaryRecords[middle].subIndex < subIndex )
			{
				left = middle + 1;
			}
			else
			if ( objectDictionaryRecords[middle].subIndex > subIndex )
			{
				right = middle - 1;
			}
			else
			{
				// Найдено совпадение `index` и `subIndex`
				for ( uint16_t j = 0; j < objectDictionaryRecords[middle].size; j++ )
				{
					objectDictionaryRecords[middle].data[j] = data[j];
				}
				return objectDictionaryRecords[middle].size;
			}
		}
	}
	// Совпадение не найдено
	return 0;
}
