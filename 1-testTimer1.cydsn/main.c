#include "project.h"
#include <stdio.h>

//	Period for Channel_1_Timer
#define Ch1Period	(9999)

static volatile uint32 Channel_1_Count;
static int intFlag = 0;

CY_ISR(Channel_1_ISR_Handler)
{ 
    intFlag = Channel_1_Timer_ReadStatusRegister();	//	Read and clear interrupt status
    onboard_led_Write(~onboard_led_Read());					        
   
    if(intFlag & Channel_1_Timer_STATUS_CAPTURE)
        Channel_1_Count = Ch1Period - Channel_1_Timer_ReadCapture();    
  
    
	if(intFlag & Channel_1_Timer_STATUS_TC)			//	Did we overflow??
        Channel_1_Count = 0;
	
    Channel_1_Timer_Stop();                         // stop and setup for next round
    Channel_1_Timer_WriteCounter(Ch1Period);		//	Rewrite counter for next cycle
}

int main(void)
{
    char Serial_Command;
    char StringToPrint[90];
    CyGlobalIntEnable;

    UART_1_Start();
    UART_1_PutString("\033[2J\033[HStarted\n"); // Clear the screen
    Channel_1_ISR_StartEx(Channel_1_ISR_Handler);
    Channel_1_Timer_Start();
    
    for(;;)
    {
        Serial_Command = UART_1_GetChar();
        switch(Serial_Command)
        {   
            case 's':
                sprintf(StringToPrint,"Channel1 capture is %ld\n",Ch1Period-Channel_1_Count);
                UART_1_PutString(StringToPrint);
            break;
            case 't':
                UART_1_PutString("Trigger 1.23 ms Edge\n");
                Channel_1_pin_Write(1);
                CyDelayUs(1230);
                Channel_1_pin_Write(0);
            break;
                
            case 'y':
                UART_1_PutString("Trigger 12ms Overflow \n");
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

            case 'w':
                Channel_1_Timer_Stop();									//	Halt timer          
                Channel_1_Timer_WriteCounter(Ch1Period);				//	Rewrite counter for next cycle
	            Channel_1_Timer_Start();								//	Restart Timer
            break;
        }
        
        if(intFlag) // There was an event... what happened?
        {
            sprintf(StringToPrint,"Channel1 %s Time= %ld\n",(intFlag & Channel_1_Timer_STATUS_CAPTURE)?"Capture":"Overflow", Channel_1_Count);
            UART_1_PutString(StringToPrint);
            intFlag = 0;
            Channel_1_Timer_Start();
        }
        
    }
}
