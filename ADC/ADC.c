#include "ADC.h"
#include "math.h"

  uint8_t ADC_CMD = 0;
  uint8_t ADC_tx_data[3];
  uint8_t ADC_rx_data[19];
  float ADC_ref[8] = {0};
  int16_t channel_codes[8] = {0};
  float channel_voltages[8] ={0};
  int32_t channel_millivolts[8];
  
  
void ADS131E0_ReadID(){
  ADC_tx_data[0] = 0b00100000; 
  ADC_tx_data[1] = 0b00000000; 

 
  ADC_CS_LOW
  delay(30);
  HAL_SPI_Transmit(&hspi1, &ADC_tx_data[0], 2, HAL_MAX_DELAY);
  delay(20);
  HAL_SPI_Receive(&hspi1, &ADC_rx_data[0], 1, HAL_MAX_DELAY);
  delay(30);
  ADC_CS_HIGH
  delay(30);
  
}


void ADS131E0_RESET(){
//  ADC_PWDN_OFF 
//  ADC_RESET_OFF
 // ADC_CS_LOW
 
  ADC_PWDN_OFF
  HAL_Delay(100);
  
  ADC_CS_HIGH
  ADC_RESET_ON
  HAL_Delay(100);
  
  ADC_RESET_OFF
  HAL_Delay(1000);
  
  ADC_CS_LOW
  delay(25);
  HAL_SPI_Receive(&hspi1, &ADC_rx_data[0], 19, HAL_MAX_DELAY);
  delay(25);
  ADC_CS_HIGH
  delay(25);
  ADC_tx_data[0] = 0b00010001;
    
  ADC_CS_LOW
  delay(25);
  HAL_SPI_Transmit(&hspi1, &ADC_tx_data[0], 1, HAL_MAX_DELAY);
  delay(25);
  ADC_CS_HIGH
  delay(25);

  
  
}

void ADS131E0_RESET2(){
  ADC_tx_data[0] = 0b00010001;
  
  ADC_CS_LOW
  delay(25);
  HAL_SPI_Transmit(&hspi1, &ADC_tx_data[0], 1, HAL_MAX_DELAY);
  delay(250);
  ADC_CS_HIGH
  delay(250);
}


void ADS131E0_DataRead(){
  delay(50);
  ADC_CS_LOW
  delay(50);
  HAL_SPI_Receive_DMA(&hspi1, &ADC_rx_data[0], 19);
}


void ADS131E0_Conf(){
  ADC_tx_data[0] = 0;
  ADC_tx_data[1] = 0;
  ADC_tx_data[2] = 0;
  
  ADC_tx_data[0] = 0b00010001;
    
  ADC_CS_LOW
  delay(100);
  HAL_SPI_Transmit(&hspi1, &ADC_tx_data[0], 1, HAL_MAX_DELAY);
  delay(100);
  ADC_CS_HIGH
    
    ADC_tx_data[0] = 0b01000001; // WREG + адрес 01h
    ADC_tx_data[1] = 0b00000000; // записать 1 регистр
    ADC_tx_data[2] = 0b10110001; // новое значение CONFIG1
  
  ADC_CS_LOW
  delay(100);
  HAL_SPI_Transmit(&hspi1, &ADC_tx_data[0], 3, HAL_MAX_DELAY);
  delay(100);
  ADC_CS_HIGH
    
  ADC_tx_data[0] = 0b00010000;
    
  ADC_CS_LOW
  delay(100);
  HAL_SPI_Transmit(&hspi1, &ADC_tx_data[0], 1, HAL_MAX_DELAY);
  delay(100);
  ADC_CS_HIGH
    
  
}

void delay(uint32_t tik){
  /*
  uint16_t toc = 0;
  while (toc <= tik){
  toc++;
  }
*/
  for (int k = 0; k < tik; k++);
}



