#ifndef FREERTOS_LIB
#if defined(USE_MDR1986VE9x)

#include "critical.h"
#include <MDR32Fx.h>

static unsigned int ulNesting = 0;

void critical_enter( void )
{
  if(ulNesting == 0)
  {
    __disable_irq();
  }
  ulNesting++;
}

void critical_exit( void )
{
  ulNesting--;
  if(ulNesting == 0)
  {
    __enable_irq();
	}
}

#endif
#endif
