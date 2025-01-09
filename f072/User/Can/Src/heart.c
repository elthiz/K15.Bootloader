#include "heart.h"

#define HEARTBEAT_SIGN	0x700

void heartbeatProcess( uint8_t status, uint8_t nodeid, uint16_t* id, uint8_t* dlc, uint8_t* dataPtr )
{
	*dlc = 1;
	*id = HEARTBEAT_SIGN + nodeid;
	dataPtr[0] = status;
}
