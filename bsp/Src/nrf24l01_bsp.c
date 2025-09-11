#include "nrf24l01_bsp.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_spi.h"
#include <stdint.h>

/**
 * @brief 向nrf24驱动提供spi交换数据的硬件接口
 *
 * @param TxData 要发送的数据
 * @return uint8_t 返回接受值
 */
uint8_t Nrf24ReadWriteByte(uint8_t TxData) {
  uint8_t rxData;
  HAL_SPI_TransmitReceive(&NRF_SPI, &TxData, &rxData, 1, 10);
  return rxData;
}

/**
 * @brief 提供给nrf24驱动的控制cs引脚电平的函数接口
 *
 * @param cs NRF24L01CSType类型
 */
void Nrf24ChipSelect(NRF24L01CSType cs) {
  HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin,
                    (cs == NRF24L01CS_Enable) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

/**
 * @brief 提供给nrf24驱动的控制ce引脚电平的函数接口
 *
 * @param en NRF24L01CEType枚举
 */
void Nrf24ChipEnable(NRF24L01CEType en) {
  HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin,
                    (en == NRF24L01CE_Enable) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * @brief 提供给nrf24驱动的获取IRQ电平的函数接口
 *
 * @return uint8_t 有中断返回0，无中断返回1
 */
uint8_t NRF24LGetIRQ(void) {
  GPIO_PinState pinState = HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port, NRF_IRQ_Pin);
  return (pinState == GPIO_PIN_RESET) ? 0 : 1;
}

/**
 * @brief 提供给nrf24驱动的延迟函数接口
 *
 * @param nTime 延迟时间，单位ms
 */
void NRF24LDelayms(volatile uint32_t nTime) { HAL_Delay(nTime); }
