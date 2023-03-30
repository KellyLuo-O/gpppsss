/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "stdio.h"

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

extern ARM_DRIVER_USART Driver_USART1;

void Init_UART(void){
	Driver_USART1.Initialize(NULL);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							9600);
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
}


void tache1(void const* argument);
osThreadId ID_Tache1;
osThreadDef(tache1, osPriorityNormal, 1, 0);

/*
 * main: initialize and start the system
 */
 
int main (void) {

  osKernelInitialize ();                    // initialize CMSIS-RTOS
	
  // initialize peripherals here
	
	Init_UART();
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_6x8);
	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	
	ID_Tache1 = osThreadCreate(osThread(tache1), NULL);

  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
}


void tache1(void const* argument)
{
	char tab[100];
	char texte[200];
	int i;
	i=0;
	
	while (1)
	{
		Driver_USART1.Receive(tab,100);
		while(Driver_USART1.GetRxCount() < 1);
		

		GLCD_DrawString(5,5+i*20,tab);
		i++; if(i==11) i=0;
		
		
		
		if(tab[3]=='R' & tab[4]=='M' & tab[5]=='C')
		{
			GLCD_DrawString(5,20,tab);
		}
		
		//osDelay(osWaitForever);
	}
	
}