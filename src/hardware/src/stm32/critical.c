#if defined(STM32F1)||defined(STM32F3)||defined(STM32F4)

#include "critical.h"
#include <libopencm3/cm3/cortex.h>


static unsigned int ulNesting = 0;

void critical_enter( void )
{
    if(ulNesting == 0)
    {
		  cm_disable_interrupts();
    }
    ulNesting++;
}

void critical_exit( void )
{
  ulNesting--;
  if(ulNesting == 0)
  {
    cm_enable_interrupts();
	}
}

#endif