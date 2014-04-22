/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifdef __XC
    #include <xc.h>         /* XC8 General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"

/* Refer to the device datasheet for information about available
oscillator configurations and to compiler documentation for macro details. */
void ConfigureOscillator(void)
{

    /* Typical actions in this function are to tweak the oscillator tuning
    register, select new clock sources, and to wait until new clock sources
    are stable before resuming execution of the main project. */
    //setup OSCCON
    OSCCONbits.SPLLEN = 1; //4x PLL is enabled (also enabled in the configuration words)
    OSCCONbits.IRCF = 0b1110;
    //results in 32MHz clock frequency
    //now wait until the HFOSC is fully running
    while (OSCSTATbits.HFIOFL == 0 || OSCSTATbits.HFIOFR == 0)
    {
        NOP();
    }

    return;

}

void StartupConfig(void)
{

    TRISAbits.TRISA4 = 1;//RA4 is an input
    ANSELAbits.ANSA4 = 0;//RA4 is used as an dig

    TRISAbits.TRISA5 = 1;//RA5 is an output, but we let the timer change this bit
    TRISAbits.TRISA3 = 1;//RA3 is MCRL, always an input
    TRISAbits.TRISA2 = 1;//RA2 is SDA, but should be set to input (see 25.4.3)
    TRISAbits.TRISA1 = 1;//RA1 is SCL, but should be set to input (see 25.4.3)
    TRISAbits.TRISA0 = 1;//RA0 is not used

    APFCONbits.CCP1SEL = 1;//the CCP1 is on the RA5 out, not RA2

    //enable interrupts for change on in/out pins.
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.IOCIE = 1;


    //enable negative edge interrupt generation for RA4
    IOCAN = 0b00010000; //falling edge detection on RA4
    IOCAP = 0;  //no rising edge detection

}
