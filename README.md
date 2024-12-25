# K15.Bootloader
Bootloader для линейки модулей K15. 
## Оглавление
0. [Цель](https://github.com/KastolomEngineering/K15.Bootloader/edit/f072/README.md#цель)
1. [Конфигурация периферии](https://github.com/KastolomEngineering/K15.Bootloader/edit/f072/README.md#конфигурация-периферии)
2. [Алгоритм работы](https://github.com/KastolomEngineering/K15.Bootloader/edit/f072/README.md#%D0%B0%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC-%D1%80%D0%B0%D0%B1%D0%BE%D1%82%D1%8B)
    1. [Включение](https://github.com/KastolomEngineering/K15.Bootloader/edit/f072/README.md#%D0%B2%D0%BA%D0%BB%D1%8E%D1%87%D0%B5%D0%BD%D0%B8%D0%B5)
    2. [Проверка приложения](https://github.com/KastolomEngineering/K15.Bootloader/edit/f072/README.md#%D0%B2%D0%BA%D0%BB%D1%8E%D1%87%D0%B5%D0%BD%D0%B8%D0%B5)
    3. [Переход к приложению](https://github.com/KastolomEngineering/K15.Bootloader/edit/f072/README.md#%D0%B2%D0%BA%D0%BB%D1%8E%D1%87%D0%B5%D0%BD%D0%B8%D0%B5)
    4. [Статусный регистр](https://github.com/KastolomEngineering/K15.Bootloader/edit/f072/README.md#%D0%B2%D0%BA%D0%BB%D1%8E%D1%87%D0%B5%D0%BD%D0%B8%D0%B5)
    5. [Запись новой прошивки](https://github.com/KastolomEngineering/K15.Bootloader/edit/f072/README.md#%D0%B2%D0%BA%D0%BB%D1%8E%D1%87%D0%B5%D0%BD%D0%B8%D0%B5)
3. [LD-скрипт загрузчика](https://github.com/KastolomEngineering/K15.Bootloader/edit/f072/README.md#%D0%B2%D0%BA%D0%BB%D1%8E%D1%87%D0%B5%D0%BD%D0%B8%D0%B5)
4. [LD-скрипт основного приложения](https://github.com/KastolomEngineering/K15.Bootloader/edit/f072/README.md#%D0%B2%D0%BA%D0%BB%D1%8E%D1%87%D0%B5%D0%BD%D0%B8%D0%B5)
5. [Таблица прерываний и указатель на неё](https://github.com/KastolomEngineering/K15.Bootloader/edit/f072/README.md#%D0%B2%D0%BA%D0%BB%D1%8E%D1%87%D0%B5%D0%BD%D0%B8%D0%B5)

## Цель
K15.Bootloader - загрузчик моделй K15 на семействе STM32F072 по CAN-шине, использующий стандартную реализацию протокола CanOpen. 

## Конфигурация периферии
![image](https://github.com/user-attachments/assets/b418d02b-f7b3-4991-8af1-e21de93c64ac)

*Прим.: индикация не реализована из-за особенностей схемотехники (на некоторых модулях светодиоды посажены на разные ножки gpio)*

## Алгоритм работы
### Включение
При включении, МК переходит в загрузчик. Внутри загрузчика МК находится 2500 мс. 
```c
/* Время, в течении которого модуль находится в bootloader */
#define TIME_STAY_ON_BOOT				2500
```
В течении этого времени МК переключает скорости свеого CAN-интерфейса, пытаясь отловить heartbeat от мастера. 

### Проверка приложения
По истечению времени ожидания МК проверяет сходится ли переменная, генерируемая в .ld-скрипте _estack (указывает на конец RAM) с тем, что лежит в начале прошивке. Значения должны сходится.
```c
uint8_t checkApp()
{
  extern void* _estack;
  if ( ( (void*)*( uint32_t* ) APP_START_ADDRESS ) != &_estack)
  {
    return 0;  // Проверка первого адреса прошивки, значение в нем должно быть размером RAM (регистр SP)
  }
  return 1;
}
```
Если проверка не пройдена, модуль остается в загрузчике.
### Переход к приложению
При переходе к приложению деинициализируется RCC и HAL. Таким образом сбрасывается периферия в состояние по умолчанию. После отключаются прерывания, устанавливается SP (stack pointer) и по указателю осуществляется переход в основное приложение.
```c
void ( *appPtr )() = ( void (*)() ) ( *( (uint32_t *)( APP_START_ADDRESS + 4 ) ) );
__set_MSP( *( (uint32_t *)APP_START_ADDRESS ) );

appPtr();
```

### Статусный регистр
Статусный регистр (далее СГ) используется для управлением загрузчика извне и контролем ошибок. Три старших бита зарезервированы. 
```c
/*Start:	0	0	0	0	0	0	0	0	*/
/*Start:	SB	NW	EB	EE	EW	0	0	0	*/
/* SB : STAY BOOT
 * NW : NEW MACHINE WORD
 * EB : ERROR BOOT
 * EE : ERROR ERASE
 * EW : ERROR WRITE
 *  */
uint8_t regControl;
```
0. SB - остаться в загрузчике.
1. NW - новые данные (4 байта адреса + 4 байта данных).
2. EB - попытка перезаписать загрузчик.
3. EE - неудачная очистка flash.
4. EW - неудачная запись.
*Прим.: при любой ошибки модуль остается в загрузчике и перестает реагировать на любые команды до перезагрузки*
Установка и чтение СГ доступны по CanOpen через объектый словарь. Индекс статусного регистра = 0x2000, Sub-индекс = 0x0, длина (в байтах) = 1.

### Запись новой прошивки
Для записи новой прошивки необходимо записать по CanOpen в объектный словарь модуля адрес и четыре байта данных (big-endian формат). 
```c
{ 0x2001, 0, sizeof(userData.address),       (uint8_t*)&userData.address        },
{ 0x2001, 1, sizeof(userData.machineWord),   (uint8_t*)&userData.machineWord    },
```
Индекс адреса = 0x2001, Sub-индекс = 0x0, длина (в байтах) = 4;

Индекс данных = 0x2001, Sub-индекс = 0x1, длина (в байтах) = 4;

После успешной передачи адреса и данных необходимо в NW бит внутри СГ установить 1. МК начнет запись. После успешной записи бит NW сбросится в 0.

При завершении передачи прошивки необходимо установить бит SB внутри СГ в 0 и модуль перейдет в основное приложение.

## LD-скрипт загрузчика
Загрузчик занимает 16KB и размещается в начале flash. Также добавлены две переменные, указывающее на начало основного приложения и его конец.
```c
/* Memories definition */
MEMORY
{
  RAM    		(xrw)   : ORIGIN = 0x20000000,   LENGTH = 16K
  BOOTLOADER    (rx)    : ORIGIN = 0x8000000,    LENGTH = 16K
  APP		    (rx)    : ORIGIN = 0x8004000,    LENGTH = 64K-16K
}

_start_app = ORIGIN(APP);
_end_app = ORIGIN(APP) + LENGTH(APP);
```

## LD-скрипт основного приложения
Основное приложение занимает всю свободную память после загрузчика (64KB-16KB=48KB).
```c
/* Memories definition */
MEMORY
{
  RAM    (xrw)    : ORIGIN = 0x20000000,   LENGTH = 16K
  FLASH    (rx)    : ORIGIN = 0x8004000,   LENGTH = 64K-16K
}
_start_app = ORIGIN(APP);
```
Из-за отсутствия регистра VTOR внутри Cortex M0 необходимо использовать переносить таблицу прерываний из прошивки основного приложения в RAM. Для этого в LD-скрипте после секции .fini-array добавлено:
```c
.ram_vector :
{
  *(.ram_vector)
} >RAM
```

## Таблица прерываний и указатель на неё
В основном приложении первым делым необходимо перенести таблицу прерываний в RAM. Размер таблицы прерываний для STM32F072 равняется 48. Адрес начала таблицы прерываний самый первый в прошивке основного приложения. Запись таблицы прерываний ведется в секцию .ram_vector (находится в RAM).
```c
extern uint32_t _start_app;
#define VECTOR_TABLE_SIZE 48
#define APP_START_ADDRESS  (uint32_t) & _start_app
volatile uint32_t* vectorTableApp = (volatile uint32_t*) APP_START_ADDRESS;
volatile uint32_t __attribute__((section(".ram_vector,\"aw\",%nobits @"))) ram_vector[VECTOR_TABLE_SIZE];
```
Код перенос таблицы прерываний из flash в RAM
```c
for (uint32_t i = 0; i < VECTOR_TABLE_SIZE; i++)
{
  ram_vector[i] = vectorTableApp[i];
}
__HAL_RCC_SYSCFG_CLK_ENABLE();
__HAL_SYSCFG_REMAPMEMORY_SRAM();
__HAL_RCC_SYSCFG_CLK_DISABLE();
```

# Всё!
