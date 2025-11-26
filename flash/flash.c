#include <flash.h>

void Flash_cmd(uint8_t cmd, uint8_t CS)
{
  CS_num = CS;
  FLASH_CS_LOW(CS);
  HAL_SPI_Transmit_DMA(&hspi1, &cmd, 1);
}



void Flash_Transmit(uint8_t num_pin, uint32_t addr)
{
  CS_num = num_pin;
  Flash_SectorErase(num_pin, addr);
  Flash_WaitBusy();
  FLASH_CS_HIGH(num_pin);
  HAL_Delay(20);
  FLASH_CS_LOW(num_pin);
  Flash_cmd(CMD_WRITE_ENABLE, num_pin );  
  txbuf[0] = CMD_PAGE_PROGRAM;
  txbuf[1] = (addr >> 16) & 0xFF;
  txbuf[2] = (addr >> 8) & 0xFF;
  txbuf[3] = addr & 0xFF;
  
  memcpy(&txbuf[4], data_TX, 256);
  FLASH_CS_HIGH(CS_num);
  HAL_Delay(20);
  FLASH_CS_LOW(num_pin);
  HAL_SPI_Transmit_DMA(&hspi1, txbuf, 4 + 256);
}


void Flash_Receive(uint8_t num_pin, uint32_t addr)
{
  CS_num = num_pin;
  
  uint8_t tx_buffer[4] = {
    CMD_READ,           
    (addr >> 16) & 0xFF,
    (addr >> 8) & 0xFF, 
    addr & 0xFF
  };
  
  FLASH_CS_LOW(num_pin);
  
  HAL_SPI_Transmit_DMA(&hspi1, tx_buffer, 4);
  Flash_WaitBusy();
  FLASH_CS_HIGH(num_pin);
  HAL_Delay(20);
  FLASH_CS_LOW(num_pin);
  HAL_SPI_Receive_DMA(&hspi1, rxbuf, 256);
  
}


void Flash_SectorErase(uint8_t num_pin, uint32_t addr)
{
  Flash_cmd(CMD_WRITE_ENABLE, num_pin);
  
  uint8_t tx_buffer[4] = {
    CMD_SECTOR_ERASE,
    (addr >> 16) & 0xFF,
    (addr >> 8) & 0xFF,
    addr & 0xFF
  };
  
  FLASH_CS_LOW(num_pin);
  HAL_SPI_Transmit_DMA(&hspi1, tx_buffer, 4);
  
}


void Flash_WaitBusy(void)
{
    uint8_t cmd = 0x05;
    uint8_t status;

    do {
        FLASH_CS_LOW(0);        
        HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
        FLASH_CS_LOW(0);
        HAL_SPI_Receive(&hspi1, &status, 1, HAL_MAX_DELAY);
    } while (status & 0x01);
}

void test_1(){
  
  FLASH_CS_LOW(0);
  HAL_Delay(20);
  uint8_t a = 0x66;
  HAL_SPI_Transmit(&hspi1, &a, 1, HAL_MAX_DELAY);
  HAL_Delay(20);
  FLASH_CS_HIGH(0);
    
  HAL_Delay(20);
  
  FLASH_CS_LOW(0);
  HAL_Delay(20);
  uint8_t b = 0x99;
  HAL_SPI_Transmit(&hspi1, &b , 1, HAL_MAX_DELAY);
  HAL_Delay(20); 
  FLASH_CS_HIGH(0);
  
  HAL_Delay(20);
  
  FLASH_CS_LOW(0);
  HAL_Delay(20);
  uint8_t c = 0x9F;
  HAL_SPI_Transmit(&hspi1, &c, 1, HAL_MAX_DELAY);
  //HAL_Delay(50);
//  FLASH_CS_HIGH(0);
//  HAL_Delay(20);
  
//  HAL_Delay(20);
  
//  FLASH_CS_LOW(0);
// HAL_Delay(20);
  HAL_SPI_Receive(&hspi1, rxbuf, 3, HAL_MAX_DELAY);
  HAL_Delay(20);
  FLASH_CS_HIGH(0);
}