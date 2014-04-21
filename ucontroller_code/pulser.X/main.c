/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "shared.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
char light_state;
volatile char but0_press;
volatile char *i2c_data;
volatile char i2c_data_length;
volatile char ack_count;
volatile char i2c_in_use;
/*
 light_state contains which state the light is in
 0 = high
 1 = medium
 2 = low
 all other values not allowed
 */
/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    //configure ports for the device
    StartupConfig();

    //configure cap sense
    SetupCapSense();

    //setup the timer; enable output
    EnableOutput();

    while(1)
    {
        if (PORTAbits.RA4 == 1)
        {
            but0_press = 0;
            NextState();
        }
    }

}

