/*
===============================================================================
 Name        : pcGL.h
 Author      : $pichao314
 Version     :
 Copyright   : $(copyright)
 Description : Graphic Library definition
===============================================================================
*/

#ifndef H_PCGL
#define H_PCGL

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include "LPC17xx.h"  
#include "ssp.h"
#include <iostream>

using namespace std;
/* Be careful with the port number and location number, because

some of the location may not exist in that port. */

#define PORT_NUM          1

uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];

#define ST7735_TFTWIDTH   127
#define ST7735_TFTHEIGHT  159

#define ST7735_CASET      0x2A
#define ST7735_RASET      0x2B
#define ST7735_RAMWR      0x2C
#define ST7735_SLPOUT     0x11
#define ST7735_DISPON     0x29

#define swap(x, y)        {x = x + y; y = x - y; x = x - y ;}

// defining color values

#define GREEN             0x00FF00
#define BLACK             0x000000
#define BLUE              0x0007FF
#define RED               0xFF0000
#define WHITE             0xFFFFFF
#define PURPLE            0xCC33FF

// defining constant values
#define PI                3.1415926

void spiwrite(uint8_t c);

void writecommand(uint8_t c);

void writedata(uint8_t c);

void writeword(uint16_t c);

void write888(uint32_t color, uint32_t repeat);

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color);

void lcddelay(int ms);

void lcd_init();

void drawPixel(int16_t x, int16_t y, uint32_t color);

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color);

int16_t v2px(float x);

int16_t v2py(float y);

void drawLine(float x0, float y0, float x1, float y1, uint32_t color);

#endif // !H_PCGL