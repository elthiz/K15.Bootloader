#ifndef INC_SETTING_H_
#define INC_SETTING_H_

/* NodeID модуля в загрузчике */
#define NODE_ID							125
/* Адрес начала основной прошивки (из .ld скрипта) */
extern uint32_t _start_app;
/* Время, в течении которого модуль находится в bootloader */
#define TIME_STAY_ON_BOOT				2500
/* Приведенный адрес начала основной прошивки */
#define APP_START_ADDRESS 				(uint32_t) & _start_app
/* Количество страниц под стирание для основной прошивки (в Cortex-M0 память организована без секторов, только страницы по 2 КБ) */
#define APP_NUM_PAGES					120
/* Позиция бита в статусном регистре (оставаться в bootloader) */
#define MASK_STAY_BOOT 					0
/* Позиция бита в статусном регистре (новая часть прошивки (4 байта данных + 4 байта адреса)) */
#define MASK_NEW_MACHINE_WORD 			1
/* Позиция бита в статусном регистре (ошибка: попытка записи в bootloader) */
#define MASK_ERROR_WRITE_IN_BOOT 		2
/* Позиция бита в статусном регистре (ошибка: очистка памяти) */
#define MASK_ERROR_ERASE 				3
/* Позиция бита в статусном регистре (ошибка: записи прошивки) */
#define MASK_ERROR_WRITE 				4

#endif
