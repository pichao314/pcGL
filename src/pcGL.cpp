/*
===============================================================================
 Name        : pcGL.cpp
 Author      : $PC
 Version     :
 Copyright   : $(copyright)
 Description : 2D and 3D Graphic Engine
===============================================================================
*/

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include "LPC17xx.h"                        //LPC17xx definitions
#include "ssp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <cmath>
#include <time.h>

using namespace std;

/* Be careful with the port number and location number, because
some of the location may not exist in that port. */

#define PORT_NUM        0
#define LOCATION_NUM    0

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

// defining colors

#define LIGHTBLUE 0x00FFE0
#define GREEN 0x00FF00
#define DARKBLUE 0x000033
#define BLACK 0x000000
#define BLUE 0x0007FF
#define RED 0xFF0000
#define MAGENTA 0x00F81F
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF
#define YELLOW 0xFFFF00
#define PINK 0xFFC0CB
#define BROWN 0xA52A2A

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
* Descriptions:        Draw line function
* parameters:           Starting point (x0,y0), Ending point(x1,y1) and color
* Returned value:        None
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


/*
===============================================================================
The virtual - physical transform function
===============================================================================
*/

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

float p2vx(int16_t x)
{
    float _x = 2.0 * x / _width - 1;
    return _x;
}

float p2vy(int16_t y)
{
    float _y = 1 - 2.0 * y /_height;
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
    p2t(float x, float y);

    //Copy constructor
    p2t(const p2t& other);

    //Setter
    void set(float x, float y);

    //Getter
    float get_x() const;
    float get_y() const;
    float get_r() const;
    float gets() const;
    float dist(p2t& other);

    //Rotate the point with respect to angle and center and return as a new point
    p2t rotate(float angle, p2t center);

    //Return a shifted new point
    p2t shift(float x, float y);

    void out();

    //Destructor
    ~p2t();
};

float p2t::dist(p2t& other)
{
    return( sqrt(pow((_x - other.get_x()),2) + pow((_y - other.get_y()),2)));
}

void p2t::out()
{
  cout << "( "<<_x<<" , "<<_y<<" )";
}

p2t::p2t(float x, float y)
{
    _x = x;
    _y = y;

    //The distance is calculated when the constructor called
    rho = sqrt(x*x + y*y);
}

p2t::p2t(const p2t& pt)
{
    _x = pt.get_x();
    _y = pt.get_y();
    rho = pt.get_r();
}

void p2t::set(float x, float y)
{
    //Values would be set if provided
    _x = x;
    _y = y;
    rho = sqrt(_x*_x + _y*_y);
}

float p2t::get_x() const
{
    return _x;
}
float p2t::get_y() const
{
    return _y;
}

float p2t::get_r() const
{
    return rho;
}

