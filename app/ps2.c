/**********************************************************************
 * @file ps2_drive.c
 * @author vince025@163.com
 * @version V1.0
 * @brief
 * @copyright Copyright (c) 2024 Inc.
 **********************************************************************/

#include "main.h"
#include "ps2.h"
#include "debug.h"

#if 1 // stm32 delay us by systick

#define SYSTICK_LOAD (SystemCoreClock/1000000U)
#define SYSTICK_DELAY_CALIB (SYSTICK_LOAD >> 1)

#define DELAY_US(us) \
    do { \
         uint32_t start = SysTick->VAL; \
         uint32_t ticks = (us * SYSTICK_LOAD)-SYSTICK_DELAY_CALIB;  \
         while((start - SysTick->VAL) < ticks); \
    } while (0)

#define DELAY_MS(ms) \
    do { \
        for (uint32_t i = 0; i < ms; ++i) { \
            DELAY_US(1000); \
        } \
    } while (0)

#endif // stm32 delay us by systick

#define DO_H                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET)
#define DO_L                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET)
#define CLK_H               HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)
#define CLK_L               HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)
#define DI_READ             (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) ? 1 : 0)

#define CS_H                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET) //CS拉高
#define CS_L                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET) //CS拉低


uint8_t Cmd[12] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //开始命令。请求数据

/**************************************************************************
 * 函数功能：PS2手柄初始化
 * 入口参数：无
 * 返回 值：无
 * **************************************************************************/
void PS2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void PS2_trans_data(uint8_t *tx_data, uint8_t *rx_data, uint16_t len)
{
    CS_L;
    DELAY_US(10);
    for(int i = 0; i < len; i++)
    {
        rx_data[i] = 0;
        for(int j = 0; j < 8; j++)
        {
            CLK_H;
            DELAY_US(5);
            if(tx_data[i] & (1 << j))
            {
                DO_H;
            }
            else
            {
                DO_L;
            }
            CLK_L;
            DELAY_US(5);
            if(DI_READ)
            {
                rx_data[i] |= (1 << j);
            }
            CLK_H;
        }
    }
    CS_H;
    DELAY_US(10);
}

/******************************************************
 * 函数功能: 手柄震动函数，
 * Calls: void PS2_Cmd(uint8_t CMD);
 * 入口参数: motor1:右侧小震动电机 0x00关，其他开
 *        motor2:左侧大震动电机 0x40~0xFF 电机开，值越大 震动越大
 * 返回 值:无
 * ******************************************************/
void PS2_Vibration(uint8_t motor1, uint8_t motor2, uint8_t *rx_data)
{
    uint8_t tx_data[12] = {0x01, 0x42, 0x00,
        0x00, // motor1: 右侧小震动电机 0x00关，其他开
        0x00, // motor2: 左侧大震动电机 0x40~0xFF 电机开，值越大 震动越大
        0x00, 0x00, 0x00, 0x00};
    tx_data[3] = motor1;
    tx_data[4] = motor2;

    PS2_trans_data(tx_data, rx_data, 9);
}

/**************************************************************************
 * 函数功能：short poll
 * 入口参数：无
 * 返回 值：无
 * **************************************************************************/
void PS2_ShortPoll(void)
{
    uint8_t rx_data[12] = {0};

    PS2_trans_data(Cmd, rx_data, 5);
}

/**************************************************************************
 * 函数功能：进入配置
 * 入口参数：无
 * 返回 值：无
 * **************************************************************************/
