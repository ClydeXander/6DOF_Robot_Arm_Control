#include "stdint.h"

void SPI_Config(void);
void SPI_Write (uint8_t SPI_REG,uint32_t SPI_Data_Write);
uint32_t SPI_Receive (uint8_t SPI_REG);

