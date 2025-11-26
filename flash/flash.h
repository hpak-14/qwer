#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_spi.h"
#include "stm32f2xx_hal_gpio.h"
#include "stm32f2xx_hal_dma.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

extern SPI_HandleTypeDef hspi1;
void Flash_WaitBusy(void);
void Flash_cmd(uint8_t cmd, uint8_t CS);
void Flash_Transmit(uint8_t num_pin, uint32_t addr);
void Flash_Receive(uint8_t num_pin, uint32_t addr);
void Flash_SectorErase(uint8_t num_pin, uint32_t addr);
bool Flash_WaitForReady(uint8_t num_pin, uint32_t timeout);


uint8_t CS_num = 0;
uint8_t DMA_TX_Finish = 0;
uint8_t DMA_RX_Finish = 0;
uint32_t addr = 0x000000;     // адресс
uint8_t txbuf [2 + 3 + 256] = {0};  // 1 байт комада + 3 байта адреса + 256 байт данных
uint8_t rxbuf [256] = {0};      // 256 байт данных
uint8_t data_TX [256] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
    71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
    81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
    91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
    101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
    111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
    121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
    131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
    141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
    151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
    161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
    171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
    181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
    191, 192, 193, 194, 195, 196, 197, 198, 199, 200,
    201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
    211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
    221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
    231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
    241, 242, 243, 244, 245, 246, 247, 248, 249, 250,
    251, 252, 253, 254, 255
};         

uint8_t data_RX [256] = {0};
// Конф. CS                         0           1           2           3           4           5           6           7
GPIO_TypeDef* FLASH_CS_PORT[8] = {  GPIOD,      GPIOD,      GPIOD,      GPIOD,      GPIOD,      GPIOB,      GPIOD,      GPIOD       };
uint16_t FLASH_CS_PIN[8]   =     {  GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_3, GPIO_PIN_7, GPIO_PIN_1, GPIO_PIN_0  };

// Установить CS низким
void FLASH_CS_LOW(uint8_t num) {
    HAL_GPIO_WritePin(FLASH_CS_PORT[num], FLASH_CS_PIN[num], GPIO_PIN_RESET);
}
// Установить CS высоким
void FLASH_CS_HIGH(uint8_t num) {
    HAL_GPIO_WritePin(FLASH_CS_PORT[num], FLASH_CS_PIN[num], GPIO_PIN_SET);
}

/* Команды SST26 */
uint8_t cmd = 0;

#define CMD_READ            0x03
#define CMD_FAST_READ       0x0B
#define CMD_PAGE_PROGRAM    0x02
#define CMD_SECTOR_ERASE    0x20 /* 4KB */
#define CMD_CHIP_ERASE      0xC7
#define CMD_WRITE_ENABLE    0x06
#define CMD_WRITE_DISABLE   0x04
#define CMD_READ_STATUS     0x05
#define CMD_READ_ID         0x9F
#define Block_Protection    0x98

#define WRITE_TIMEOUT       1000    // 1 секунда 
#define ERASE_TIMEOUT       5000    // 5 секунд 
#define CHIP_ERASE_TIMEOUT  60000   // 60 секунд 
