/**********************************************************************
 * @file ps2_drive.h
 * @author vince025@163.com
 * @version V1.0
 * @brief
 * @copyright Copyright (c) 2024 Inc.
 **********************************************************************/
#ifndef _PS2_H_
#define _PS2_H_

#include <stdint.h>

#define USE_SPI_HAL         0   // is too fast, MUST less than 250Kbps

//手柄接口初始化 输入 DI->PA0
// 输出 DO->PA1 CS->PA2 CLK->PA3

//These are our button constants
#define PSB_SELECT          0
#define PSB_L3              1
#define PSB_R3              2
#define PSB_START           3
#define PSB_PAD_UP          4
#define PSB_PAD_RIGHT       5
#define PSB_PAD_DOWN        6
#define PSB_PAD_LEFT        7
#define PSB_L2              8
#define PSB_R2              9
#define PSB_L1              10
#define PSB_R1              11
#define PSB_GREEN           12
#define PSB_RED             13
#define PSB_BLUE            14
#define PSB_PINK            15

#define PSB_TRIANGLE        13
#define PSB_CIRCLE          14
#define PSB_CROSS           15
#define PSB_SQUARE          16

//#define WHAMMY_BAR 8

//These are stick values
#define PSS_RX 5 //右摇杆X轴数据
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8

void PS2_Init(void);
void PS2_Vibration(uint8_t motor1, uint8_t motor2, uint8_t rx_data[9]); //振动设置motor1 0xFF开，其他关，motor2 0x40~0xFF
void PS2_SetInit(void); //配置初始化

void PS2_demo(void); //读取PS2手柄的控制量

#endif // _PS2_H_
