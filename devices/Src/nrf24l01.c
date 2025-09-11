#include "nrf24l01.h"
#include "stddef.h"
#include <stdint.h>


const uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10,
                                          0x01}; // 发送地址
const uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10,
                                          0x01}; // 发送地址

/*写寄存器*/
static uint8_t NRF24L01WriteRegister(NRF24L01ObjectType *nrf, uint8_t reg,
                                     uint8_t value);
/*读取寄存器值*/
static uint8_t NRF24L01ReadRegigster(NRF24L01ObjectType *nrf, uint8_t reg);
/*在指定位置读出指定长度的数据*/
static uint8_t NRF24L01ReadBuffer(NRF24L01ObjectType *nrf, uint8_t reg,
                                  uint8_t *pBuf, uint8_t len);
/*在指定位置写指定长度的数据*/
static uint8_t NRF24L01WriteBuffer(NRF24L01ObjectType *nrf, uint8_t reg,
                                   uint8_t *pBuf, uint8_t len);
/*检测24L01是否存在,返回值:0，成功;1，失败*/
static uint8_t NRF24L01Check(NRF24L01ObjectType *nrf);
/*设置nRF24L01的模式*/
static void SetNRF24L01Mode(NRF24L01ObjectType *nrf, NRF24L01ModeType mode);
/*缺省片选处理函数*/
static void NRF24L01CSDefault(NRF24L01CSType cs);

/**
 * @brief 启动NRF24L01发送一次数据包
 *
 * @param nrf NRF24L01ObjectType对象
 * @param txbuf 待发送数据首地址
 * @return uint8_t 发送完成状况
 */
uint8_t NRF24L01TransmitPacket(NRF24L01ObjectType *nrf, uint8_t *txbuf) {
  uint8_t status;

  SetNRF24L01Mode(nrf, NRF24L01TxMode);

  nrf->ChipEnable(NRF24L01CE_Disable);
  NRF24L01WriteBuffer(nrf, WR_TX_PLOAD, txbuf,
                      TX_PLOAD_WIDTH); // 写数据到TX BUF 32个字节
  nrf->ChipEnable(NRF24L01CE_Enable);  // 启动发送

  while (nrf->GetIRQ() != 0)
    ;                                          // 等待发送完成
  status = NRF24L01ReadRegigster(nrf, STATUS); // 读取状态寄存器的值
  NRF24L01WriteRegister(nrf, WRITE_REG_NRF + STATUS,
                        status); // 清除TX_DS或MAX_RT中断标志
  if (status & MAX_TX)           // 达到最大重发次数
  {
    NRF24L01WriteRegister(nrf, FLUSH_TX, 0xFF); // 清除TX FIFO寄存器
    return MAX_TX;
  }
  if (status & TX_OK) // 发送完成
  {
    return TX_OK;
  }
  return 0xFF; // 其他原因发送失败
}

/**
 * @brief 启动NRF24L01接收一次数据包
 * 
 * @param nrf NRF24L01ObjectType对象
 * @param rxbuf 待发送数据首地址
 * @return uint8_t 0，接收完成；其他，错误代码
 */
uint8_t NRF24L01ReceivePacket(NRF24L01ObjectType *nrf, uint8_t *rxbuf) {
  uint8_t status;

  SetNRF24L01Mode(nrf, NRF24L01RxMode);

  status = NRF24L01ReadRegigster(nrf, STATUS); // 读取状态寄存器的值
  NRF24L01WriteRegister(nrf, WRITE_REG_NRF + STATUS,
                        status); // 清除TX_DS或MAX_RT中断标志
  if (status & RX_OK)            // 接收到数据
  {
    NRF24L01ReadBuffer(nrf, RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH); // 读取数据
    NRF24L01WriteRegister(nrf, FLUSH_RX, 0xFF); // 清除RX FIFO寄存器
    return 0;
  }
  return 1; // 没收到任何数据
}

