/**
 * @file oled_font.h
 * @brief 用于OLED屏幕的字库,基于江协科技
 * @author Muli25946 (2687025869@qq.com)
 * @version 1.0
 * @date 2025-08-03
 *
 * @copyright Copyright (c) 2025  GPL
 *
 */
#ifndef OLED_FONT_H
#define OLED_FONT_H

#include <stdint.h>

/*ASCII字模数据声明*/
extern const uint8_t OLED_F8x16[][16];
extern const uint8_t OLED_F6x8[][6];

/*图像数据声明*/
extern const uint8_t Diode[];
extern const uint8_t Face_eyes[];
extern const uint8_t Face_happy[];
extern const uint8_t Face_mania[];
extern const uint8_t Face_sleep[];
extern const uint8_t Face_stare[];
extern const uint8_t Face_very_happy[];
extern const uint8_t Face_hello[];
/*按照上面的格式，在这个位置加入新的图像数据声明*/

#endif // !OLED_FONT_H
