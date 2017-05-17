
#include "project.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define Ch1Period	55555									//	Period for Channel_1_Timer

char Serial_Command;
char StringToPrint[90];
volatile uint32 Channel_1_Count;

CY_ISR(Channel_1_ISR_Handler)
{ 
uint8 InterruptCause;										//	Set by status register read
    Channel_1_Count = Channel_1_Timer_ReadPeriod();
    onboard_led_Write(~onboard_led_Read());					//	Indicate interrupt
	Channel_1_Timer_ClearFIFO();							//	Remove any additional captures
    InterruptCause = Channel_1_Timer_ReadStatusRegister();	//	Read and clear interrupt status
	if(InterruptCause & Channel_1_Timer_STATUS_TC)			//	Did we overflow??
	{
		Channel_1_Count = 0;								//	Error, no falling edge on signal
		return;												//	No further action required
	}
	Channel_1_Timer_Stop();									//	Halt timer
    Channel_1_Timer_WriteCounter(Ch1Period);				//	Rewrite counter for next cycle
	Channel_1_Timer_Start();								//	Restart Timer
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    UART_1_Start();
    Channel_1_Timer_Start();
	Channel_1_ISR_StartEx(Channel_1_ISR_Handler);
    for(;;)
    {
     Serial_Command = UART_1_GetChar();
        if(Serial_Command == 's'){
        sprintf(StringToPrint,"Channel1 capture is %ld\n\r",Channel_1_Count);
        UART_1_PutString(StringToPrint);
        }   
    }
}

/* [] END OF FILE */
