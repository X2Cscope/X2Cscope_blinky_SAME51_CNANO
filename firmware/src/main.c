/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

#include <math.h>

#define PI  M_PI

/**
 Section: Global Variables
 */

volatile bool btnState = false;
volatile bool led1Control = false;
volatile bool led2State = false;

volatile uint16_t sawTooth = 0; //sawtooth value
uint8_t speed = 1; // slope speed of the sawtooth
int16_t gain = 100; // amplitude of the generated sawtooth

typedef struct _MY_STRUCT_T
{
    float sinus;
    double rad;
}MY_STRUCT_t;

MY_STRUCT_t myStruct;

/**
 * Generate sawtooth and sine wave. Then do sample point for X2Cscope.
 */
void myTMR_1ms_callback(TC_TIMER_STATUS status, uintptr_t context)
{
    sawTooth += speed;          // Generate sawtooth
    if(sawTooth >= (360 * 2) )     // up-to multiple of 360
    {   
        sawTooth = 0;
        IO_LED2_Toggle();       //Toggle LED to Signal of sawtooth overflow (hearthbeat)
    }
    // Generate sinus from the sawtooth signal
    myStruct.rad = ((double) sawTooth) * PI / 360.0;
    myStruct.sinus = gain * sin(myStruct.rad);
    
    // This is the sample point of the scope
    // The period of execution interval of this function must be synced with the X2CScope settings

    X2CScope_Update();
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    TC0_TimerCallbackRegister(myTMR_1ms_callback, (uintptr_t)NULL);
    TC0_TimerStart();

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        // Add this line manually to the idle loop or low priority task
        X2CScope_Communicate();

        //Handle LED1
        if(led1Control)
        {
            IO_LED1_Set();
        }
        else
        {
            IO_LED1_Clear();
        }
        //Read SW1 state
        btnState = IO_SW1_Get();
        //Read back LED2 state
        led2State = IO_LED2_Get();
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

