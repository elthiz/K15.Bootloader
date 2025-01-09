#include "nmt.h"

#include <main.h>

#define NMT_SIGN	0x00

#define NMT_OPERATION		0x01
#define NMT_STOPPED			0x02
#define NMT_PREOPERATION	0x80
#define NMT_RESET			0x81

#define BROADCAST_ADR		0x00

uint8_t nmtCheckSign( uint16_t id, uint8_t* dataPtr, uint8_t nodeID )
{
	if ( ( id == NMT_SIGN ) && ( ( dataPtr[1] == BROADCAST_ADR ) || ( dataPtr[1] == nodeID ) ) )
	{
		return 1;
	}
	return 0;
}

uint8_t nmtGetStatus( uint8_t* dataPtr )
{
	if( dataPtr[0] == NMT_OPERATION ) { return CANOPEN_OPERATION; }
	else
	if( dataPtr[0] == NMT_STOPPED ) { return CANOPEN_STOPPED; }
	else
	if( dataPtr[0] == NMT_PREOPERATION ) { return CANOPEN_PREOPERATION; }
	else
	if( dataPtr[0] == NMT_RESET ) { return CANOPEN_RESET; }

	return CANOPEN_RESET;
}