void PS2_EnterConfing(void)
{
    uint8_t tx_data[12] = {0x01, 0x43, 0x00,
        0x01, //
        0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t rx_data[12] = {0};

    PS2_trans_data(tx_data, rx_data, 9);
}

/**************************************************************************
 * 函数功能：发送模式设置
 * 入口参数：无
 * 返回 值：无
 * **************************************************************************/
void PS2_TurnOnAnalogMode(void)
{
    uint8_t tx_data[12] = {0x01, 0x44, 0x00,
        0x01,   // analog=0x01;digital=0x00 软件设置发送模式
        0x03,   // Ox03锁存设置，即不可通过按键“MODE”设置模式。
        //0xee,   // 0xEE不锁存软件设置，可通过按键“MODE”设置模式。
        0x00, 0x00, 0x00, 0x00};
    uint8_t rx_data[12] = {0};

    PS2_trans_data(tx_data, rx_data, 9);
}

/**************************************************************************
 * 函数功能：振动设置
 * 入口参数：无
 * 返回 值：无
 * **************************************************************************/
void PS2_VibrationMode(void)
{
    uint8_t tx_data[12] = {0x01, 0x4D, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
    uint8_t rx_data[12] = {0};

    PS2_trans_data(tx_data, rx_data, 5);
}

/**************************************************************************
 * 函数功能：完成并保存配置
 * 入口参数：无
 * 返回 值：无
 * **************************************************************************/
void PS2_ExitConfing(void)
{
    uint8_t tx_data[12] = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
    uint8_t rx_data[12] = {0};

    PS2_trans_data(tx_data, rx_data, 9);
}

/**************************************************************************
 * 函数功能：手柄配置初始化
 * 入口参数：无
 * 返回 值：无
 * **************************************************************************/
void PS2_SetInit(void)
{
    PS2_ShortPoll();
    PS2_ShortPoll();
    PS2_ShortPoll();
    PS2_EnterConfing();     //进入配置模式
    PS2_TurnOnAnalogMode(); //“红绿灯”配置模式，并选择是否保存
    PS2_VibrationMode();    //开启震动模式
    PS2_ExitConfing();      //完成并保存配置
}

/**************************************************************************
 * 函数功能：读取PS2手柄的控制量
 * 入口参数：无
 * 返回 值：无
 * **************************************************************************/
void PS2_demo(void)
{
    uint8_t rx_data[12];
    uint16_t key_value;
    int8_t PS2_LX = 0;
    int8_t PS2_LY = 0;
    int8_t PS2_RX = 0;
    int8_t PS2_RY = 0; //PS2相关变量

    static uint8_t motor_right_vibration = 0;
    static uint8_t motor_left_vibration = 0;

#if 0
    PS2_trans_data(Cmd, rx_data, 9);
#else
    PS2_Vibration(motor_right_vibration, motor_left_vibration, rx_data);
#endif

    if(rx_data[2] == 0x5A) // Data Frame
    {
        DEBUG_PRINTF("\r\n    RAW DATA = 0x%02X %02X %02X %02X %02X %02X %02X %02X %02X #",
                     rx_data[0],rx_data[1],rx_data[2],rx_data[3],rx_data[4],rx_data[5],rx_data[6],rx_data[7],rx_data[8]);

        DEBUG_PRINTF("    Mode = %s -- ", (rx_data[1] == 0x73) ? "Red" : "Green"); // 0x41 = green mode, 0x73 = red mode

        key_value = (rx_data[4] << 8) | rx_data[3]; //这是16个按键 按下为0， 未按下为1
        if(rx_data[1] == 0x73)
        {
            PS2_LX = rx_data[PSS_LX] - 128; //读取左边遥感X轴方向的模拟量
            PS2_LY = 127 - rx_data[PSS_LY]; //读取左边遥感Y轴方向的模拟量
            PS2_RX = rx_data[PSS_RX] - 128; //读取右边遥感X轴方向的模拟量
            PS2_RY = 127 - rx_data[PSS_RY]; //读取右边遥感Y轴方向的模拟量
        }
        else
        {
            PS2_SetInit();
            return;
        }

        DEBUG_PRINTF("    LX = %04d, LY = %04d, RX = %04d, RY = %04d -- ", PS2_LX, PS2_LY, PS2_RX, PS2_RY);
        for(int i = 0; i < 16; i++)
        {
            DEBUG_PRINTF("%d ", (key_value & (1 << i) ? 1 : 0));
        }
        DEBUG_PRINTF("\r\n\r\n");
        //DEBUG_PRINTF("    Key Value (0x%04X)\r\n", key_value, key_value);

        motor_left_vibration = 0;
        if((key_value & (1 << PSB_L2)) == 0) // 0: presed
        {
            motor_left_vibration = PS2_RY;
        }

        motor_right_vibration = 0x00;
        if((key_value & (1 << PSB_R2)) == 0) // 0: presed
        {
            motor_right_vibration = 0x01;
        }
    }
}
