#ifndef NRF24L01_BSP_H
#define NRF24L01_BSP_H

/*依赖*/
#include "nrf24l01.h"
/*官方库*/
#include <stdint.h>
#include "spi.h"

#define NRF_SPI hspi1

uint8_t Nrf24ReadWriteByte(uint8_t TxData);  // 声明向nRF24L01读写一个字节的函数
void Nrf24ChipSelect(NRF24L01CSType cs);     // 声明片选操作函数
void Nrf24ChipEnable(NRF24L01CEType en);     // 声明使能及模式操作函数
uint8_t NRF24LGetIRQ(void);                  // 声明中断获取函数
void NRF24LDelayms(volatile uint32_t nTime); // 毫秒延时操作指针

#endif
