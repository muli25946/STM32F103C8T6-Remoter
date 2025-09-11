/*官方库*/
#include "adc.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_dma.h"
#include "string.h"
#include <stdint.h>
/*自定义依赖*/
#include "joystick.h"
// #include "oled.h"

uint16_t
    adc_buffer[80]; // 4通道*10次采样+双缓冲adc采集数据数组，定义在joystick.c中
static JoystickData_t pJoy = {.pData = adc_buffer}; // 用于该文件中的数据传递

float MulsampleFilter(uint16_t *pData, JoystickFilterChannel ch);
void AdcConvertRaw(void);
void AdcConvertVolt(JoystickData_t *joy);
void AdcConvertRemoteData(JoystickData_t *joy);

// DMA半传输中断
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
  if (hadc->Instance == ADC1 && pJoy.isDataReady == 0) {
    pJoy.pData = adc_buffer;
    pJoy.isDataReady = 1;
  }
}

// DMA全传输中断
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  if (hadc->Instance == ADC1 && pJoy.isDataReady == 0) {
    pJoy.pData = &adc_buffer[40];
    pJoy.isDataReady = 1;
  }
}

/**
 * @brief 初始化遥感adc采集
 *
 * @param joy JoystickData_t数据类型地址
 * @note 该函数中开启了adc的dma同时初始化了双缓冲数组的首个访问指针
 */
void JoystickInit(void) {
  HAL_ADCEx_Calibration_Start(&hadc1);                   // ADC校准
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, 80); // 使用半传输中断
}

/**
 * @brief 获取摇杆滤波后的数据
 *
 * @param joy JoystickData_t数据类型地址。解析后的数据也在该变量中
 */
void JoystickGetData(JoystickData_t *joy) {
  // 获取可读位置指针
  joy->pData = pJoy.pData;

  /*数据滤波+装载数据*/
  joy->chData[0] = MulsampleFilter(joy->pData, CH1);
  joy->chData[1] = MulsampleFilter(joy->pData, CH2);
  joy->chData[2] = MulsampleFilter(joy->pData, CH3);
  joy->chData[3] = MulsampleFilter(joy->pData, CH4);

  /*解析形式*/
  switch (joy->mode) {
  case RAW: // 原始adc值
    AdcConvertRaw();
    break;
  case VOLTAGE: // 电压值0-3.3v
    AdcConvertVolt(joy);
    break;
  case REMOTE: // 转为占空比1000-2000
    AdcConvertRemoteData(joy);
    break;
  default:
    break;
  }
  pJoy.isDataReady = 0;

  // OLED_ShowFloatNum(0, 40, joy->chData[0], 4, 0, OLED_6X8);
  // OLED_Update();
}

/**
 * @brief 将滤波后的数据保留原格式
 *
 * @param joy JoystickData_t数据类型地址
 */
void AdcConvertRaw(void) { return; }

/**
 * @brief 将滤波后的数据转为电压值
 *
 * @param joy JoystickData_t数据类型地址
 */
void AdcConvertVolt(JoystickData_t *joy) {
  joy->chData[0] *= 0.0008;
  joy->chData[1] *= 0.0008;
  joy->chData[2] *= 0.0008;
  joy->chData[3] *= 0.0008;
}

/**
 * @brief 将采集到的数据转为用于PARAMETER_TUNING的数据
 *
 * @param joy JoystickData_t数据类型地址
 */
void AdcConvertRemoteData(JoystickData_t *joy) {
  joy->chData[0] = joy->chData[0] * 0.0879 - 180; // Yaw
  joy->chData[1] = joy->chData[1] * 0.244f + 1000;   // GAS
  joy->chData[2] = joy->chData[2] * 0.0146 - 30;   // Pitch
  joy->chData[3] = joy->chData[3] * 0.0146 - 30;   // Roll
}

/**
 * @brief 多重采样平均滤波
 *
 * @param pData 指向需要滤波的adc数组的起点
 * @param ch 指定需要滤波的通道
 * @return float 滤波结果
 */
float MulsampleFilter(uint16_t *pData, JoystickFilterChannel ch) {
  uint16_t sum = 0;
  for (uint8_t i = (uint8_t)ch; i < 40; i += 4) {
    sum += *(pData + i);
  }
  return (float)(sum / 10.0f);
}
