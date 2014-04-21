/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

void InitApp(void);         /* I/O and Peripheral Initialization */
void InitTimer(void);       //timer initialization
void DisTimer(void);        //timer disable
void EnableOutput(void);    //enable PWM output, also enables timer
void DisableOutput(void);   //turns off PWM output, does not touch timer
//void ToggleOnOff(void);     //toggles PWM output
void NextState(void);       //cycles to the next brightness state
void InitMSSP(void);        //initializes the MSSP
void DisMSSP(void);         //disables the MSSP
char SendI2C(char *data, char length);
void WaitForI2C(void);
void SetupCapSense(void);

