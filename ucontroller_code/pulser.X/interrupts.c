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
    if (IOCAFbits.IOCAF4 == 1)
    {
        IOCAF = IOCAF && 0b00101111;//set IOCAF4 to zero
        but0_press = 0x00;
    } else if (PIR1bits.SSP1IF == 1) {
    ///////I2C ISR
        PIR1bits.SSP1IF = 0;    //clear the interrupt flag

        //accumulate the ACK (if = 1, then ACK not received)
        ack_count += SSP1CON2bits.ACKSTAT;

        if (i2c_data_length == 0)
        {
            if (i2c_in_use == 1)
            {
                SSP1CON2bits.PEN = 1;
                i2c_in_use = 2; //2 indicates that a stop condition has been issued
            } else if (i2c_in_use == 2)
            {
                i2c_in_use = 0;
            }

            //now we decrement the ack_count by the ACKSTAT to overcome the problem
            //that the ISR enters twice at the end but the ACK bit doesn't change
            ack_count -= SSP1CON2bits.ACKSTAT;

        } else {
            SSP1BUF = *i2c_data;
            i2c_data_length--;  //decrement the number of data left in the array
            i2c_data++; //increment the address of the array
        }
    } else {
        CCPR1L = 0x0D;
        CCP1CONbits.DC1B = 0b00;
    }
}


