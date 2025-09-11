/**
 * @file key.h
 * @author Muli25946 (2687025869@qq.com)
 * @brief 按键由中断驱动，此文件只提供在中断中的所以的抽象类型
 * @version 0.1
 * @date 2025-09-06
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef KEY_H
#define KEY_H

/*官方库*/
#include <stdint.h>

/*按键编号*/
typedef enum {
  DEFAULT_KEY_NUM,
  KEY1 = 1,
  KEY2 = 2,
  KEY3 = 3,
  KEY4 = 4,
} KeyNumEnum;

/*按键控制数据*/
typedef struct {
  KeyNumEnum keyData;
  uint8_t isDanger;
} KeyControlData_t;

#endif
