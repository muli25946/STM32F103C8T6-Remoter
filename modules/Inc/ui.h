/**
 * @file ui.h
 * @author Muli25946 (2687025869@qq.com)
 * @brief ui显示控制，同时修改摇杆数据解析形式以匹配ui
 * @version 0.1
 * @date 2025-09-06
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef UI_H
#define UI_H

/*官方库*/
#include <stdint.h>
/*依赖*/
#include "oled.h"
#include "joystick.h"

/*ui模式*/
typedef enum {
  RAWUI,
  REMOTEUI,
} UIMode;

typedef struct {
  uint8_t isUpdateStatic; // 静态ui更新标志
  float joyData[4];       // 摇杆数据
  UIMode mode;            // 显示模式
} UIData_t;

void UIInit(UIData_t *ui, JoystickData_t *joy);
void task_UIShow(UIData_t *ui, JoystickData_t *joy);

#endif
