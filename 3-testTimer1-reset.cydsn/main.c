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
            case 't': 
                UART_1_PutString("Trigger 1.23 ms edge no overflow\n");
                Channel_1_pin_Write(1);
                CyDelayUs(1230);
                Channel_1_pin_Write(0);
            break;
                
            case 'y':
                UART_1_PutString("Trigger 12ms edge will Overflow \n");
                Channel_1_pin_Write(1);
                CyDelayUs(12000);
                Channel_1_pin_Write(0);
            break;
            
            int counter;
            case 'c':
                counter = Channel_1_Timer_ReadCounter();
                sprintf(StringToPrint,"Channel1 counter is %d\n",counter);
                UART_1_PutString(StringToPrint);
            break;
            
            case 'q':
                Channel_1_Timer_Stop();									//	Halt timer
            break;
                
            case 'r':
                onboard_led_Write(0);
            break;
                
            case 'f':
                sprintf(StringToPrint,"Int %d \n",intFlag);
                UART_1_PutString(StringToPrint);
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
            if(printMode)
            {
                sprintf(StringToPrint,"Int %d Counter= %ld\n",intFlag,Ch1Period-Channel_1_Count);
                UART_1_PutString(StringToPrint);
            }
            intFlag = 0;
        }
       
    }
}