/*nRF24L01对象初始化函数*/
NRF24L01ErrorType
NRF24L01Initialization(NRF24L01ObjectType *nrf,            // nRF24L01对象
                       NRF24L01ReadWriteByte spiReadWrite, // SPI读写函数指针
                       NRF24L01ChipSelect cs,  // 片选信号操作函数指针
                       NRF24L01ChipEnable ce,  // 使能信号操作函数指针
                       NRF24L01GetIRQ irq,     // 中断信号获取函数指针
                       NRF24L01Delayms delayms // 毫秒延时
) {
  int retry = 0;

  if ((nrf == NULL) || (spiReadWrite == NULL) || (ce == NULL) ||
      (irq == NULL) || (delayms == NULL)) {
    return NRF24L01_InitError;
  }
  nrf->ReadWriteByte = spiReadWrite;
  nrf->ChipEnable = ce;
  nrf->GetIRQ = irq;
  nrf->Delayms = delayms;

  if (cs != NULL) {
    nrf->ChipSelect = cs;
  } else {
    nrf->ChipSelect = NRF24L01CSDefault;
  }

  while (NRF24L01Check(nrf) && (retry < 5)) {
    nrf->Delayms(300);
    retry++;
  }

  if (retry >= 5) {
    return NRF24L01_Absent;
  }

  for (int i = 0; i < 8; i++) {
    nrf->reg[i] = 0;
  }

  SetNRF24L01Mode(nrf, NRF24L01RxMode);

  return NRF24L01_NoError;
}

/*设置nRF24L01的模式*/
static void SetNRF24L01Mode(NRF24L01ObjectType *nrf, NRF24L01ModeType mode) {
  nrf->ChipEnable(NRF24L01CE_Disable);

  if (mode == NRF24L01RxMode) {
    /*初始化NRF24L01到RX模式。设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA
      HCURR； 当CE变高后,即进入RX模式,并可以接收数据了*/
    NRF24L01WriteBuffer(nrf, WRITE_REG_NRF + RX_ADDR_P0, (uint8_t *)RX_ADDRESS,
                        RX_ADR_WIDTH); // 写RX节点地址

    NRF24L01WriteRegister(nrf, WRITE_REG_NRF + EN_AA,
                          0x01); // 使能通道0的自动应答
    NRF24L01WriteRegister(nrf, WRITE_REG_NRF + EN_RXADDR,
                          0x01); // 使能通道0的接收地址
    NRF24L01WriteRegister(nrf, WRITE_REG_NRF + RF_CH, 40); // 设置RF通信频率
    NRF24L01WriteRegister(nrf, WRITE_REG_NRF + RX_PW_P0,
                          RX_PLOAD_WIDTH); // 选择通道0的有效数据宽度
    NRF24L01WriteRegister(nrf, WRITE_REG_NRF + RF_SETUP,
                          0x0F); // 设置TX发射参数,0db增益,2Mbps,低噪声增益开启
    NRF24L01WriteRegister(
        nrf, WRITE_REG_NRF + CONFIG,
        0x0F); // 配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式
  } else {
    /*初始化NRF24L01到TX模式。设置TX地址,写TX数据宽度,设置RX自动应答的地址,
      填充TX发送数据,选择RF频道,波特率和LNA HCURR；PWR_UP,CRC使能；当CE变高后,
      即进入RX模式,并可以接收数据了；CE为高大于10us,则启动发送*/
    NRF24L01WriteBuffer(nrf, WRITE_REG_NRF + TX_ADDR, (uint8_t *)TX_ADDRESS,
                        TX_ADR_WIDTH); // 写TX节点地址
    NRF24L01WriteBuffer(nrf, WRITE_REG_NRF + RX_ADDR_P0, (uint8_t *)RX_ADDRESS,
                        RX_ADR_WIDTH); // 设置TX节点地址,主要为了使能ACK

    NRF24L01WriteRegister(nrf, WRITE_REG_NRF + EN_AA,
                          0x01); // 使能通道0的自动应答
    NRF24L01WriteRegister(nrf, WRITE_REG_NRF + EN_RXADDR,
                          0x01); // 使能通道0的接收地址
    NRF24L01WriteRegister(
        nrf, WRITE_REG_NRF + SETUP_RETR,
        0x1A); // 设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
    NRF24L01WriteRegister(nrf, WRITE_REG_NRF + RF_CH, 40); // 设置RF通道为40
    NRF24L01WriteRegister(nrf, WRITE_REG_NRF + RF_SETUP,
                          0x0F); // 设置TX发射参数,0db增益,2Mbps,低噪声增益开启
    NRF24L01WriteRegister(
        nrf, WRITE_REG_NRF + CONFIG,
        0x0E); // 配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
  }

  nrf->reg[CONFIG] = NRF24L01ReadRegigster(nrf, CONFIG);
  nrf->reg[EN_AA] = NRF24L01ReadRegigster(nrf, EN_AA);
  nrf->reg[EN_RXADDR] = NRF24L01ReadRegigster(nrf, EN_RXADDR);
  nrf->reg[SETUP_AW] = NRF24L01ReadRegigster(nrf, SETUP_AW);
  nrf->reg[SETUP_RETR] = NRF24L01ReadRegigster(nrf, SETUP_RETR);
  nrf->reg[RF_CH] = NRF24L01ReadRegigster(nrf, RF_CH);
  nrf->reg[RF_SETUP] = NRF24L01ReadRegigster(nrf, RF_SETUP);
  nrf->reg[STATUS] = NRF24L01ReadRegigster(nrf, STATUS);

  nrf->ChipEnable(
      NRF24L01CE_Enable); // CE为高。设置RX时，进入接收模式；设置为TX时,10us后启动发送
}

