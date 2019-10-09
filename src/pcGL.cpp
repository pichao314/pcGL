/*
===============================================================================
 Name        : pcGL.cpp
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
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cmath>
#include <time.h>

using namespace std;



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

#define PI 3.1415926

int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;


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

 int count = 24000;

 int i;

 for ( i = count*ms; i--; i > 0);

}



void lcd_init()

{

 int i;
 //printf("LCD Demo Begins!!!\n");
 cout << "LCD Demo Begins!!!\n";
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


void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)

{

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

  lcddelay(1);

  err -= dy;

  if (err < 0) {

   y0 += ystep;

   err += dx;

  }

 }

}


int16_t v2px(float x)
{
    //cout << "Input X is " << x << endl;
    int16_t _x = (x + 1) * _width / 2;
    //cout << "The returned X is " << _x <<endl;
    return _x;
}

int16_t v2py(float y)
{
    //cout << "Input Y is " << y << endl;
    int16_t _y = (1 - y) * _height / 2;
    //cout << "The returned Y is " << _y <<endl;
    return _y;
}

void drawFLine(float x0, float y0, float x1, float y1, uint32_t color)
{
    drawLine(v2px(x0), v2py(y0), v2px(x1), v2py(y1), color);
}


/*
===============================================================================

The 2D point class

===============================================================================
*/

class p2t
{
private:
    // the location value is saved as float, that is the standardized value from -1 to 1
    float _x,_y,rho;
    uint32_t _color;

public:
    // Constructor
    p2t(float x, float y, uint32_t color);

    //Copy constructor
    p2t(const p2t& other);

    //Setter
    void set(float x, float y, uint32_t color);

    //Getter
    float get_x() const;
    float get_y() const;
    uint32_t get_color() const;
    float get_r() const;

    //Rotate the point with respect to angle and center and return as a new point
    p2t rotate(float angle, p2t center);

    void out();

    //Destructor
    ~p2t();
};

void p2t::out()
{
  cout << "( "<<_x<<" , "<<_y<<" )";
}

p2t::p2t(float x = 0, float y = 0, uint32_t color = 0x000000)
{
    _x = x;
    _y = y;
    _color = color;

    //The distance is calculated when the constructor called
    rho = sqrt(x*x + y*y);
}

p2t::p2t(const p2t& pt)
{
    _x = pt.get_x();
    _y = pt.get_y();
    _color = pt.get_color();
    rho = pt.get_r();
}

void p2t::set(float x, float y, uint32_t color)
{
    //Values would be set if provided
    _x = x;
    _y = y;
    rho = sqrt(_x*_x + _y*_y);
    _color = color;
}

float p2t::get_x() const
{
    return _x;
}
float p2t::get_y() const
{
    return _y;
}

uint32_t p2t::get_color() const
{
    return _color;
}

float p2t::get_r() const
{
    return rho;
}


p2t p2t::rotate(float angle, p2t center)
{
    //convert the angle to radian
    angle = angle * PI / 180;
    
    //calculate the value of sin(angle) and cos(angle)
    float s = sin(angle);
    float c = cos(angle);

    //Calculate the location distance to the center
    float dx = _x - center.get_x();
    float dy = _y - center.get_y();

    //Build a new p2t with rotated location
    p2t point( dx*c - dy*s + center.get_x(), dx*s + dy*c + center.get_y());
    return point;
}

p2t::~p2t()
{

}

/*
===============================================================================

The 2D polygon class

===============================================================================
*/

// 2D polygon class, have the location of each corner and color, can be plotted and shrinked
class polygon
{
private:
    //The corner locations are stored in vector for dynamic allocating
    vector<p2t> corner;
    uint32_t _color;

public:
    //Constructor, input should be vector also
    polygon(vector<p2t> plg, uint32_t color);

    //Copy constructor
    polygon(const polygon& plg);

    //Destructor
    ~polygon();

    //Setter
    void set(const vector<p2t> plg, uint32_t color);

    //Getter
    vector<p2t> get()const;

    //Shrink the polygon with specified rate
    void shrink(float rate);

    //Plot the polygon
    void plot();

    void out();
};

void polygon::out(){
  for (int i = 0; i < corner.size();i++)
  {
    corner[i].out();
  }
  cout << endl;
}

polygon::polygon(vector<p2t> plg, uint32_t color = 0x000000)
{
    for (vector<p2t>::iterator it = plg.begin(); it != plg.end(); ++it)
    {
        corner.push_back(*it);
    }

    _color = color;
}

polygon::polygon(const polygon& plg)
{
    vector<p2t> corner = plg.get();
}

polygon::~polygon()
{
    
}

void polygon::set(const vector<p2t> plg, uint32_t color = 0x000000)
{
    polygon(plg, color);
}

vector<p2t> polygon::get() const
{
    return corner;
}

void polygon::shrink(float rate)
{
    //n is the number of sides
    int n = corner.size();

    //save the temp value for last calculate
    float tmp_x = corner[0].get_x();
    float tmp_y = corner[0].get_y();

    //calculate the new location by new = new + rate * (new - old)
    for (int i = 0; i < n - 1; i++)
    {
        corner[i].set(corner[i].get_x() + rate * (corner[i+1].get_x() - corner[i].get_x()), corner[i].get_y() + rate * (corner[i+1].get_y()-corner[i].get_y()), _color);
    }

    //Update the last point
    corner[n-1].set(corner[n-1].get_x()+rate*(tmp_x - corner[n-1].get_x()),corner[n-1].get_y()+rate*(tmp_y - corner[n-1].get_y()),_color);
}

void polygon::plot()
{
    int n = corner.size();
    //traverse the corner location and plot each side
    for (int i = 0; i < n; i++)
    {
        //cout << "\nNow plotting" << i << " to " << (i+1)%4<<endl;
        drawFLine(corner[i].get_x(), corner[i].get_y(), corner[(i+1)%n].get_x(), corner[(i+1)%n].get_y(), _color);
    }
}


/*

 Main Function main()

*/

void rotateSquare()
{
    uint32_t color[] = {LIGHTBLUE, GREEN, DARKBLUE, BLUE, RED, MAGENTA, WHITE, PURPLE};
    p2t p1(-0.5,-0.5, BLACK);
    p2t p2(0.5,-0.5, BLACK);
    p2t p3(0.5,0.5, BLACK);
    p2t p4(-0.5,0.5, BLACK);
    vector<p2t> square = {p1,p2,p3,p4};

    polygon sq(square, GREEN);

    for (int i = 0; i < 8; i++)
    {
        sq.plot();
        sq.shrink(0.2);
    }
   
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





	return 0;

}

