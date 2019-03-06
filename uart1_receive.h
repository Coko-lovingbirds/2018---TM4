/*
 * uart1_receive.h
 *
 *  Created on: 2018年10月20日
 *      Author: wmy
 */

#ifndef UART_UART1_RECEIVE_H_
#define UART_UART1_RECEIVE_H_

#define SEND_ID 0xD2  //小车发送

extern char UART1_Rx_Buffers[50];
extern char UART1_Rx_Len;
extern char UART1_Sender_Address;
extern char UART1_Rx_Data[50];
extern bool UART1_Updated_Flag;
extern void Uart1_Data_Pros(void);
extern void Recive_UART1_Config(void);
extern void UART1IntHandler(void);

#endif /* UART_UART1_RECEIVE_H_ */