float p2t::gets() const{
    return _y * _width + _width;
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

p2t p2t::shift(float x, float y)
{
    p2t point(_x+x, _y+y);
    return point;
}

p2t::~p2t()
{
}

void drawFLine(p2t start, p2t stop, uint32_t color)
{
    float x0 = start.get_x();
    float x1 = stop.get_x();
    float y0 = start.get_y();
    float y1 = stop.get_y();
    
    drawLine(v2px(x0), v2py(y0), v2px(x1), v2py(y1), color);
}

void drawLine(p2t start, p2t stop, uint32_t color)
{
    int16_t x0 = start.get_x();
    int16_t y0 = start.get_y();
    int16_t x1 = stop.get_x();
    int16_t y1 = stop.get_y();
    drawLine(x0,y0,x1,y1,color);
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
    set<int16_t> border;

public:
    //Constructor, input should be vector also
    polygon(vector<p2t> plg, uint32_t color);

    //Copy constructor
    polygon(const polygon& plg);

    //Destructor
    ~polygon();

    //Getter
    vector<p2t> get()const;

    //Shrink the polygon with specified rate
    void shrink(float rate);

    //Plot the polygon
    void plot();

    //Physical plot
    void pplot();

    //Physical fill
    //void fill(p2t seed, uint32_t clr);

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
        corner[i].set(corner[i].get_x() + rate * (corner[i+1].get_x() - corner[i].get_x()), corner[i].get_y() + rate * (corner[i+1].get_y()-corner[i].get_y()));
    }

    //Update the last point
    corner[n-1].set(corner[n-1].get_x()+rate*(tmp_x - corner[n-1].get_x()),corner[n-1].get_y()+rate*(tmp_y - corner[n-1].get_y()));
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

void polygon::pplot()
{
    int n = corner.size();
    //traverse the corner location and plot each side
    for (int i = 0; i < n; i++)
    {
        //cout << "\nNow plotting" << i << " to " << (i+1)%4<<endl;
        drawLine(corner[i].get_x(), corner[i].get_y(), corner[(i+1)%n].get_x(), corner[(i+1)%n].get_y(), _color);
    }
}

/*
===============================================================================

The 3D point class

===============================================================================
*/

class p3t
{
private:
    float x,y,z;
public:
    p3t(float x, float y, float z);
    float get_x() const;
    float get_y() const;
    float get_z() const;
    float dist(p3t& pt) const;
    float dist(float _x, float _y, float _z) const;
    p3t shift(float _x, float _y, float _z) const;
    ~p3t();
};

p3t::p3t(float x1, float y1, float z1)
{
    x = x1;
    y = y1;
    z = z1;
}

float p3t::get_x() const
{
    return x;
}

float p3t::get_y() const
{
    return y;
}

float p3t::get_z() const
{
    return z;
}

float p3t::dist(p3t& pt) const{
    return (sqrt(pow((x - pt.get_x()),2)+pow((y - pt.get_y()),2)+pow((z - pt.get_z()),2)));
}

float p3t::dist(float _x, float _y, float _z) const{
    return (sqrt(pow((x - _x),2)+pow((y - _y),2)+pow((z - _z),2)));
}

p3t p3t::shift(float _x = 0, float _y = 0, float _z = 0) const{
    p3t sf(x+_x,y+_y,z+_z);
    return sf;
}



p3t::~p3t()
{
}

/*
===============================================================================

The world to viewer transform

===============================================================================
*/

//process with the physical location
p2t world2viewer(int x, int y, int z)
{
    int d=50,
        x_diff=ST7735_TFTWIDTH/2,
        y_diff=ST7735_TFTHEIGHT/2,
        cam_x = 150,
        cam_y = 150,
        cam_z = 250;

    float   rho = sqrt((pow(cam_x,2))+(pow(cam_y,2))+(pow(cam_z,2))),
            theta = acos(cam_x/sqrt(pow(cam_x,2)+pow(cam_y,2))),
            phi = acos(cam_z/sqrt(pow(cam_x,2)+pow(cam_y,2)+pow(cam_z,2)));
    
    float   x_v = y*cos(theta) - x*sin(theta),
            y_v = z*sin(phi) - x*cos(theta) * cos(phi) - y*cos(phi) * sin(theta),
            z_v = rho - y*sin(phi) * cos(theta) - x*sin(phi) * cos(theta) - z*cos(phi);

    int w_x = x_v * d / z_v + x_diff,
        w_y = y_diff - y_v * d / z_v;
/*
    float   w_x = p2vx(x_v * d / z_v + x_diff),
            w_y = p2vy(y_diff - y_v * d / z_v);
*/

    p2t screen(w_x,w_y);
    return screen;
}

p2t world2viewer(p3t pt)
{
    int x = pt.get_x(),
        y = pt.get_y(),
        z = pt.get_z();
    return (world2viewer(x,y,z));
}
/*
===============================================================================

Draw 3d coordinates

===============================================================================
*/

void draw3Dcoord(){
    vector<p3t> cord = {
        p3t(0,0,0),
        p3t(250,0,0),
        p3t(0,250,0),
        p3t(0,0,250)
    };
    vector<p2t> coord;
    uint32_t color[3] = {RED,GREEN,BLUE};

    for (int i=0; i<4; i++){
        coord.push_back(world2viewer(cord[i]));
    }
    for (int i=1; i<4; i++){
        drawLine(coord[0],coord[i],color[i-1]);
    }
}


/*
===============================================================================

Draw cube

===============================================================================
*/

void drawCube(){
    vector<p3t> bot3 = {
        p3t(0,0,0),
        p3t(100,0,0),
        p3t(100,100,0),
        p3t(0,100,0)
    };
    vector<p2t> bot;
    vector<p2t> top;
    for (int i=0; i<4; i++){
        bot.push_back(world2viewer(bot3[i].shift(0,0,10)));
        top.push_back(world2viewer(bot3[i].shift(0,0,110)));
    }

    polygon bottom(bot,YELLOW);
    polygon upper(top,YELLOW);
    upper.pplot();
    //upper.fill(upper.get()[0].shift(0,-10),YELLOW);
    bottom.pplot();

    for (int i=0; i<4; i++){
       // drawLine(bot[i],bot[(i+1)%4],YELLOW);
        drawLine(bot[i],top[i],YELLOW);
        //drawLine(top[i],top[(i+1)%4],YELLOW );
    }
}


/*
===============================================================================

Draw shadow

===============================================================================
*/

float getLambda(p3t light, p3t point, p3t arbi, p3t norm)
{
    p3t up(arbi.get_x() - point.get_x(), arbi.get_y() - point.get_y(), arbi.get_z() - point.get_z());
    p3t down(light.get_x() - point.get_x(), light.get_y() - point.get_y(), light.get_z() - point.get_z());
    float lambda = (norm.get_x()*up.get_x() + norm.get_y()*up.get_y()+norm.get_z()*up.get_z())/(norm.get_x()*down.get_x() + norm.get_y()*down.get_y()+norm.get_z()*down.get_z());
    return lambda;
}


void drawShadow()
{
    vector<p3t> bot3 = {
        p3t(0,0,0),
        p3t(100,0,0),
        p3t(100,100,0),
        p3t(0,100,0)
    };
    vector<p3t> top3;

    vector<p2t> bot;
    vector<p2t> top;
    vector<p2t> proj;

    for (int i=0; i<4; i++){
        bot.push_back(world2viewer(bot3[i].shift(0,0,10)));
        top.push_back(world2viewer(bot3[i].shift(0,0,110)));
        top3.push_back(bot3[i].shift(0,0,110));
    }

    p3t light(-50,50,250);
    p3t norm(0,0,10);
    p3t ori(0,0,0);

    for (int i = 0; i < top3.size(); i++){
        float lambda = getLambda(light, top3[i], ori, norm);
        cout << "The lambda value of "<<i+1<<"th corner is "<< lambda <<endl;
        float x = top3[i].get_x() + lambda * (light.get_x() - top3[i].get_x());
        float y = top3[i].get_y() + lambda * (light.get_y() - top3[i].get_y());
        float z = top3[i].get_z() + lambda * (light.get_z() - top3[i].get_z());
        cout << "The location is ("<<x<<","<<y<<","<<z<<")\n";
        proj.push_back(world2viewer(x,y,z));
    }

    p2t lit = world2viewer(light.get_x(), light.get_y(), light.get_z());

    for (int i = 0; i < proj.size(); i++)
    {
        drawLine(lit,proj[i],PURPLE);
        drawLine(proj[i],proj[(i+1)%4],WHITE);
    }

    for (int i=0; i<4; i++){
        drawLine(bot[i],bot[(i+1)%4],YELLOW);
        drawLine(bot[i],top[i],YELLOW);
        drawLine(top[i],top[(i+1)%4],YELLOW );
    }
}

/*

===============================================================================

Lab 2:Diffuse Reflection

===============================================================================

*/

class dfpolygon
{
private:
    //The corner locations are stored in vector for dynamic allocating
    vector<p2t> corner;
    vector<uint32_t> cnr_clr;
    vector<p2t> border_l;
    vector<p2t> border_r;


public:
    //Constructor, input should be vector also
    dfpolygon(vector<p2t> plg, vector<uint32_t> colors);

    void ddal1();
    void ddal2();
    void ddar1();
    void ddar2();
    void dda(p2t a, p2t b, uint32_t clr1, uint32_t clr2);
    //Plot the polygon
    void plot();
    void getlen();
    void bdout();

    //Fill with bilinear interpolation
    void fill();
};

void dfpolygon::bdout(){
    cout << "Left border:";
    for (vector<p2t>::iterator it = border_l.begin(); it != border_l.end(); ++it)
    {
        it->out();
    }
    cout << "Right border:";
    for (vector<p2t>::iterator it = border_r.begin(); it != border_r.end(); ++it)
    {
        it->out();
    }
    cout << endl;
}

void dfpolygon::getlen(){
    cout << "length of left is "<<border_l.size() << endl;
    cout << "length of right is "<<border_r.size() << endl;
}

dfpolygon::dfpolygon(vector<p2t> plg, vector<uint32_t> colors)
{
    for (vector<p2t>::iterator it = plg.begin(); it != plg.end(); ++it)
    {
        corner.push_back(*it);
    }
    for (vector<uint32_t>::iterator it = colors.begin(); it != colors.end(); ++it)
    {
        cnr_clr.push_back(*it);
    }
}

void toRGB(uint32_t clr, uint32_t* rgb){
    rgb[0] = clr >> 16;
    rgb[1] = (clr >> 8) - (rgb[0]<<8);
    rgb[2] = clr - (rgb[0]<<16) - (rgb[1]<<8);
}

uint32_t fromRGB(uint32_t* rgb){
    uint32_t clr = (rgb[0] << 16) + (rgb[1]<<8) + rgb[2];
    return clr;
}

uint32_t color_calc(p2t a, p2t b, p2t c, uint32_t clr0, uint32_t clr1)
{
    uint32_t rgb0[3],rgb1[3],rgb2[3];
    toRGB(clr0, rgb0);
    toRGB(clr1, rgb2);

    rgb1[0] = rgb2[0] < rgb0[0] ? rgb0[0] - abs(int(rgb2[0] - rgb0[0])) * a.dist(b)/a.dist(c):rgb0[0] + abs(int(rgb2[0] - rgb0[0])) * a.dist(b)/a.dist(c);
    rgb1[1] = rgb2[1] < rgb0[1] ? rgb0[1] - abs(int(rgb2[1] - rgb0[1])) * a.dist(b)/a.dist(c):rgb0[1] + abs(int(rgb2[1] - rgb0[1])) * a.dist(b)/a.dist(c);
    rgb1[2] = rgb2[2] < rgb0[2] ? rgb0[2] - abs(int(rgb2[2] - rgb0[2])) * a.dist(b)/a.dist(c):rgb0[2] + abs(int(rgb2[2] - rgb0[2])) * a.dist(b)/a.dist(c);


    for (int i = 0; i < 3; i ++)
    {
        if (rgb1[i] > 0xFF)
            rgb1[i] = 0xFF;
    }

    uint32_t color;
    color = fromRGB(rgb1);
    return color;
}

uint32_t diffuse(p3t source, p3t corner, float* k){
    uint32_t rgb[3];
    p3t origin(0,0,0);
    p3t normal(0,0,1);
    p3t r(source.get_x() - corner.get_x(),source.get_y() - corner.get_y(), source.get_z() - corner.get_z());
    float upper = normal.get_x()*r.get_x() + normal.get_y()*r.get_y()+normal.get_z()*r.get_z();
    float bot = normal.dist(origin) * pow(r.dist(origin),3);
    rgb[0] = k[0] * 3.4 * 3000000 * upper / bot;
    rgb[1] = k[1] * 1000000 * upper / bot;
    rgb[2] = k[2] * 1000000 * upper / bot;
    
    uint32_t color = fromRGB(rgb);
    //return RED;
    return color;
}

void dfpolygon::dda(p2t a, p2t b, uint32_t clr1, uint32_t clr2)
{   
    //cout << "clr1 is "<<hex<<clr1<<", clr2 is "<<hex<<clr2<<endl;
    int16_t x0 = a.get_x();
    int16_t y0 = a.get_y();
    int16_t x1 = b.get_x();
    int16_t y1 = b.get_y();
    uint32_t color;

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
            p2t tmp(y0,x0);
            color = color_calc(a,tmp,b,clr1,clr2);
            drawPixel(y0, x0, color);
        }
        else {
            p2t tmp(x0,y0);
            color = color_calc(a,tmp,b,clr1,clr2);
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

void dfpolygon::ddal1()
{   
    int16_t x0 = corner[0].get_x();
    int16_t y0 = corner[0].get_y();
    int16_t x1 = corner[3].get_x();
    int16_t y1 = corner[3].get_y();
    uint32_t color;

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
            p2t tmp(y0,x0);
            border_l.push_back(tmp);
            color = color_calc(corner[0],tmp,corner[3],cnr_clr[0],cnr_clr[3]);
            drawPixel(y0, x0, color);
        }
        else {
            p2t tmp(x0,y0);
            border_l.push_back(tmp);
            color = color_calc(corner[0],tmp,corner[3],cnr_clr[0],cnr_clr[3]);
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

void dfpolygon::ddal2()
{   
    int16_t x0 = corner[3].get_x();
    int16_t y0 = corner[3].get_y();
    int16_t x1 = corner[2].get_x();
    int16_t y1 = corner[2].get_y();
    uint32_t color;

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
            p2t tmp(y0,x0);
            border_l.push_back(tmp);
            color = color_calc(corner[3],tmp,corner[2],cnr_clr[3],cnr_clr[2]);
            drawPixel(y0, x0, color);
        }
        else {
            p2t tmp(x0,y0);
            border_l.push_back(tmp);
            color = color_calc(corner[3],tmp,corner[2],cnr_clr[3],cnr_clr[2]);
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

void dfpolygon::ddar1()
{   
    int16_t x0 = corner[0].get_x();
    int16_t y0 = corner[0].get_y();
    int16_t x1 = corner[1].get_x();
    int16_t y1 = corner[1].get_y();
    uint32_t color;

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
            p2t tmp(y0,x0);
            border_r.push_back(tmp);
            color = color_calc(corner[0],tmp,corner[1],cnr_clr[0],cnr_clr[1]);
            drawPixel(y0, x0, color);
        }
        else {
            p2t tmp(x0,y0);
            border_r.push_back(tmp);
            color = color_calc(corner[0],tmp,corner[1],cnr_clr[0],cnr_clr[1]);
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

void dfpolygon::ddar2()
{   
    int16_t x0 = corner[1].get_x();
    int16_t y0 = corner[1].get_y();
    int16_t x1 = corner[2].get_x();
    int16_t y1 = corner[2].get_y();
    uint32_t color;

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
            p2t tmp(y0,x0);
            border_r.push_back(tmp);
            color = color_calc(corner[1],tmp,corner[2],cnr_clr[1],cnr_clr[2]);
            drawPixel(y0, x0, color);
        }
        else {
            p2t tmp(x0,y0);
            border_r.push_back(tmp);
            color = color_calc(corner[1],tmp,corner[2],cnr_clr[1],cnr_clr[2]);
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

void dfpolygon::plot()
{
    //cout << "The color is" << hex << cnr_clr[0] <<cnr_clr[1] <<cnr_clr[2]<<cnr_clr[3]<<endl;
    ddal1();
    ddal2();
    ddar1();
    ddar2();
    return;
}

void dfpolygon::fill()
{
    for (int i = 0; i < border_l.size();i++)
    {
        for (int j = 0; j < border_r.size();j++)
        {
            if (border_r[j].get_y() == border_l[i].get_y())
            {
                uint32_t clr1, clr2;
                //cout << "The color is" << hex << cnr_clr[0] <<cnr_clr[1] <<cnr_clr[2]<<cnr_clr[3]<<endl;
                if (border_l[i].get_y() <= corner[3].get_y())
                {
                    clr1 = color_calc(corner[0],border_l[i],corner[3],cnr_clr[0],cnr_clr[3]);
                }
                else
                {
                    clr1 = color_calc(corner[3],border_l[i],corner[2],cnr_clr[3],cnr_clr[2]);
                }

                


                if (border_l[i].get_y() <= corner[1].get_y())
                {
                    clr2 = color_calc(corner[0],border_r[j],corner[1],cnr_clr[0],cnr_clr[1]);
                    //dda(border_l[i],border_r[j],clr2,clr1);
                }
                else
                {
                    clr2 = color_calc(corner[1],border_r[j],corner[2],cnr_clr[1],cnr_clr[2]);
                    //dda(border_l[i],border_r[j],clr1,clr2);
                }

                
                if (j == 4 || j==5){
                    //cout << "The calculated color is "<<hex<<clr1<<","<<hex<<clr2<<endl;
                }
                dda(border_l[i],border_r[j],clr1,clr2);
            }
        }
    }
    return;
}

void diffShadow()
{
    vector<p3t> bot3 = {
        p3t(0,0,0),
        p3t(100,0,0),
        p3t(100,100,0),
        p3t(0,100,0)
    };
    vector<p3t> top3;

    vector<p2t> bot;
    vector<p2t> top;
    vector<p2t> proj;

    for (int i=0; i<4; i++){
        bot.push_back(world2viewer(bot3[i].shift(0,0,10)));
        top.push_back(world2viewer(bot3[i].shift(0,0,110)));
        top3.push_back(bot3[i].shift(0,0,110));
    }

    p3t light(-50,50,300);
    p3t norm(0,0,10);
    p3t ori(0,0,0);

    for (int i = 0; i < top3.size(); i++){
        float lambda = getLambda(light, top3[i], ori, norm);
        //cout << "The lambda value of "<<i+1<<"th corner is "<< lambda <<endl;
        float x = top3[i].get_x() + lambda * (light.get_x() - top3[i].get_x());
        float y = top3[i].get_y() + lambda * (light.get_y() - top3[i].get_y());
        float z = top3[i].get_z() + lambda * (light.get_z() - top3[i].get_z());
        //cout << "The location is ("<<x<<","<<y<<","<<z<<")\n";
        proj.push_back(world2viewer(x,y,z));
    }

    vector<uint32_t> colors{WHITE,WHITE,WHITE,WHITE};
    dfpolygon shadow(proj,colors);
    shadow.plot();
    shadow.fill();

}

void drawDiff(){
    vector<p3t> bot3 = {
        p3t(0,0,0),
        p3t(0,100,0),
        p3t(100,100,0),
        p3t(100,0,0)
    };
    vector<p2t> bot;
    vector<p2t> top;
    vector<p2t> left;
    vector<p2t> right;
    for (int i=0; i<4; i++){
        bot.push_back(world2viewer(bot3[i].shift(0,0,10)));
        top.push_back(world2viewer(bot3[i].shift(0,0,110)));
    }

    vector<uint32_t> colorsl{RED,RED,RED,RED};
    left.push_back(world2viewer(bot3[3].shift(0,0,110)));
    left.push_back(world2viewer(bot3[2].shift(0,0,110)));
    left.push_back(world2viewer(bot3[2]));
    left.push_back(world2viewer(bot3[3]));

    vector<uint32_t> colorsr{GREEN,GREEN,GREEN,GREEN};
    right.push_back(world2viewer(bot3[1].shift(0,0,110)));
    right.push_back(world2viewer(bot3[1]));
    right.push_back(world2viewer(bot3[2]));
    right.push_back(world2viewer(bot3[2].shift(0,0,110)));

    p3t light(-50,50,250);
    float k[3] = {0.8,0,0};
    uint32_t cncl0 =  diffuse(light,bot3[0].shift(0,0,110),k);
    uint32_t cncl1 =  diffuse(light,bot3[1].shift(0,0,110),k);
    uint32_t cncl2 =  diffuse(light,bot3[2].shift(0,0,110),k);
    uint32_t cncl3 =  diffuse(light,bot3[3].shift(0,0,110),k);


    uint32_t rgb[3];
    toRGB(cncl2,rgb);
    rgb[0] *= 0.1;
    cncl2 = fromRGB(rgb);

    toRGB(cncl3,rgb);
    rgb[0] *= 0.1;
    cncl3 = fromRGB(rgb);

    vector<uint32_t> colors{cncl0,cncl1,cncl2,cncl3};
    vector<uint32_t> colors1{YELLOW,YELLOW,YELLOW,YELLOW};
    dfpolygon bottom(bot,colors1);
    dfpolygon upper(top,colors);

    dfpolygon leftp(left,colorsl);
    dfpolygon rightp(right, colorsr);

    diffShadow();

    leftp.plot();
    leftp.fill();
    rightp.plot();
    rightp.fill();

    //bottom.plot();
    //bottom.fill();
    /*
    for (int i=0; i<4; i++){
        //drawLine(bot[i],bot[(i+1)%4],YELLOW);
        drawLine(bot[i],top[i],YELLOW);
        //drawLine(top[i],top[(i+1)%4],YELLOW );
    }
    */
    upper.plot();
    upper.fill();
    
}

/*
===============================================================================
Main Function
===============================================================================
*/

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

    draw3Dcoord();
    drawDiff();
    //drawCube();
    //drawShadow();
	return 0;
}

