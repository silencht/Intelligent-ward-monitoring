#include "system.h"

//-----------------------------------------------------------------------------
void SystemInit(void) 
{
  // no initialization required
}

//-----------------------------------------------------------------------------
void DelayMicroSeconds(u32t nbrOfUs)   /* -- adapt this delay for your uC -- */
{
  u32t i;
  for(i = 0; i < nbrOfUs; i++)
  {
    __nop();  // nop's may be added or removed for timing adjustment
    __nop();
    __nop();
    __nop();
  }
}
