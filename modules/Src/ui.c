#include "ui.h"
#include "joystick.h"
#include "oled.h"

void UIRefreshStatic(UIData_t *ui, JoystickData_t *joy);

/**
 * @brief 初始化ui显示。设置摇杆数据默认解析形式和ui默认界面
 *
 * @param ui ui数据结构体地址
 * @param joy JoystickData_t地址
 */
void UIInit(UIData_t *ui, JoystickData_t *joy) {
  OLED_Init();
  joy->mode = RAW;
  ui->mode = RAWUI;
  ui->isUpdateStatic = 1;
}

/**
 * @brief ui显示任务
 *
 * @param ui UIData_t地址
 * @param joy JoystickData_t地址
 * @note
 * 更新显示。若需要切换显示模式则修改ui.mode同时ui.isUpdateStatic置1。数据需传入到ui.joyData/remoteData。
 */
void task_UIShow(UIData_t *ui, JoystickData_t *joy) {
  // 静态部分更新
  UIRefreshStatic(ui, joy);
  // 动态部分
  switch (ui->mode) {
  case RAWUI:
    for (int i = 1; i <= 4; i++) {
      OLED_ShowNum(24, 8 * i, (uint32_t)ui->joyData[i - 1], 4, OLED_6X8);
    }
    break;
  case REMOTEUI:
    OLED_ShowFloatNum(36, 8, ui->joyData[1], 4, 1, OLED_6X8);
    OLED_ShowFloatNum(36, 16, ui->joyData[0], 3, 1, OLED_6X8);
    OLED_ShowFloatNum(36, 24, ui->joyData[2], 2, 1, OLED_6X8);
    OLED_ShowFloatNum(36, 32, ui->joyData[3], 2, 1, OLED_6X8);
    break;
  }
  OLED_Update();
}

/**
 * @brief 静态ui部分更新
 *
 * @param ui UIData_t地址
 * @param joy JoystickData_t地址
 * @note
 * 通过检查ui->isUpdateStatic来判断是否更新，刷新完成后自动置0;同时修改遥感数据解析模式
 */
void UIRefreshStatic(UIData_t *ui, JoystickData_t *joy) {
  if (ui->isUpdateStatic) {
    switch (ui->mode) {
    case RAWUI:
      joy->mode = RAW;
      OLED_Clear();
      OLED_ShowString(64, 0, "RAW", OLED_6X8);
      OLED_ShowString(0, 8, "CH1:", OLED_6X8);
      OLED_ShowString(0, 16, "CH2:", OLED_6X8);
      OLED_ShowString(0, 24, "CH3:", OLED_6X8);
      OLED_ShowString(0, 32, "CH4:", OLED_6X8);
      break;
    case REMOTEUI:
      joy->mode = REMOTE;
      OLED_Clear();
      OLED_ShowString(64, 0, "REMOTE", OLED_6X8);
      OLED_ShowString(0, 8, "Gas:", OLED_6X8);
      OLED_ShowString(0, 16, "Yaw:", OLED_6X8);
      OLED_ShowString(0, 24, "Pitch:", OLED_6X8);
      OLED_ShowString(0, 32, "Roll:", OLED_6X8);
      break;
    }
    ui->isUpdateStatic = 0;
    OLED_Update();
  }
}
