/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "stdio.h"


void tache1(void const* argument);
osThreadId ID_Tache1;
osThreadDef(tache1, osPriorityNormal, 1, 0);

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

extern ARM_DRIVER_USART Driver_USART1;

void usart_cb(uint32_t event)
{
	switch (event)
		{
		case ARM_USART_EVENT_RECEIVE_COMPLETE :
			osSignalSet(ID_Tache1, 0x02); break;
		}
}

void Init_UART(void){
	Driver_USART1.Initialize(usart_cb);
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
	char tab[1];
	int i = 6;
	char data[6];
	char etat = 0;
	
	while (1)
	{
		
		Driver_USART1.Receive(tab, 1);
		osSignalWait(0x02, osWaitForever);
		
		switch (etat)
		{
			case 0 :
				if (tab[0] == '$'){ data[0] = tab[0]; etat = 1; }
				else { etat = 0;}
				break;
				
			case 1 :
				if (tab[0] == 'G'){ data[1] = tab[0]; etat = 2;}
				else { etat = 0;}
				break;
			case 2 :
				if (tab[0] == 'P'){ data[2] = tab[0]; etat = 3;}
				else { etat = 0;}
				break;
			case 3 :
				if (tab[0] == 'G') {data[3] = tab[0]; etat = 4;}
				else { etat = 0;}				
				break;
			case 4 :
				if (tab[0] == 'G'){ data[4] = tab[0]; etat = 5;}
				else { etat = 0;}				
				break;
			case 5 :
				if (tab[0] == 'A'){ data[5] = tab[0]; etat = 6;}
				else { etat = 0;}				
				break;
			case 6:
				data[i] = tab[0]; 
				i++;
				if (i == 50) etat = 0;
				break;
		}	
		
		GLCD_DrawString(5,5,data);	
		

//		for (j=0; j<200; j++)
//		{
//			if (tab[j] == '$') //&& (tab[j+3] == 'G') && (tab[j+4] == 'G')&& (tab[j+3] == 'A') )
//			{
//				data[0] = tab[j]; data[1] = tab[j+1]; data[2] = tab[j+2]; data[3] = tab[j+3]; data[4] = tab[j+4]; data[5] = tab[j+5];
//				GLCD_DrawString(5,400,data);
//				
//				break;
//			}
//		}
	}
}