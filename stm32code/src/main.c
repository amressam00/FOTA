/*
 * main.c
 *
 *  Created on: Oct 13, 2020
 *      Author: Ahmed
 */


#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "RCC_interface.h"
#include "DIO_interface.h"
#include "STK_interface.h"
#include "USART_interface.h"
#include "FPEC_interface.h"
#include "NVIC_INTERFACE.h"
void Parser_voidParseRecord(u8* Copy_u8BufData);
volatile u8  u8RecBuffer[200]   ;
volatile u8  u8RecCounter    = 0;
volatile u8  u8TimeOutFlag   = 0;
volatile u16 u16TimerCounter = 0;
volatile u8  u8BLWriteReq    = 1;
volatile u8  DoOnce=0;
typedef void (*Function_t)(void);
void App_delayMs(u32 Copy_u32Delay);
Function_t addr_to_call = 0;
void func(void)
{
#define SCB_VTOR   *((volatile u32*)0xE000ED08)

	SCB_VTOR = 0x08001000;

	addr_to_call = *(Function_t*)(0x08001004);
	addr_to_call();
}
void Clear()
{
	for(int z=0;z<200;z++)
		{
		u8RecBuffer[z]=0;
		}

	u8RecCounter=0;
}
u8 WIFICONNECT()
{
	for(int po=0;po<200;po++)
	{
		if(u8RecBuffer[po]=='W' && u8RecBuffer[po+1]=='I'&& u8RecBuffer[po+2]=='F' && u8RecBuffer[po+3]=='I')
		{
			/*
			 * This Means That ESP connected to WIFI
			 * */
			return 1;
		}
	}
	return 0;
}

u8 check_func()
{
	for(int i=0;i<100;i++)
	{

		if(u8RecBuffer[i]=='D'&&u8RecBuffer[i+1]=='N')
		{
		/*
			DN --> this means end of line
		*/
			return 1;
		}
		if(u8RecBuffer[i]=='A'&&u8RecBuffer[i+1]=='V')
		{
			/*
				DN --> this means Update available
			*/

			return 2;
		}
		if(u8RecBuffer[i]=='N'&&u8RecBuffer[i+1]=='O')
		{
			/*
			 * AV --> this means No Update available
			 *  */
			MNVIC_VidDisablePeripheral(USART1);
			func();
		}

	}


return 0;
}

void RecevingData()
{
	u8 DataRec=0;
	DataRec=MUSART1_u8ReadDataRegister();
	if(DataRec!='\r' && DataRec!='\n')
	{
		u8RecBuffer[u8RecCounter]=DataRec;
		u8RecCounter ++ ;
	}

}
u8 ReciveComplete()
{
	for(int i=0;i<100;i++)
	{
		if(u8RecBuffer[i]=='d'&&u8RecBuffer[i+1]=='o'&&u8RecBuffer[i+2]=='n'&&u8RecBuffer[i+3]=='e')
		{
			/*
			 * done  --> this means end of hex file
			 *  */
			MNVIC_VidDisablePeripheral(USART1);
			func();

		}

	}
	return 0;
}
void main(void)
{

	RCC_voidInitSysClock();
	RCC_voidEnableClock(RCC_APB2,14); /* USART1 */
	RCC_voidEnableClock(RCC_APB2,2);  /* PortA  */
	RCC_voidEnableClock(RCC_AHB,4);   /* FPEC   */


	MGPIO_VidSetPinDirection(GPIOA,9,0b1010);   /* TX AFPP */
	MGPIO_VidSetPinDirection(GPIOA,10,0b0100);  /* RC Input Floating */
	/* Enable NVIC For USART1 */
	MNVIC_VidSetPriority      ( USART1 , 1 , NO_SUB_PRIORITY , GROUP3 ) ;
	MNVIC_VidEnablePeripheral ( USART1                                ) ;
	MUSART1_voidInit();

	MSTK_voidInit();
	MUSART1_VidSetCallBack(&RecevingData);
u8 sendflag=0;
u8 EndLine=0,wc=0,complete=0;
	while(u8TimeOutFlag == 0)
	{

		while(wc!=1)
        		{
			       wc=WIFICONNECT();

		        }

		if(sendflag==0)
		{
			u8RecCounter=0;
			 Clear();
			MUSART1_voidTransmit((u8*)"UPDATE_TRIGGER");
			sendflag=1;
		}
		App_delayMs(10000);
		EndLine=check_func();
		if (EndLine == 2)
			{
				while(complete==0)
			    	{
						Clear();

						MUSART1_voidTransmit((u8*)"ok\n");
						if(DoOnce==0)
						{
							App_delayMs(10000);
							DoOnce=1;
						}
						App_delayMs(600);
					    EndLine=check_func();
					if(EndLine==1 )
						{
							if (u8BLWriteReq == 1)
							{
								FPEC_voidEraseAppArea();
								u8BLWriteReq = 0;
							}

							/* Parse */
							Parser_voidParseRecord((u8*)u8RecBuffer);
							complete= ReciveComplete();
						}
			        }
		}

}
}

void App_delayMs(u32 Copy_u32Delay)
{
	for(u32 Local_u32Counter = 0; Local_u32Counter < Copy_u32Delay*300; Local_u32Counter++)
	{
		asm("NOP");
	}
}
