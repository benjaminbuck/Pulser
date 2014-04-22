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
#define _XTAL_FREQ 32000000
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
    CCPR1L = 0x0C;  //MSB for 22% power
    CCP1CONbits.DC1B = 0b00; //set LSB to 0
    T2CONbits.TMR2ON = 1;//configures the timer2 to be on with no pre-or post scaler
    return;
}

void InitMSSP(void)
{
    //enable interrupts for perifs
    PIE1bits.SSP1IE = 1; //MSSP interrupt
    PIE2bits.BCL1IE = 1; //bus collision for MSSP

    //configure baud rate generator
    SSP1ADDbits.SSPADD = 0x57; //set a 90.9kHz bit rate

    //configure the I2C interface
    SSP1CON1bits.SSPM = 0b1000; //I2C master mode
    SSP1CON1bits.SSPEN = 1;
    SSP1CON2bits.RCEN = 0; //the receiver is idle, we are only sending data
    SSP1CON3bits.PCIE = 1;
    SSP1CON3bits.SCIE = 1;
}

void DisMSSP(void)
{
    //disable interrupts for perifs
    PIE1bits.SSP1IE = 0; //MSSP interrupt
    PIE2bits.BCL1IE = 0; //bus collision for MSSP

    //disable the I2C interface
    SSP1CON1bits.SSPEN = 0;
    SSP1CON3bits.PCIE = 0;
    SSP1CON3bits.SCIE = 0;
}

char SendI2C(char *data, char length)
{

    //Wait for the I2C bus to be free
    
    WaitForI2C();

    if (SSP1CON1bits.SSPEN == 0)
    {
        InitMSSP();
    }

    //now write to the global i2c buffer and i2c buffer length (check to make sure buffer is empty)
    if (i2c_data_length == 0)
    {
        //here we know that there is no maore data in the i2c_data_buffer
        i2c_data_length = length;
        i2c_data = data;
    } else {
        return 0x01;  //error code which indicates transaction possibly still in progress
    }
    //now we have loaded the I2C data, we initiate a start condition
    i2c_in_use = 1;
    SSP1CON2bits.SEN = 1;
    return 0x00;    //non error
}

void WaitForI2C(void)
{
    while (i2c_in_use != 0)
    {
        NOP();
    }

}

void SetupCapSense(void)
{
    char result = 0;
    char data_build[5];

    ack_count = 0;

    //First disable capsense scanning
    data_build[0] = 0x00;//address + write
    data_build[1] = 0xA0;//command reg
    data_build[2] = 0x0A;//disable capsense scanning for the duration of setup
    result = SendI2C(data_build, 3);
    WaitForI2C();
    _delay(160000);//5ms / (1/32e6) = 160000

    //Then, enter setup mode see page 35
    data_build[0] = 0x00;//address + write
    data_build[1] = 0xA0;//command reg
    data_build[2] = 0x08;//enter setup mode (0mS wait required)
    result = SendI2C(data_build, 3);

    data_build[0] = 0x00;//address + write
    data_build[1] = 0x1C;//OP_SEL_0;
    data_build[2] = 0b10000000;//see page 14-15 of cypress
    result = SendI2C(data_build, 3);

    data_build[0] = 0x00;//address + write
    data_build[1] = 0x21;//OP_SEL_1;
    data_build[2] = 0b10000010;//see schematic
    result = SendI2C(data_build, 3);

    data_build[0] = 0x00;//address + write
    data_build[1] = 0x1E;//LOGICAL_OPR_INPUT0
    data_build[2] = 0b00000001;//see schematic
    result = SendI2C(data_build, 3);

    data_build[0] = 0x00;//address + write
    data_build[1] = 0x23;//LOGICAL_OPR_INPUT0
    data_build[2] = 0b00000001;//see schematic
    result = SendI2C(data_build, 3);

    //Then, exit setup mode see page 35
    data_build[0] = 0x00;//address + write
    data_build[1] = 0xA0;//command reg
    data_build[2] = 0x07;//exit setup mode (0mS wait required)
    result = SendI2C(data_build, 3);

    //Finally, enable capsense scanning
    data_build[0] = 0x00;//address + write
    data_build[1] = 0xA0;//command reg
    data_build[2] = 0x09;//enable capsense scanning for the duration of setup
    result = SendI2C(data_build, 3);
    WaitForI2C();
    //required 10mS (page 11)
    _delay(320000);//10ms / (1/32e6) = 320000

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
    switch(light_state)
    {
        //page 185 for equations
        case 0: //0 is high
            CCPR1L = 0x36;
            CCP1CONbits.DC1B = 0b00;
            light_state = 1;
            //result is 0xD8
            break;

        case 1: //1 is medium
            CCPR1L = 0x1B;
            CCP1CONbits.DC1B = 0b00;
            light_state = 2;
            //result is 0x6C;
            break;

        case 2: //2 is low
            CCPR1L = 0x05;
            CCP1CONbits.DC1B = 0b10;
            light_state = 0;
            //result is 0x16;
            break;

        default:
            CCPR1L = 0x0C;
            CCP1CONbits.DC1B = 0b00;
            light_state = 0;
            //default state is on 90%
    }
}