/*检测24L01是否存在,返回值:0，成功;1，失败*/
static uint8_t NRF24L01Check(NRF24L01ObjectType *nrf) {
  uint8_t writeBuf[5] = {0XA5, 0XA5, 0XA5, 0XA5, 0XA5};
  uint8_t readBuf[5] = {0XAA, 0XAA, 0XAA, 0XAA, 0XAA};
  uint8_t status = 0;

  NRF24L01WriteBuffer(nrf, WRITE_REG_NRF + TX_ADDR, writeBuf,
                      5);                       /*写入5个字节的地址*/
  NRF24L01ReadBuffer(nrf, TX_ADDR, readBuf, 5); /*读出写入的地址*/

  for (int i = 0; i < 5; i++) {
    if (readBuf[i] != 0XA5) {
      status = 1; // 检测nRF24L01错误
      break;
    }
  }

  return status;
}

/*写寄存器*/
/*参数：reg:指定寄存器地址*/
/*      value:写入的值*/
/*返回值：状态值*/
static uint8_t NRF24L01WriteRegister(NRF24L01ObjectType *nrf, uint8_t reg,
                                     uint8_t value) {
  uint8_t status;

  nrf->ChipSelect(NRF24L01CS_Enable);  // 使能SPI传输
  status = nrf->ReadWriteByte(reg);    // 发送寄存器号
  nrf->ReadWriteByte(value);           // 写入寄存器的值
  nrf->ChipSelect(NRF24L01CS_Disable); // 禁止SPI传输

  return (status); // 返回状态值
}

/*读取寄存器值*/
/*参数：reg:要读的寄存器*/
/*返回值：读取的寄存器值*/
static uint8_t NRF24L01ReadRegigster(NRF24L01ObjectType *nrf, uint8_t reg) {
  uint8_t reg_val;

  nrf->ChipSelect(NRF24L01CS_Enable); // 使能SPI传输

  nrf->ReadWriteByte(reg);             // 发送寄存器号
  reg_val = nrf->ReadWriteByte(0XFF);  // 读取寄存器内容
  nrf->ChipSelect(NRF24L01CS_Disable); // 禁止SPI传输

  return (reg_val); // 返回状态值
}

/*在指定位置读出指定长度的数据*/
/*参数：reg:寄存器(位置)*/
/*      *pBuf:数据指针*/
/*      len:数据长度*/
/*返回值,此次读到的状态寄存器值*/
static uint8_t NRF24L01ReadBuffer(NRF24L01ObjectType *nrf, uint8_t reg,
                                  uint8_t *pBuf, uint8_t len) {
  uint8_t status;

  nrf->ChipSelect(NRF24L01CS_Enable); // 使能SPI传输

  status = nrf->ReadWriteByte(reg); // 发送寄存器值(位置),并读取状态值

  for (int i = 0; i < len; i++) {
    pBuf[i] = nrf->ReadWriteByte(0XFF); // 读出数据
  }

  nrf->ChipSelect(NRF24L01CS_Disable); // 关闭SPI传输

  return status; // 返回读到的状态值
}

/*在指定位置写指定长度的数据*/
/*参数：reg:寄存器(位置)*/
/*      *pBuf:数据指针*/
/*      len:数据长度*/
/*返回值,此次读到的状态寄存器值*/
static uint8_t NRF24L01WriteBuffer(NRF24L01ObjectType *nrf, uint8_t reg,
                                   uint8_t *pBuf, uint8_t len) {
  uint8_t status;

  nrf->ChipSelect(NRF24L01CS_Enable); // 使能SPI传输

  status = nrf->ReadWriteByte(reg); // 发送寄存器值(位置),并读取状态值

  for (int i = 0; i < len; i++) {
    nrf->ReadWriteByte(pBuf[i]); // 写入数据
  }

  nrf->ChipSelect(NRF24L01CS_Disable); // 关闭SPI传输

  return status; // 返回读到的状态值
}

/*缺省片选处理函数*/
static void NRF24L01CSDefault(NRF24L01CSType cs) {
  // 用于在SPI通讯时，片选信号硬件电路选中的情况
  return;
}
