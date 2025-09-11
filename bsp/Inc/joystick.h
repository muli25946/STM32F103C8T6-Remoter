/**
 * @file joystick.h
 * @author Muli25946 (2687025869@qq.com)
 * @brief 摇杆采集数据，拥有自己的数据类型JoystickData_t
 * @version 0.1
 * @date 2025-09-06
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef JOYSTOCK_H
#define JOYSTOCK_H

#include <stdint.h>

/*读取数据解码模式*/
typedef enum {
  RAW,              // 原始值模式
  VOLTAGE,          // 电压值模式
  REMOTE,           // 遥控模式
  PARAMETER_TUNING, // 参数整定模式
} JoystickDecodeMode;

/*多重采样滤波的通道选择*/
typedef enum {
  CH1 = 0,
  CH2 = 1,
  CH3 = 2,
  CH4 = 3,
} JoystickFilterChannel;

/*摇杆数据缓冲*/
typedef struct {
  // 左摇杆竖直方向的值 -->ADC1_CH1
  // 左摇杆水平方向的值 -->ADC1_CH2
  // -->ADC1_CH3
  // -->ADC1_CH4
  float chData[4];         // 四个通道的数据
  void *pData;             // 双缓冲指针
  uint8_t isDataReady;     // 数据就绪标志位
  JoystickDecodeMode mode; // 数据解码格式
} JoystickData_t;

void JoystickInit(void);
void JoystickGetData(JoystickData_t *joy);

#endif
