/*
 * mpu6050_uart3.h
 *
 *  Created on: 2018Äê10ÔÂ27ÈÕ
 *      Author: wmy
 */

#ifndef MPU6050_MPU6050_UART3_H_
#define MPU6050_MPU6050_UART3_H_

extern char UART3_Rx_Buffers[11];
extern float a_x;
extern float a_y;
extern float a_z;
extern float w_x;
extern float w_y;
extern float w_z;
extern float roll;
extern float pitch;
extern float yaw;
extern float T;
extern bool a_updated_flag;
extern bool w_updated_flag;
extern bool angle_updated_flag;

extern void Recive_UART3_Config(void);
extern void UART3IntHandler(void);
extern void Printf_MPU_Infos(void);



#endif /* MPU6050_MPU6050_UART3_H_ */
