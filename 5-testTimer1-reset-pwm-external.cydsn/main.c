#include "project.h"
#include <stdio.h>

#define Ch1Period (9999)
static volatile uint32 Channel_1_Count;
static int intFlag = 0;


CY_ISR(isr_1_Handler)
{
    Channel_1_Count = Channel_1_Timer_ReadCounter();
    intFlag = Channel_1_Timer_ReadStatusRegister();  

    onboard_led_Write(~onboard_led_Read());
    CR_Write(1);
      
    if(intFlag & Channel_1_Timer_STATUS_TC)
        Channel_1_Count = 0;
    
}
int main(void)
{
    char Serial_Command;
    char StringToPrint[90];
    int printMode = 0;
    CyGlobalIntEnable; /* Enable global interrupts. */

    UART_1_Start();
    // Clear the screen with VT100 Escape Code + Start Message
    UART_1_PutString("\033[2J\033[HStarted\n");
 
    isr_1_StartEx(isr_1_Handler); 
    Channel_1_Timer_Start();
    
    PWM_1_Start();
    PWM_2_Start();
    
    for(;;)
    {
        // Command Loop
        Serial_Command = UART_1_GetChar();
        switch(Serial_Command)
        {   
            case 's': // Print value of last event
                sprintf(StringToPrint,"Channel1 capture is %ld\n",Ch1Period-Channel_1_Count);
                UART_1_PutString(StringToPrint);
            break;
           
            
                
            case 'k':
                PWM_1_WriteCompare(PWM_1_ReadCompare() - 100);
            break;
           
            case 'l':
                PWM_1_WriteCompare(PWM_1_ReadCompare() + 100);
            break;
           
            case 'a': // auto print mode
                printMode = 1;
            break;
            case 'A': // auto print mode
                printMode = 0;
            break;
                
                

        }
        if(intFlag)  // If an event occurred print the value
        {
            PWM_2_WriteCompare(Ch1Period - Channel_1_Count);
            if(printMode)
            {
                sprintf(StringToPrint,"Int %d Counter= %ld\n",intFlag,Ch1Period-Channel_1_Count);
                UART_1_PutString(StringToPrint);
            }
            intFlag = 0;
        }
       
    }
}
