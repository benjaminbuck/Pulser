/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#ifdef __XC
    #include <xc.h>         /* XC8 General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "shared.h"

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/


void interrupt isr(void)
{
    /* This code stub shows general interrupt handling.  Note that these
    conditional statements are not handled within 3 seperate if blocks.
    Do not use a seperate if block for each interrupt flag to avoid run
    time errors. */
    if (IOCAF4 = 1)
    {
        but0_press = 1;
    }

}


