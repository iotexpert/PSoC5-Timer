#include "project.h"
#include <stdio.h>

static volatile uint32 Channel_1_Count;
static int intFlag = 0;

//#define ISR_RESET

CY_ISR(isr_1_Handler)
{
    uint8 InterruptCause = Channel_1_Timer_ReadStatusRegister();	//	Read and clear interrupt status
    
    if(InterruptCause & Channel_1_Timer_STATUS_CAPTURE)
    {
        intFlag = 1;
    }
    
	if(InterruptCause & Channel_1_Timer_STATUS_TC)			//	Did we overflow??
	{
    	intFlag = 2;
	}
    
    #ifdef ISR_RESET
    resetPin_Write(1);
    #endif
    Channel_1_Timer_Stop();
    Channel_1_Timer_WriteCounter(100);
    
    #ifdef ISR_RESET
    Channel_1_Timer_Start();
    resetPin_Write(0);
    #endif
}
int main(void)
{
    char Serial_Command;
    char StringToPrint[90];
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
                Channel_1_Timer_Stop();									    //	Halt timer
            break;
           
            case 'w':
                Channel_1_Timer_Stop();									    //	Halt timer
                Channel_1_Timer_WriteCounter(100);
                Channel_1_Timer_Start();									//	Halt timer
            break;
           
                

        }
        if(intFlag)  // If an event occurred print the value
        {
            sprintf(StringToPrint,"Int %d Counter= %ld\n",intFlag,Channel_1_Count);
            UART_1_PutString(StringToPrint);         
            #ifndef ISR_RESET
            resetPin_Write(1);
            Channel_1_Timer_Start();
            resetPin_Write(0); 
            #endif
            intFlag = 0;
        }
       
    }
}
