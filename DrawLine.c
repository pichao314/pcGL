/*
===============================================================================
 Name        : DrawLine.c
 Author      : $RJ
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include "LPC17xx.h"                        /* LPC17xx definitions */
#include "ssp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>



/* Be careful with the port number and location number, because

some of the location may not exist in that port. */

#define PORT_NUM            0


uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];


#define ST7735_TFTWIDTH 127
#define ST7735_TFTHEIGHT 159

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_SLPOUT 0x11
#define ST7735_DISPON 0x29



#define swap(x, y) {x = x + y; y = x - y; x = x - y ;}

// defining color values

#define LIGHTBLUE 0x00FFE0
#define GREEN 0x00FF00
#define DARKBLUE 0x000033
#define BLACK 0x000000
#define BLUE 0x0007FF
#define RED 0xFF0000
#define MAGENTA 0x00F81F
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF



int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;

typedef struct Point
{
  float x;
  float y;
}Pt;

typedef struct Branch
{
  Pt mid;
  Pt left;
  Pt ext;
  Pt right;
}Br;




void spiwrite(uint8_t c)

{

 int pnum = 0;

 src_addr[0] = c;

 SSP_SSELToggle( pnum, 0 );

 SSPSend( pnum, (uint8_t *)src_addr, 1 );

 SSP_SSELToggle( pnum, 1 );

}



void writecommand(uint8_t c)

{

 LPC_GPIO0->FIOCLR |= (0x1<<21);

 spiwrite(c);

}



void writedata(uint8_t c)

{

 LPC_GPIO0->FIOSET |= (0x1<<21);

 spiwrite(c);

}



void writeword(uint16_t c)

{

 uint8_t d;

 d = c >> 8;

 writedata(d);

 d = c & 0xFF;

 writedata(d);

}



void write888(uint32_t color, uint32_t repeat)

{

 uint8_t red, green, blue;

 int i;

 red = (color >> 16);

 green = (color >> 8) & 0xFF;

 blue = color & 0xFF;

 for (i = 0; i< repeat; i++) {

  writedata(red);

  writedata(green);

  writedata(blue);

 }

}



void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)

{

 writecommand(ST7735_CASET);

 writeword(x0);

 writeword(x1);

 writecommand(ST7735_RASET);

 writeword(y0);

 writeword(y1);

}


void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)

{

 int16_t i;

 int16_t width, height;

 width = x1-x0+1;

 height = y1-y0+1;

 setAddrWindow(x0,y0,x1,y1);

 writecommand(ST7735_RAMWR);

 write888(color,width*height);

}



void lcddelay(int ms)

{

 int count = 12000;

 int i;

 for ( i = count*ms; i--; i > 0);

}



void lcd_init()

{

 int i;
 printf("LCD Demo Begins!!!\n");
 // Set pins P0.16, P0.21, P0.22 as output
 LPC_GPIO0->FIODIR |= (0x1<<16);

 LPC_GPIO0->FIODIR |= (0x1<<21);

 LPC_GPIO0->FIODIR |= (0x1<<22);

 // Hardware Reset Sequence
 LPC_GPIO0->FIOSET |= (0x1<<22);
 lcddelay(500);

 LPC_GPIO0->FIOCLR |= (0x1<<22);
 lcddelay(500);

 LPC_GPIO0->FIOSET |= (0x1<<22);
 lcddelay(500);

 // initialize buffers
 for ( i = 0; i < SSP_BUFSIZE; i++ )
 {

   src_addr[i] = 0;
   dest_addr[i] = 0;
 }

 // Take LCD display out of sleep mode
 writecommand(ST7735_SLPOUT);
 lcddelay(200);

 // Turn LCD display on
 writecommand(ST7735_DISPON);
 lcddelay(200);

}




void drawPixel(int16_t x, int16_t y, uint32_t color)

{

 if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))

 return;

 setAddrWindow(x, y, x + 1, y + 1);

 writecommand(ST7735_RAMWR);

 write888(color, 1);

}



/*****************************************************************************


** Descriptions:        Draw line function

**

** parameters:           Starting point (x0,y0), Ending point(x1,y1) and color

** Returned value:        None

**


*****************************************************************************/
Pt v2p(Pt p)
{
  Pt n = {(p.x + 1 ) * _width/2,(1 - p.y) * _height/2};
  return n;
}

void drawLine(Pt st, Pt ed, float color)
{
  Pt start, end;
  start = v2p(st);
  end = v2p(ed);

 int16_t x0,y0,x1,y1;
 
 x0 = start.x;
 y0 = start.y;
 x1 = end.x;
 y1 = end.y;

 int16_t slope = abs(y1 - y0) > abs(x1 - x0);

 if (slope) {

  swap(x0, y0);

  swap(x1, y1);

 }

 if (x0 > x1) {

  swap(x0, x1);

  swap(y0, y1);

 }

 int16_t dx, dy;

 dx = x1 - x0;

 dy = abs(y1 - y0);

 int16_t err = dx / 2;

 int16_t ystep;

 if (y0 < y1) {

  ystep = 1;

 }

 else {

  ystep = -1;

 }

 for (; x0 <= x1; x0++) {

  if (slope) {

   drawPixel(y0, x0, color);

  }

  else {

   drawPixel(x0, y0, color);

  }

  err -= dy;

  if (err < 0) {

   y0 += ystep;

   err += dx;

  }

  lcddelay(1);

 }

}


