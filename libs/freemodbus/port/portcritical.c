/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include <libopencm3/cm3/cortex.h>

static ULONG    ulNesting;


void __critical_enter( void )
{
    if(ulNesting == 0)
    {
		  cm_disable_interrupts();
    }
    ulNesting++;
}

void __critical_exit( void )
{
  ulNesting--;
  if(ulNesting == 0)
  {
    cm_enable_interrupts();
	}
}
