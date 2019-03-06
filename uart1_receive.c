/*
 * uart2_receive.c
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/interrupt.h"
#include "driverlib/fpu.h"
#include "driverlib/qei.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "time.h"
#include "inc/hw_i2c.h"
#include "driverlib/rom.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "string.h"
#include "driverlib/timer.h"
#include "string.h"
#include "uart1_receive.h"
#include "drv8825.h"

#define ADDRESS_ID 0xA1

int8_t str[7]={0xEA,SEND_ID,0xA1,0X02,0x88,0x81,0XEF};    //Ò£¸Ë¿ØÖÆ   ·ÉÐÐÆ÷Í£

char UART1_Rx_Buffers[50];
char UART1_Rx_Len;
char UART1_Sender_Address;
char UART1_Rx_Data[50];
bool UART1_Updated_Flag=false;

double str_angle;
double ag_angle;

void Recive_UART1_Config(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0| GPIO_PIN_1);
    UARTConfigSetExpClk(UART1_BASE,SysCtlClockGet(), 9600,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    IntEnable(INT_UART1);
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
}

//STX SID RID LEN DATA ETX
//eg EA AD A1 01 33 EF

void UART1IntHandler(void)
{
    uint32_t ui32Status;
    char rx_buffer;
    static bool receive_flag=false;
    static int index=0;
    ui32Status = UARTIntStatus(UART1_BASE, true);
    UARTIntClear(UART1_BASE, ui32Status);
    while(UARTCharsAvail(UART1_BASE))
    {
        rx_buffer = (uint8_t)(UARTCharGetNonBlocking(UART1_BASE));
        if(rx_buffer==0xEA)
        {
            receive_flag = true;
            index = 0;
            memset(UART1_Rx_Buffers,0,50);
        }
        if((index-1)>=2)
        {
            if(UART1_Rx_Buffers[2]!=ADDRESS_ID)
            {
                receive_flag = false;
                index = 0;
                memset(UART1_Rx_Buffers,0,50);
            }
        }
        if(receive_flag==true)
        {
            UART1_Rx_Buffers[index++] = rx_buffer;
            if(rx_buffer==0xEF)
            {
                receive_flag = false;
                if(index==5+UART1_Rx_Buffers[3])
                {
                    index = 0;
                    UART1_Sender_Address = UART1_Rx_Buffers[1];
                    UART1_Rx_Len = UART1_Rx_Buffers[3];
                    memset(UART1_Rx_Data,0,50);
                    memcpy(UART1_Rx_Data, UART1_Rx_Buffers+4, UART1_Rx_Len);
                    UART1_Updated_Flag = true;
                }
                else
                {
                    index = 0;
                    memset(UART1_Rx_Buffers,0,50);
                }
            }
        }
    }
}



void Uart1_Data_Pros(void)
{

	int i;
	uint32_t data = 0;
    if(UART1_Updated_Flag==true)
    {
//    	UARTprintf("%s",UART1_Rx_Data);
    	UART1_Updated_Flag=false;
    	switch(UART1_Rx_Len)
    	{
    	case 2:
    		switch(UART1_Rx_Data[0])
    		{
    		case 0x88:
//    		    GPIOPinWrite(GPIO_PORTD_BASE,  GPIO_PIN_0, GPIO_PIN_0);
//    		    GPIOPinWrite(GPIO_PORTD_BASE,  GPIO_PIN_1, GPIO_PIN_1);
    			PWMGenDisable(PWM1_BASE, PWM_GEN_1);
    			PWMGenDisable(PWM0_BASE, PWM_GEN_1);
    			for(i=0;i<7;i++)
    				UARTCharPutNonBlocking(UART1_BASE,str[i]);    //·ÉÐÐÆ÷½µÂä

    			break;
    		case 0x11:
    			switch(UART1_Rx_Data[1])
    			{
    			case 0x81:
    				DRV8825_Motor1_Control(350, '+');
    				DRV8825_Motor2_Control(350, '+');
//    				Reset_PWM();

    				break;
    			case 0x82:
    				DRV8825_Motor1_Control(500, '+');
    				DRV8825_Motor2_Control(500, '+');
//    				Reset_PWM();


    				break;
    			case 0x83:
    				DRV8825_Motor1_Control(750, '+');
    				DRV8825_Motor2_Control(750, '+');
 //   				Reset_PWM();

    				break;
    			}
    			break;
    		case 0x22:
    			switch(UART1_Rx_Data[1])
    			{
    			case 0x81:
    				DRV8825_Motor1_Control(350, '-');
    				DRV8825_Motor2_Control(350, '-');
 //   				Reset_PWM();

    			    break;
    			case 0x82:
    				DRV8825_Motor1_Control(500, '-');
    				DRV8825_Motor2_Control(500, '-');
 //   				Reset_PWM();

    			    break;
    			case 0x83:
    				DRV8825_Motor1_Control(750, '-');
    				DRV8825_Motor2_Control(750, '-');
 //   				Reset_PWM();


    			    break;
    			}
    			break;
    		case 0x33:
    			switch(UART1_Rx_Data[1])
    			{
    			case 0x81:
    				DRV8825_Motor1_Control(350, '+');
    				DRV8825_Motor2_Control(350, '-');
//    				Reset_PWM();

    			    break;
    			case 0x82:
    				DRV8825_Motor1_Control(500, '+');
    				DRV8825_Motor2_Control(500, '-');
//    				Reset_PWM();


    			    break;
    			case 0x83:
    				DRV8825_Motor1_Control(750, '+');
    				DRV8825_Motor2_Control(750, '-');
//    				Reset_PWM();

    			    break;
    			}
    			break;
    		case 0x44:
    			switch(UART1_Rx_Data[1])
    			{
    			case 0x81:
    				DRV8825_Motor1_Control(350, '-');
    				DRV8825_Motor2_Control(350, '+');
 //   				Reset_PWM();


    			    break;
    			case 0x82:
    				DRV8825_Motor1_Control(500, '-');
    				DRV8825_Motor2_Control(500, '+');
 //   				Reset_PWM();


    			    break;
    			case 0x83:
    				DRV8825_Motor1_Control(750, '-');
    				DRV8825_Motor2_Control(750, '+');
 //   				Reset_PWM();


    			    break;
    			}
    			break;
    		}
    		break;
    	case 5:
    		switch(UART1_Rx_Data[0])
    		{
    		case 0xAA:
				switch(UART1_Rx_Data[3])
				{
				case 0x2B:
					data = 255*UART1_Rx_Data[1]+UART1_Rx_Data[2];
					str_angle = 1.59*360*data/(3.14*3.24*3.24);
					switch(UART1_Rx_Data[4])
					{
					case 0x81:
						Angle_Run(str_angle,'+');

						break;
					case 0x82:
						Angle_Run(str_angle,'+');

						break;
					case 0x83:
						Angle_Run(str_angle,'+');

						break;

					}

					break;
				case 0x2D:
					data = 255*UART1_Rx_Data[1]+UART1_Rx_Data[2];
					str_angle = 1.65*360*data/(3.14*3.24*3.24);
					switch(UART1_Rx_Data[4])
					{
					case 0x81:
						Angle_Run(str_angle,'-');

						break;
					case 0x82:
						Angle_Run(str_angle,'-');

						break;
					case 0x83:
						Angle_Run(str_angle,'-');

						break;
					}
					break;
				}

    			break;

    		case 0xBB:
    			switch(UART1_Rx_Data[3])
				{
				case 0x2B:                      //RIGHT
					data = 255*UART1_Rx_Data[1]+UART1_Rx_Data[2];
					ag_angle = 1.55*((data*3.14)/180*16.5)*360/(3.14*3.24*3.24);
					switch(UART1_Rx_Data[4])
					{
					case 0x81:
						Rotation_Angle_Motor1(ag_angle,'+');

						break;
					case 0x82:
						Rotation_Angle_Motor1(ag_angle,'+');

						break;
					case 0x83:
						Rotation_Angle_Motor1(ag_angle,'+');

						break;

					}

					break;
				case 0x2D:				 	  //LEFT
					data = 255*UART1_Rx_Data[1]+UART1_Rx_Data[2];
					ag_angle = 1.55*((data*3.14)/180*16.5)*360/(3.14*3.24*3.24);
					switch(UART1_Rx_Data[4])
					{
					case 0x81:
						Rotation_Angle_Motor2(ag_angle,'+');

						break;
					case 0x82:
						Rotation_Angle_Motor2(ag_angle,'+');

						break;
					case 0x83:
						Rotation_Angle_Motor2(ag_angle,'+');

						break;
					}
					break;
				}
    			break;
    		}
    		break;
    	}
    }
}



