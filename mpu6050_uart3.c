/*
 * mpu6050_uart3.c
 *
 *  Created on: 2018Äê10ÔÂ27ÈÕ
 *      Author: wmy
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
#include "mpu6050_uart3.h"

char UART3_Rx_Buffers[11];
float a_x, a_y, a_z;
float w_x, w_y, w_z;
float roll, pitch, yaw;
float T;
bool a_updated_flag=false;
bool w_updated_flag=false;
bool angle_updated_flag=false;

void Recive_UART3_Config(void)
{
     SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
     GPIOPinConfigure(GPIO_PC6_U3RX);
     GPIOPinConfigure(GPIO_PC7_U3TX);
     GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6| GPIO_PIN_7);
     UARTConfigSetExpClk(UART3_BASE,SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
     IntEnable(INT_UART3);
     UARTIntEnable(UART3_BASE, UART_INT_RX | UART_INT_RT);
}

void UART3IntHandler(void)
{
    uint32_t ui32Status;
    uint8_t rx_buffer=0;
    static bool receive_flag=false;
    static int index=0;
    ui32Status = UARTIntStatus(UART3_BASE, true);
    UARTIntClear(UART3_BASE, ui32Status);
    while(UARTCharsAvail(UART3_BASE))
    {
        rx_buffer = (uint8_t)(UARTCharGetNonBlocking(UART3_BASE));
        if(rx_buffer==0x55)
        {
            receive_flag=true;
            index=0;
        }
        if(receive_flag==true)
        {
            UART3_Rx_Buffers[index]=rx_buffer;
            index++;
        }
        if(index>=11&&receive_flag==true)
        {
            receive_flag=false;
            index=0;
            switch(UART3_Rx_Buffers[1])
            {
            case 0x51:
                a_x = (short)(UART3_Rx_Buffers[3]<<8|UART3_Rx_Buffers[2])/32768.0*16;
                a_y = (short)(UART3_Rx_Buffers[5]<<8|UART3_Rx_Buffers[4])/32768.0*16;
                a_z = (short)(UART3_Rx_Buffers[7]<<8|UART3_Rx_Buffers[6])/32768.0*16;
                T = (short)(UART3_Rx_Buffers[9]<<8|UART3_Rx_Buffers[8])/340.0+36.25;
                a_updated_flag = true;
                break;
            case 0x52:
                w_x = (short)(UART3_Rx_Buffers[3]<<8|UART3_Rx_Buffers[2])/32768.0*2000;
                w_y = (short)(UART3_Rx_Buffers[5]<<8|UART3_Rx_Buffers[4])/32768.0*2000;
                w_z = (short)(UART3_Rx_Buffers[7]<<8|UART3_Rx_Buffers[6])/32768.0*2000;
                T = (short)(UART3_Rx_Buffers[9]<<8|UART3_Rx_Buffers[8])/340.0+36.25;
                w_updated_flag = true;
                break;
            case 0x53:
                roll = (short)(UART3_Rx_Buffers[3]<<8|UART3_Rx_Buffers[2])/32768.0*180;
                pitch = (short)(UART3_Rx_Buffers[5]<<8|UART3_Rx_Buffers[4])/32768.0*180;
                yaw = (short)(UART3_Rx_Buffers[7]<<8|UART3_Rx_Buffers[6])/32768.0*180;
                T = (short)(UART3_Rx_Buffers[9]<<8|UART3_Rx_Buffers[8])/340.0+36.25;
                angle_updated_flag = true;
                break;
            }
        }
    }
}


void Printf_MPU_Infos(void)
{
    if(a_updated_flag|w_updated_flag|angle_updated_flag)
    {
        a_updated_flag=false;
        w_updated_flag=false;
        angle_updated_flag=false;
//        GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,~GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3));
//        UARTprintf("ax = %d.%dg, ay = %d.%dg, az = %d.%dg\n", (int)a_x, abs((int)(1000*(a_x-(int)a_x))), \
                   (int)a_y, abs((int)(1000*(a_y-(int)a_y))), (int)a_z, abs((int)(1000*(a_z-(int)a_z))));
//        UARTprintf("wx = %d.%ddeg/s, wy = %d.%ddeg/s, wz = %d.%ddeg/s\n", (int)w_x, abs((int)(1000*(w_x-(int)w_x))), \
                   (int)w_y, abs((int)(1000*(w_y-(int)w_y))), (int)w_z, abs((int)(1000*(w_z-(int)w_z))));
//        UARTprintf("roll = %d.%ddeg, pitch = %d.%ddeg, yaw = %d.%ddeg\n", (int)roll, abs((int)(1000*(roll-(int)roll))), \
                   (int)pitch, abs((int)(1000*(pitch-(int)pitch))), (int)yaw, abs((int)(1000*(yaw-(int)yaw))));
//        UARTprintf("T = %d.%ddeg\n", (int)T, abs((int)(1000*(T-(int)T))));

//        UARTCharPutNonBlocking(UART1_BASE, 0xDA);
//        UARTCharPutNonBlocking(UART1_BASE, 0x01);
//        UARTCharPutNonBlocking(UART1_BASE, ((int)yaw)/255);
//        UARTCharPutNonBlocking(UART1_BASE, ((int)yaw)%255);
    }



}
