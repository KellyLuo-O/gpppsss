/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "stdio.h"

typedef struct
{
	float latitude;
	float longitude;
}cordonnee;
	
	
void tache1(void const* argument);
osThreadId ID_Tache1;
osThreadDef(tache1, osPriorityNormal, 1, 0);

void tache2(void const* argument);
osThreadId ID_Tache2;
osThreadDef(tache2, osPriorityNormal, 1, 0);

osMailQId ID_BAL;
osMailQDef(BALData, 2, cordonnee);


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
	ID_BAL = osMailCreate(osMailQ(BALData), NULL);

  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
}


void tache1(void const* argument)
{
	char tab[1];
	int i=0;
	char etat = 0;
	char data[60];
	
	cordonnee *ptr;
	float lat, lon;
	char id[6], temps[10], n[1], e[1]; 
	
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
				if ((data[3] == 'G') && (data[4] == 'G') && (data[5] == 'A')) etat = 2 ;		
				break;
			case 2 :
				GLCD_DrawString(5,10,"case2");
				data[i] = tab[0];
				i++;
				if (i==40) 
				{
					GLCD_DrawString(5,10,"envoi");
					//sscanf(data, "%s,%s,%f,%c,%f,%c", id, temps, &lat, n, &lon, e);
					
					ptr = osMailAlloc(ID_BAL, osWaitForever);
					ptr -> latitude = lat;
					ptr -> longitude = lon;
					osMailPut(ID_BAL, ptr);
					
					etat = 0;
				}
				break;		
		}
	}
}	
		

void tache2(void const* argument)
{

	cordonnee *recep, valeur;
	osEvent EVretour;
	
	char texte[50];
	
	while (1)
	{
		EVretour = osMailGet(ID_BAL, osWaitForever);
		
		GLCD_DrawString(5,20,"recu");
		recep = EVretour.value.p;
		valeur = *recep;

		osMailFree(ID_BAL, recep);


		sprintf(texte, "lat : %f , lon : %f", valeur.latitude, valeur.longitude);
		GLCD_DrawString(5,50,texte);
			
//		GLCD_DrawString(5,50,lat);
//		GLCD_DrawString(5,100,lon);
	}
}	
			
