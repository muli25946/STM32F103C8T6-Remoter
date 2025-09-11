/**
 * @file oled.h
 * @brief
 * 该设备挂载在IIC3上。该函数基于江协科技，在其基础上删除了汉字显示，并移植为HAL库。
 * @author Muli25946 (2687025869@qq.com)
 * @version 1.0
 * @date 2025-08-03
 *
 * @copyright Copyright (c) 2025  GPL
 *
 */
#ifndef OLED_H
#define OLED_H

#include "oled_font.h"
#include <stdint.h>

/*字符大小*/
#define OLED_8X16 8
#define OLED_6X8 6

#define OLED_ADDR 0x78 // OLED的I2C从机地址(默认写模式)
#define OLED_WRITE_MODE 0x00
#define OLED_WRITE_DATA 0x40

/*初始化函数*/
void OLED_Init(void);

/*更新函数*/
void OLED_Update(void);
void OLED_UpdateArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

/*显存控制函数*/
void OLED_Clear(void);
void OLED_ClearArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void OLED_Reverse(void);
void OLED_ReverseArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

/*显示函数*/
void OLED_ShowChar(int16_t X, int16_t Y, char Char, uint8_t FontSize);
void OLED_ShowString(int16_t X, int16_t Y, char *String, uint8_t FontSize);
void OLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length,
                  uint8_t FontSize);
void OLED_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length,
                        uint8_t FontSize);
void OLED_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length,
                     uint8_t FontSize);
void OLED_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length,
                     uint8_t FontSize);
void OLED_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength,
                       uint8_t FraLength, uint8_t FontSize);
// void OLED_ShowChinese(int16_t X, int16_t Y, char *Chinese);
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height,
                    const uint8_t *Image);
void OLED_Printf(int16_t X, int16_t Y, uint8_t FontSize, char *format, ...);

/*绘图函数*/
void OLED_DrawPoint(int16_t X, int16_t Y);
uint8_t OLED_GetPoint(int16_t X, int16_t Y);
void OLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1);
void OLED_DrawRectangle(int16_t X, int16_t Y, uint8_t Width, uint8_t Height,
                        uint8_t IsFilled);
void OLED_DrawTriangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1,
                       int16_t X2, int16_t Y2, uint8_t IsFilled);
void OLED_DrawCircle(int16_t X, int16_t Y, uint8_t Radius, uint8_t IsFilled);
void OLED_DrawEllipse(int16_t X, int16_t Y, uint8_t A, uint8_t B,
                      uint8_t IsFilled);
void OLED_DrawArc(int16_t X, int16_t Y, uint8_t Radius, int16_t StartAngle,
                  int16_t EndAngle, uint8_t IsFilled);

#endif // !OLED_H
