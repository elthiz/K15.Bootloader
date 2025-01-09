#include <usercan.h>

#include "hardwarecan.h"
#include "canopen.h"
#include "nmt.h"
#include "heart.h"

#include "main.h"
#include "moduledata.h"

#define TIMEOUT_SPEED	200

uint8_t nodeId = 0;
uint8_t status = CANOPEN_SCAN_MASTER;
uint8_t speed = CAN_SPEED_1000K;

HardwareCanFrame canFrame = {0};

void usercanInit( uint8_t id )
{
	canopendata.deviceType = 0x00010191;

	nodeId = id;

	hardwarecanInit( CAN_SPEED_1000K );
	hardwarecanFilterInit( nodeId );
}

void usercanProcess()
{
	static uint32_t timeoutScan = 0;

	if ( status == CANOPEN_SCAN_MASTER )
	{
		if ( hardwarecanCheckNewMessage() )
		{
			hardwarecanReceiveMessage( &canFrame );
			if ( canopenCheckHeartBeat( &canFrame.id ) )
			{
				status = CANOPEN_OPERATION;
				heartbeatProcess( status, nodeId, &canFrame.id, &canFrame.dlc, canFrame.data );
				hardwarecanTransmiteMessage(&canFrame);
			}
		}
		if ( HAL_GetTick() > timeoutScan )
		{
			speed = ( speed + 1 ) % 6;
			hardwarecanInit( speed );
			timeoutScan = HAL_GetTick() + TIMEOUT_SPEED;
		}
	}

	if ( status == CANOPEN_OPERATION )
	{
		if ( hardwarecanCheckNewMessage() )
		{
			hardwarecanReceiveMessage( &canFrame );
			if( canopenCheckSDO( &canFrame.id ) )
			{
				if( canopenProcessSDO( &canFrame.id, &canFrame.dlc, canFrame.data ) ) { hardwarecanTransmiteMessage(&canFrame); }
			}
		}
	}
}
