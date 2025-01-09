#ifndef INC_MODULEDATA_H_
#define INC_MODULEDATA_H_

#include <stdint.h>

typedef struct ModuleData
{

} ModuleData;

typedef struct UserData
{
	/*Start:	0	0	0	0	0	0	0	0	*/
	/*Start:	SB	NW	EB	EE	EW	0	0	0	*/
	/* SB : STAY BOOT
	 * NW : NEW MACHINE WORD
	 * EB : ERROR BOOT (попытка перезаписать загрузчик)
	 * EE : ERROR ERASE
	 * EW : ERROR WRITE
	 *  */
	uint8_t regControl;
	uint32_t machineWord;
	uint32_t address;
} UserData;

typedef struct CanopenData
{
	uint32_t deviceType;
} CanopenData;


extern ModuleData moduleData;
extern UserData userData;
extern CanopenData canopendata;

#endif
