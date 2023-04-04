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
	int i=0;
	int j, k;
	char data[50];
	char etat = 0;
	char lat[9];
	char lon[9];
	
	while (1)
	{
		
		Driver_USART1.Receive(tab, 1);
		osSignalWait(0x02, osWaitForever);
		
		switch (etat)
		{
			case 0 :
				i=1;
				data[0] = tab[0];
				if (tab[0] == '$') etat = 1;
				break;
			case 1 :
				data[i] = tab[0];
				i++;
				if ((data[3] == 'G') && (data[4] == 'G') && (data[5] == 'A')) etat = 2;
				break;
			case 2 :
				data[i] = tab[0];
				i++;
			if (i==45) etat = 3;
					
//				for (i=0; i<40; i++)
//					{
//						if (data[i]== 'N')
//						{
//							for (j=0; j<9; j++)
//							{
//								lat[j] = data[i-9-j];
//							}
//						}
//						if (data[i]== 'E')
//						{
//							for (j=0; j<9; j++)
//							{
//								lon[j] = data[i-9-j];
//							}
//						}
//					}
					break;		
			case 3 :
				for (i=0; i<45; i++)
					{
						if (data[i] == 'N')
						{
							for (j=0; j<9; j++)
							{
								lat[j] = data[i-9-j];
							}	
						}
						else if (data[i] == 'E')
						{
							for (j=0; j<9; j++)
							{
								lon[j] = data[i-9-j];
							}
						}
					}
					GLCD_DrawString(5,10,data);
					etat = 0;
			
				break;	
		}
	}
}	
		

				
	

	

//		
//data[i] = tab [0];
//i++;
//		GLCD_DrawString(5,10,data);


