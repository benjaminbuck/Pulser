/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifdef __XC
    #include <xc.h>         /* XC8 General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"
#include "shared.h"
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitTimer(void)
{
    TRISAbits.TRISA5 = 1; //disable RA5 output by setting TRISA5 = 1
    PR2 = 0x35;//should be a freq of 150.94kHz
    PIE1bits.CCP1IE = 0;//disable interrupts
    PIR1bits.CCP1IF = 0;//disable interrupts
    CCP1CONbits.CCP1M = 0b1100;  //set LSB to 0, set P1A to active high
    CCP1CONbits.P1M = 0b00; //set single output (P1A)
    CCPR1L = 0x0C;  //MSB for 90% power
    CCP1CONbits.DC1B = 0b00; //set LSB to 0
    T2CONbits.TMR2ON = 1;//configures the timer2 to be on with no pre-or post scaler
    return;
}

void DisTimer(void)
{
    T2CONbits.TMR2ON = 0;//disables timer2, leaving other settings intact
    return;
}

void EnableOutput(void)
{
    if (T2CONbits.TMR2ON == 1)//check to see if the timer is running
    {
        TRISAbits.TRISA5 = 0;//enable RA5 output by setting TRISA5 = 0
    } else {
        InitTimer();//initialize the timer (should satisfy the first if statement)
        TRISAbits.TRISA5 = 0;//enable RA5 output by setting TRISA5 = 0
    }
    return;
}

void DisableOutput(void)
{
    TRISAbits.TRISA5 = 1;//disables output by setting TRISA5 = 1
    return;
}
/*
void ToggleOnOff(void)     //toggles PWM output
{
    if (TRISAbits.TRISA5 == 1)   //this means the output is disabled
    {
        EnableOutput();
    } else {
        DisableOutput();
    }
}
*/
void NextState(void)       //cycles to the next brightness state
{
    if (TRISAbits.TRISA5 == 0)
    {
        switch(light_state)
        {
            case 0: //0 is high
                CCPR1L = 0x0C;
                CCP1CONbits.DC1B = 0b00;
                //result is 0x30
                break;

            case 1: //1 is medium
                CCPR1L = 0x06;
                CCP1CONbits.DC1B = 0b10;
                //result is 0x1A;
                break;

            case 2: //2 is low
                CCPR1L = 0x02;
                CCP1CONbits.DC1B = 0b10;
                //result is 0x0A;
                break;

            default:
                CCPR1L = 0x0C;
                CCP1CONbits.DC1B = 0b00;
                //default state is on 90%
        }
    }
}