void drawRec(Pt* req, uint32_t color)
{
  for(int i = 0; i < 4; i++)
  {
    drawLine(req[i], req[(i+1)%4],color);
  }
}

Pt* nextRec(Pt* req, float rate, uint32_t color){
  Pt* next = malloc(sizeof(Pt)*4);

  for (int i = 0; i < 4; i++){
    next[i].x = req[i].x + rate * (req[(i+1)%4].x - req[i].x);
    next[i].y = req[i].y + rate * (req[(i+1)%4].y - req[i].y);
  }
  free(req);
  return next;
}


void recreq(Pt* req, int level, float rate, uint32_t color){

  rate = 1- rate;

  Pt* seq = malloc(sizeof(Pt)*4);
  seq = req;

   for (int j = 0; j < level; j++){
     drawRec(seq, color);
     seq = nextRec(seq,rate,color);
   }
}


Pt rot(Pt p, Pt o, float angle){

	angle = angle * (3.14285/180);
	float s = sin(angle);
	float c = cos(angle);


	Pt t = {p.x - o.x,p.y - o.y};
	Pt rt ={ t.x * c - t.y * s, t.x * s + t.y * c};

	Pt new ={ rt.x + o.x,rt.y + o.y};

	return new;
}


/* Pt array version
Pt* drawBranch(Pt start, Pt end, float rate, int angle, uint32_t color)
{
  Pt* next = malloc(sizeof(Pt)*4);
  drawLine(start, end, color);

  Pt mid = {end.x + rate * (end.x - start.x), end.y + rate * (end.y - start.y)};
  next[0] = end;
  next[1] = mid;
  next[2] = rot(mid, end, angle);
  next[3] = rot(mid, end, 360 - angle);

  printf("++++++++++++++++\nThe rotted value is: ");
  for (int i = 0; i < 4; i++){
	  printf("Pt%d: %f, %f\t",i, next[i].x, next[i].y);
  }
  printf("\n");
  return(next);
}

*/


void drawTree(Pt* init, float rate, int angle, int level, uint32_t color)
{
  Pt** cur = malloc(sizeof(Pt*));
  //cur[0] = malloc(sizeof(Pt)*4);
  cur[0] = drawBranch(init[0], init[1], rate, angle, color);
  Pt** next;

  for (int i = 2; i <= level; i++){
    /*
    draw each level by l, m, r, each level owns 3^i branches, 
    output # (equals to input for next level) is 4*3^(i-1)
    */
   printf("Now printing the %d level!\n-------------------\n", i);
   int num = pow(3,i-1);

   //malloc the memory for next level

   next = malloc(sizeof(Pt*) * num);
   printf("%d locs of PT* malloc\n", num);

   //traverse each branch on current level
   for (int j = 0; j < num; j++){
     printf("Start to draw %d branch\n", j);
     //draw three branch l,m,r and return the pt for next level
     for (int k = 0; k < 4; k++){
       next[j * 4 + k] = drawBranch(cur[j][0], cur[j][k+1],rate, angle, color);
     }

   }
    //free the current level and refer to the next level
   free(cur);
   cur = next;
   
  }
}



/*

Recursive version

******

void drawTree(Pt start, Pt end, float rate, int angle,int level, uint32_t color){

  if (level ==0) return;

	Pt c = {end.x + rate * (end.x - start.x), end.y + rate * (end.y - start.y)};

	drawLine(c, end, GREEN);
  Pt rtl, rtr;

	rtl = rot(c, end, angle);
	drawLine(rtl, end, GREEN);

	rtr = rot(c, end, 360 - angle);
	drawLine(rtr, end, GREEN);

  drawTree(end, c, rate,angle, level - 1, color);
  drawTree(end, rtl, rate, angle, level - 1, color);
  drawTree(end, rtr, rate, angle, level - 1, color);
}


void drawTrees(Pt* req, int level, float rate, uint32_t color){
  drawLine(req[0],req[1],GREEN);
  for (int j = 0; j < level; j++){
    Pt* tmp = malloc(sizeof(Pt)*2*3*(j+1));
    for (int i = 0; i <3; i++){

    }
    drawTree(req[0],req[1],0.8,30,GREEN);
    free(tmp);
    
  }
}
*/

void draw2DCordinate(){
   Pt xn = {-1,0};
   Pt xp = {1,0};
   Pt yn = {0,-1};
   Pt yp = {0,1};

	 drawLine(xn,xp,PURPLE);
	 drawLine(yn,yp,PURPLE);
}

void drawMidSquare(){
  Pt seq2[4] = {{-0.5,-0.5},{0.5,-0.5},{0.5,0.5},{-0.5,0.5}};
  drawRec(seq2, BLUE);
}

void screenSaver(){
  Pt se[4] = {{-1,-1},{1,-1},{1,1},{-1,1}};
  recreq(se, 11, 0.8, BLUE);
}

void drawForest(){
  Pt init[2] = {{0,-1},{0,-0.5}};
  drawTree(init, 0.8, 30, 3, GREEN);
}

int main (void)

{

	 uint32_t pnum = PORT_NUM;

	 pnum = 0 ;

	 if ( pnum == 0 )
		 SSP0Init();

	 else
		 puts("Port number is not correct");

	 lcd_init();

	 fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, BLACK);

  //draw2DCordinate();
  //drawMidSquare();
  //screenSaver();
  drawForest();

	 return 0;
}

