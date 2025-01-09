#include "boot.h"

#include "main.h"

#include "moduledata.h"
#include "setting.h"

/* Функция перехода в основное приложение */
void jumpToApp( void );
/* Функция проверки времени нахождения в bootloader */
uint8_t checkTimeoutBootloader();
/* Функция записи */
void writeSingleWord();
/* Функция очисти памяти */
uint8_t eraseFlash();

uint8_t isAppValid();

void bootInit( void )
{

}

void bootProcess( void )
{
	if ( ( 1 << MASK_ERROR_WRITE_IN_BOOT ) & userData.regControl )
	{
		return;
	}

	if ( ( 1 << MASK_ERROR_ERASE ) & userData.regControl )
	{
		return;
	}

	if ( ( 1 << MASK_ERROR_WRITE ) & userData.regControl )
	{
		return;
	}

	if ( !( ( 1 << MASK_STAY_BOOT ) & userData.regControl ) )
	{
		if ( checkTimeoutBootloader() )
		{
			jumpToApp();
		}
	}

	if ( ( 1 << MASK_NEW_MACHINE_WORD ) & userData.regControl )
	{
		writeSingleWord();
	}
}

uint8_t eraseFlash()
{
	HAL_FLASH_Unlock();
	uint32_t pageError = 0;
	FLASH_EraseInitTypeDef eraseInitStruct;
	eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; // Тип: стирание страниц
	eraseInitStruct.PageAddress = APP_START_ADDRESS;   // Адрес начала страницы
	eraseInitStruct.NbPages = APP_NUM_PAGES;		   // Кол-во страниц
	/* Если очистить флеш не получилось */
	if (HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) != HAL_OK)
	{
		/* Устанавливаем флаг ошибки */
		userData.regControl |= (1 << MASK_ERROR_ERASE);
		HAL_FLASH_Lock();
		return 0;
	}
	HAL_FLASH_Lock();
	return 1;
}

void writeSingleWord()
{
	static uint32_t address = 0;
	static uint32_t machineWord = 0;
	static uint8_t isFlashErased = 0;

	/* Достаем адрес из OD */
	address = userData.address;
	/* Достаем данные из OD */
	machineWord = userData.machineWord;
	/* Если адрес попадает в диапазон адресов загрузчика */
	if ( address < APP_START_ADDRESS )
	{
		/* Устанавливаем флаг ошибки */
		userData.regControl |= (1 << MASK_ERROR_WRITE_IN_BOOT);
		return;
	}
	/* Если флеш еще не очищена */
	if ( !isFlashErased )
	{
		if ( eraseFlash() )
		{
			isFlashErased = 1;
		}
	}

	HAL_FLASH_Unlock();
	/* Переводим данные в Little-endian формат */
	uint32_t machineWordLE =
			((machineWord >> 24) & 0xFF) |
			((machineWord >> 8) & 0xFF00) |
			((machineWord << 8) & 0xFF0000) |
			((machineWord << 24) & 0xFF000000);
	/* Если записать данные не получилось */
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, machineWordLE) != HAL_OK)
	{
		/* Устанавливаем флаг ошибки */
		userData.regControl |= (1 << MASK_ERROR_WRITE);
		HAL_FLASH_Lock();

		return;
	}
	HAL_FLASH_Lock();
	/* Обнуляем флаг новых данных */
	userData.regControl &= ~(1 << MASK_NEW_MACHINE_WORD);
}

uint8_t checkTimeoutBootloader()
{
	static uint8_t isStartWait = 0;
	static uint32_t timeStayOnBoot = 0;

	if ( !isStartWait )
	{
		timeStayOnBoot = HAL_GetTick();
		isStartWait = 1;
		return 0;
	}

	if ( HAL_GetTick() >= timeStayOnBoot + TIME_STAY_ON_BOOT )
	{
		return 1;
	}
	return 0;
}

uint8_t isAppValid()
{
	extern void* _estack;
	if ( ( (void*)*( uint32_t* ) APP_START_ADDRESS ) != &_estack)
	{
		return 0;  // Проверка первого адреса прошивки, значение в нем должно быть размером RAM (регистр SP)
	}
	return 1;
}

void jumpToApp( void )
{
	if ( !isAppValid() )
	{
		userData.regControl |= (1 << MASK_STAY_BOOT);
		return;
	}
	/* Деиницилизируем периферию и HAL */
	HAL_RCC_DeInit();
	HAL_DeInit();

	__disable_irq();

	/* Сбрасываем SysTick. На всякий (SysTick отключается в HAL_DeInit) */
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;

	/* Выставляем stack pointer */
	__set_MSP(*(uint32_t *)APP_START_ADDRESS);

	/* Magic! Получаем указатель на reset_handler основного приложения */
	void (*appPtr)() = (void (*)())(*(uint32_t *)(APP_START_ADDRESS + 4));

	/* Просим, чтобы все операции были завершены */
	__ISB();
	__DSB();

	appPtr();

	/* До этого кода мы никогда не дойдем */
	while(1) { }
}

