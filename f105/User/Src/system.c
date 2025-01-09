#include "system.h"

#include "can.h"
#include "gpio.h"

void SystemClock_Config( void );

void systemInit( void )
{
	HAL_Init();
	SystemClock_Config();
	MX_CAN1_Init();
}


void SystemClock_Config(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
	  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
	  {
	  }
	  LL_RCC_HSE_Enable();

	   /* Wait till HSE is ready */
	  while(LL_RCC_HSE_IsReady() != 1)
	  {

	  }
	  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
	  LL_RCC_PLL_Enable();

	   /* Wait till PLL is ready */
	  while(LL_RCC_PLL_IsReady() != 1)
	  {

	  }
	  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
	  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	   /* Wait till System clock is ready */
	  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
	  {

	  }
	  LL_SetSystemCoreClock(72000000);

	   /* Update the time base */
	  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
	  {
	    Error_Handler();
	  }
}
