#ifndef SPI_H
#define SPI_H

#include <stdint.h>

// ------------------------- SPI_1 ------------------------- //
//sck - pa5; miso - pa6; mosi - pa7;
#define GPIO_SPI_LCD            GPIOA
#define GPIO_Pin_SPI_LCD_SCK    GPIO_Pin_5      // SCK
#define GPIO_Pin_SPI_LCD_MISO   GPIO_Pin_6     // DO
#define GPIO_Pin_SPI_LCD_MOSI   GPIO_Pin_7     // DI

// remap
//sck - pb3; miso - pb4; mosi - pb5;   
/*
#define SCK_PIN GPIO_Pin_3      // SCK
#define MIS0_PIN GPIO_Pin_4     // DO
#define MOSI_PIN GPIO_Pin_5     // DI   
*/

#define SET_SCK_HI      (GPIO_SPI_LCD->BSRR = GPIO_Pin_SPI_LCD_SCK);
#define SET_SCK_LOW     (GPIO_SPI_LCD->BRR = GPIO_Pin_SPI_LCD_SCK);

#define SET_MOSI_HI     (GPIO_SPI_LCD->BSRR = GPIO_Pin_SPI_LCD_MOSI);
   
// wait untill all data will be sent
#define WAIT_FREE_TX    while ((SPI1->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET);
#define WAIT_FOR_BSY    while ((SPI1->SR & SPI_I2S_FLAG_BSY) != (uint16_t)RESET);

//#define WAIT_FOR_END    while((SPI1->DR & (SPI_I2S_FLAG_TXE | SPI_I2S_FLAG_BSY)) != 0x02)
// ------------------------------------------------------- //

// ------------------------- SPI_1 ------------------------- //
void init_SPI1(void);
void sendData8_SPI1(uint8_t data);
void sendArr8_SPI1(void *data, uint32_t size);
void sendData16_SPI1(uint16_t data);
void sendData32_SPI1(uint16_t data0, uint16_t data1);
void sendArr16_SPI1(void *data, uint32_t size);
void repeatData16_SPI1(uint16_t data, uint32_t size);

// ------------------------- SPI_2 ------------------------- //
void init_SPI2(void);

#endif /* SPI_H */
