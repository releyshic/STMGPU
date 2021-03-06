#ifndef _ILI9341_H
#define _ILI9341_H

#include <stdbool.h>
#include <stdint.h>

#ifdef STM32F40XX
#include <stm32f4xx.h>
#else
#include <stm32f10x.h>
#endif /* STM32F40XX */

#include "gpio_list.h"

#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

//-------------------------------------------------------------------------------------------//
#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09

#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13

#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0D
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29

#define ILI9341_CASET   0x2A
#define ILI9341_RASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E

#define ILI9341_PTLAR   0x30
#define ILI9341_VSCRDEF 0x33
#define ILI9341_MADCTL  0x36
#define ILI9341_VSCRSADD 0x37
#define ILI9341_IDLEOFF 0x38
#define ILI9341_IDLEON  0x39
#define ILI9341_PIXFMT  0x3A
#define ILI9341_WRCC    0x3C  // Write Continue Command

#define ILI9341_WRDBR   0x51  // display Brightness
#define ILI9341_WRCABC  0x55  // Content Adaptive Brightness Control 

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR  0xB4
#define ILI9341_DFUNCTR 0xB6
#define ILI9341_ENTRYMODE 0xB7

#define ILI9341_PWCTR1  0xC0
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7
#define ILI9341_PWCTRA  0xCB

#define ILI9341_RDID1   0xDA
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDID4   0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1

//#define ILI9341_PWCTR6  0xFC

#define MADCTL_MY       0x80
#define MADCTL_MX       0x40
#define MADCTL_MV       0x20
#define MADCTL_ML       0x10
#define MADCTL_RGB      0x00
#define MADCTL_BGR      0x08
#define MADCTL_MH       0x04

//-------------------------------------------------------------------------------------------//
// definitions for screen scrolling
#define TFT_BOT_FIXED_AREA 0  // Number of lines in bottom fixed area (lines counted from bottom of screen)
#define TFT_TOP_FIXED_AREA 0 // Number of lines in top fixed area (lines counted from top of screen)

//-------------------------------------------------------------------------------------------//
#ifdef STM32F40XX // if STM32F401CCU6 64k RAM
#define USE_FSMC      1
#else
#define USE_FSMC      0
#endif

#if USE_FSMC
#define SET_TFT_CS_HI
#define SET_TFT_CS_LOW

#define SET_TFT_DC_HI
#define SET_TFT_DC_LOW

// just a protection
#define TFT_CS_ALWAS_ACTIVE 0

#else
// free MCU from toggling CS GPIO
// Set this to 0 if not only one TFT is slave on that SPI
#define TFT_CS_ALWAS_ACTIVE 1

#define SET_TFT_DC_HI    GPIO_SET_PIN(GPIOB, TFT_DC_PIN)
#define SET_TFT_DC_LOW   GPIO_RESET_PIN(GPIOB, TFT_DC_PIN)

//#define SET_TFT_CS_HI    GPIO_SET_PIN(GPIOB, TFT_SS_PIN)
//#define SET_TFT_CS_LOW   GPIO_RESET_PIN(GPIOB, TFT_SS_PIN)
#endif /* USE_FSMC */

#ifdef STM32F10X_MD
#define SET_TFT_RES_HI     GPIO_SET_PIN(GPIOB, TFT_RES_PIN)
#define SET_TFT_RES_LOW    GPIO_RESET_PIN(GPIOB, TFT_RES_PIN)
#else
#define SET_TFT_RES_HI
#define SET_TFT_RES_LOW
#endif

#if TFT_CS_ALWAS_ACTIVE
//#undef SET_TFT_CS_HI
//#undef SET_TFT_CS_LOW
#define SET_TFT_CS_HI
#define SET_TFT_CS_LOW
// make SET_TFT_CS_LOW; always active
#define GRAB_TFT_CS     GPIO_RESET_PIN(GPIOB, TFT_SS_PIN)
#else
#define GRAB_TFT_CS     SET_TFT_CS_HI
#endif /* TFT_CS_ALWAS_ACTIVE */

#define SET_CMD()  SET_TFT_DC_LOW
#define SET_DATA() SET_TFT_DC_HI

// Cross defines to simplify code
#if USE_FSMC
#define WAIT_DMA_BSY
// #define WAIT_DMA_BSY      wait_DMA_FSMC_busy()
#define SEND_DATA(a)      sendData16_FSMC(a)
#define SEND_DATA16(a)    sendData16_FSMC(a)
#define SEND_2_DATA(a,b)   writeCommandData32_FSMC(a,b);

#define SEND_ARR16_FAST(pointer, size)   sendArr16_FSMC(pointer, size);
#define REPEAT_DATA16(data, size)        repeatData16_FSMC(data, size);
// #define SEND_ARR16_FAST(pointer, size)   sendData16_Fast_DMA_FSMC(pointer, size);
// #define REPEAT_DATA16(data, size)        repeatData16_DMA_FSMC(data, size);

#define WRITE_CMD(c)      writeCommand_FSMC(c);
#define WRITE_DATA(c)     sendData8_FSMC(c);

#else
#define WAIT_DMA_BSY      wait_DMA1_SPI1_busy()
#define SEND_DATA(a)      sendData8_SPI1(a)
#define SEND_DATA16(a)    sendData16_SPI1(a)
#define SEND_2_DATA(a,b)  sendData32_SPI1(a,b)

#define SEND_ARR16_FAST(pointer, size) sendData16_Fast_DMA1_SPI1(pointer, size)
#define REPEAT_DATA16(data, size)      repeatData16_DMA1_SPI1(data, size);

#define WRITE_CMD(c)       SET_CMD(); SEND_DATA(c);
#define WRITE_DATA(c)      SET_DATA(); SEND_DATA(c);

