#include "main.h"

#include "system.h"
#include "usercan.h"
#include "boot.h"
#include "setting.h"


int main(void)
{
	systemInit();
	usercanInit( NODE_ID );
	bootInit();

	while (1)
	{
		usercanProcess();
		bootProcess();
	}
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
