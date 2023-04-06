/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "stdio.h"

char data[60];

	
void tache1(void const* argument);
osThreadId ID_Tache1;
osThreadDef(tache1, osPriorityNormal, 1, 0);

void tache2(void const* argument);
osThreadId ID_Tache2;
osThreadDef(tache2, osPriorityHigh, 1, 0);


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
	ID_Tache2 = osThreadCreate(osThread(tache2), NULL);

  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
}


void tache1(void const* argument)
{
	char tab[1];
	int i=0;
	char etat = 0;
	
	while (1)
	{
		
		Driver_USART1.Receive(tab, 1);
		osSignalWait(0x02, osWaitForever);
		
		switch (etat)
		{
			case 0 :
				i=1; data[0] = 0; data[3] = 0; data[4] = 0; data[5] = 0; 
				data[0] = tab[0]; 
				if (tab[0] == '$') etat = 1;
				break;
			
			case 1 :
				data[i] = tab[0];
				i++;
				if ((data[3] == 'G') && (data[4] == 'G') && (data[5] == 'A')) 
				{
					etat = 2;
				}
				else if (i==6)
				{
					etat = 0;
				}
				break;
				
			case 2 :
				data[i] = tab[0];
				i++;
				if (i==60    ) 
				{
					osSignalSet(ID_Tache2, 0x1);
					etat = 0;
				}
				break;		
		}
	}
}	
		

void tache2(void const* argument)
{
	int i, j, k=0;
	char lat[9];
	char lon[9];
	
	while (1)
	{
		osSignalWait(0x1, osWaitForever);
		for (i=0; i<60; i++)
			{
				switch (data[i])
				{
					case 'N':
						for (j=0; j<9; j++)
						{
							lat[j] = data[i-10+j];	
						}	
						break;
					case 'E':
						for (j=0; j<9; j++)
						{
							lon[j] = data[i-11+j];
						}
						break;break;
				}
			}	
			
//		GLCD_DrawString(5,10,data);
		GLCD_DrawString(5,50+k,lat);
//		GLCD_DrawString(5,100,lon);
			//GLCD_DrawPixel(k, k);
			k++;
			
	}
	
}	
			