#endif /* USE_FSMC */

/*
 typedef union {
 uint8_t data[14];
 struct {
 uint16_t par1;
 uint16_t par2;
 uint16_t par3;
 uint16_t par4;
 uint16_t par5;
 uint16_t par6;
 uint16_t par7;
 };
 } gfx_t;

 typedef union {
 uint8_t data[14];
 struct {
 uint16_t x0;
 uint16_t y0;
 uint16_t x1;
 uint16_t y1;
 uint16_t x2;
 uint16_t y2;
 uint16_t color;
 };
 } gfx_3P1C_t; // 3 point 1 color

 typedef union {
 uint8_t data[10];
 struct {
 uint16_t x0;
 uint16_t y0;
 uint16_t x1;
 uint16_t y1;
 uint16_t color;
 };
 } gfx_2P1C_t; // 2 point 1 color

 typedef union {
 uint8_t data[6];
 struct {
 uint16_t x0;
 uint16_t y0;
 uint16_t color;
 };
 } gfx_1P1C_t; // 1 point 1 color
 */

//-------------------------------------------------------------------------------------------//
extern uint32_t _ulWidth, _ulHeight;

#if 0 //USE_FSMC

static const uint8_t initSequence[] = {
  2, ILI9341_SWRESET, 0x00,
  2, ILI9341_DISPOFF, 0x00,
  2, ILI9341_PWCTR1, 0x23,                      // Power control  (VRH[5:0])
  2, ILI9341_PWCTR2, 0x10,// Power control (SAP[2:0];BT[3:0])
  3, ILI9341_VMCTR1, 0x2B, 0x2B,// VCM control (VCOMH = 3.825)
  2, ILI9341_VMCTR2, 0xC0,// VCM control2 (VCOML = -1.375)
  2, ILI9341_MADCTL, MADCTL_MX|MADCTL_BGR,// Memory Access Control
  2, ILI9341_PIXFMT, 0x55,// Pixel Format Set (16 bit)
  3, ILI9341_FRMCTR1, 0x00, 0x18,// Frame Rate Control (In Normal Mode/Full Colors) == Frame Rate 79Hz
  2, ILI9341_ENTRYMODE, 0x07,
  0
};

#else

static const uint8_t initSequence[] = { 4, 0xEF, 0x03, 0x80,
    0x02,                    // Memory to Display Address Mapping
    4, 0xCF, 0x00, 0XC1,
    0X30,                    // Power control B
    5, 0xED, 0x64, 0x03, 0X12,
    0X81,              // Power on sequence control
    4, 0xE8, 0x85, 0x00,
    0x78,                    // Driver timing control A
    6, ILI9341_PWCTRA, 0x39, 0x2C, 0x00, 0x34,
    0x02,                          // Power control A
    2, 0xF7,
    0x20,                                // Pump ratio control
    3, 0xEA, 0x00,
    0x00,                          // Driver timing control B
    2, ILI9341_PWCTR1,
    0x23,                      // Power control  (VRH[5:0])
    2, ILI9341_PWCTR2,
    0x10,                      // Power control (SAP[2:0];BT[3:0])
    3, ILI9341_VMCTR1, 0x3e,
    0x28,                // VCM control
    2, ILI9341_VMCTR2,
    0x86,                      // VCM control2 (VML=58 VMH=58)
    2, ILI9341_MADCTL,
    MADCTL_MX | MADCTL_BGR,      // Memory Access Control
    2, ILI9341_PIXFMT,
    0x55,                      // Pixel Format Set (16 bit)
    3, ILI9341_FRMCTR1, 0x00,
    0x10,               // Frame Rate Control (In Normal Mode/Full Colors)
    3, ILI9341_FRMCTR2, 0x00,
    0x1F,               // Frame Rate Control (In Idle Mode/8 colors)
    3, ILI9341_FRMCTR2, 0x00,
    0x10,               // Frame Rate control (In Partial Mode/Full Colors)
    4, ILI9341_DFUNCTR, 0x00, 0x80,
    0x27,         // Display Function Control
    2, 0xF2,
    0x00,                                // Gamma Function Disable
    2, ILI9341_GAMMASET,
    0x01,                    // Gamma curve selected
    16, ILI9341_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37,
    0x07, 0x10, 0x03, 0x0E, 0x09,
    0x00, // Set Gamma
    16, ILI9341_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48,
    0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
    0 };

#endif /* USE_FSMC */

//-------------------------------------------------------------------------------------------//

void writeCommand(uint8_t c);
void writeData(uint8_t d);
void writeWordData(uint16_t c);

void vInit_TFT(void);
void vInit_BacklightTFT(void);

void setAddrWindow(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);
void setSqAddrWindow(uint32_t x0, uint32_t y0, uint32_t size);
void setVAddrWindow(uint32_t x0, uint32_t y0, uint32_t y1);
void setHAddrWindow(uint32_t x0, uint32_t y0, uint32_t x1);
void setAddrPixel(uint32_t x0, uint32_t y0);

void setRotation(uint32_t m);
void setScrollArea(uint16_t TFA, uint16_t BFA);
void scrollAddress(uint16_t VSP);
uint16_t scrollScreen(uint16_t lines, uint16_t yStart);
uint16_t scrollScreenSmooth(uint16_t lines, uint16_t yStart, uint8_t wait);
void setSleep(bool enable);
void setIdleMode(bool mode);
void setDispBrightness(uint16_t brightness);
void setDispBrightnessFade(uint8_t dir, uint16_t newValue, uint8_t step);
void setInvertion(bool i);
void setAdaptiveBrightness(uint8_t value);
//void setGamma(uint8_t gamma);

#endif /* _ILI9341_H */
