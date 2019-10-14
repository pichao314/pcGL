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
#include <list>
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

    //Rotate the point with respect to angle and center and return as a new point
    p2t rotate(float angle, p2t center);

    //Return a shifted new point
    p2t shift(float x, float y);

    void out();

    //Destructor
    ~p2t();
};

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
    float get_x();
    float get_y();
    float get_z();
    ~p3t();
};

p3t::p3t(float x1, float y1, float z1)
{
    x = x1;
    y = y1;
    z = z1;
}

float p3t::get_x()
{
    return x;
}

float p3t::get_y()
{
    return y;
}

float p3t::get_z()
{
    return z;
}

p3t::~p3t()
{
}


/*
===============================================================================

The world to viewer transform

===============================================================================
*/


int cursor_x = 0, cursor_y = 0;
int rotation = 0;
int textsize = 1;
int x_diff = 64;
int y_diff = 80;

int cam_x = 120;
int cam_y = 120;
int cam_z = 120;

int light_x = 22;
int light_y = 22;
int light_z = 60;

#define LOCATION_NUM    0
int colstart = 0;
int rowstart = 0;


p2t world2viewer_coord(int x_w, int y_w, int z_w)
{
    int scrn_x, scrn_y, Dist=100,x_diff=ST7735_TFTWIDTH/2,y_diff=ST7735_TFTHEIGHT/2;
    double x_p, y_p, z_p, theta,phi, rho;
    theta = acos(cam_x/sqrt(pow(cam_x,2)+pow(cam_y,2)));
    phi = acos(cam_z/sqrt(pow(cam_x,2)+pow(cam_y,2)+pow(cam_z,2)));
    rho=sqrt((pow(cam_x,2))+(pow(cam_y,2))+(pow(cam_z,2)));
    x_p = (y_w*cos(theta))-(x_w*sin(theta));
    y_p = (z_w*sin(phi))-(x_w*cos(theta)*cos(phi))-(y_w*cos(phi)*sin(theta));
    z_p = rho-(y_w*sin(phi)*cos(theta))-(x_w*sin(phi)*cos(theta))-(z_w*cos(phi));
    scrn_x = x_p*Dist/z_p;
    scrn_y = y_p*Dist/z_p;
    scrn_x = x_diff+scrn_x;
    scrn_y = y_diff-scrn_y;

    float nx = p2vx(scrn_x);
    float ny = p2vy(scrn_y);

    p2t screen(nx,ny);
    return screen;
}


/*
===============================================================================

Draw 3D coordinate

===============================================================================
*/

void draw3D()
{
    vector<p2t> cords;
    cords.push_back(world2viewer_coord(0,0,0));
    cords.push_back(world2viewer_coord(180,0,0));
    cords.push_back(world2viewer_coord(0,180,0));
    cords.push_back(world2viewer_coord(0,0,180));
    int32_t colors[3] = {RED, GREEN, BLUE};

    for (int i = 1; i < 4; i++)
    {
        drawFLine(cords[0].get_x(),cords[0].get_y(), cords[i].get_x(), cords[i].get_y(), colors[i-1]);
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

    p3t a(0,0,100);
    p3t b(0,100,0);
    p3t c(100,100,100);
    p3t d(100,0,0);

    vector<p3t> top = {a,b,c,d};

    p3t light(-5,50,200);
    p3t norm(0,0,1);
    p3t ori(0,0,0);
    vector<p2t> tp;

    for (int i = 0; i < top.size(); i++)
    {
        tp.push_back(world2viewer_coord(top[i].get_x(), top[i].get_y(), top[i].get_z()));
    }
    
    polygon topsquare(tp,GREEN);
    topsquare.plot();

    vector<p2t> proj;

    for (int i = 0; i < top.size(); i++){
        float lambda = getLambda(light, top[i], ori, norm);
        float x = top[i].get_x() + lambda * (light.get_x() - top[i].get_x());
        float y = top[i].get_y() + lambda * (light.get_x() - top[i].get_x());
        float z = top[i].get_x() + lambda * (light.get_x() - top[i].get_x());
        proj.push_back(world2viewer_coord(x,y,z));
    }

    p2t lit = world2viewer_coord(light.get_x(), light.get_y(), light.get_z());

    for (int i = 0; i < proj.size(); i++)
    {
        drawLine(lit.get_x(), lit.get_y(),proj[i].get_x(),proj[i].get_y(),RED);
    }
}


/*

===============================================================================

Homework 1

===============================================================================

*/

void rotateSquare()
{
    uint32_t color[] = {LIGHTBLUE, GREEN, DARKBLUE, BLACK, BLUE, RED, MAGENTA, WHITE, PURPLE,YELLOW,PINK, BROWN};
    /*
    p2t p1(-0.5,-0.5);
    p2t p2(0.5,-0.5);
    p2t p3(0.5,0.5);
    p2t p4(-0.5,0.5);
    vector<p2t> square = {p1,p2,p3,p4};
    polygon sq(square, GREEN);
    */

    float rate;
    cout << "Please input the rate(input 0 would be set to default rate 0.8):";
    cin >> rate;
    if (rate == 0)
    {
        rate = 0.8;
    }
    srand(time(NULL));

    for (int i = 0; i < 100; i++)
    {
        //random location
        p2t ori(p2vx(rand()%_width),p2vy(rand()%_height));
        //random size
        float size;
        size = p2vx(rand()%_width)/2.0;
        //random color
        int clr;
        clr = rand()%12;
        //Generate square vector
        p2t p1(ori.shift(-size,size));
        p2t p2(ori.shift(-size, -size));
        p2t p3(ori.shift(size, -size));
        p2t p4(ori.shift(size, size));
        vector<p2t> square = {p1,p2,p3,p4};
        polygon sq(square, color[clr]);
        for (int i = 0; i < 11; i++)
        {
            sq.plot();
            sq.shrink(rate);
        }
    }
}

/*

===============================================================================

Lab 1 Foreset

===============================================================================

*/

void drawTrunk(p2t start, p2t stop)
{
    float len = stop.get_y() - start.get_y();
    float wid = len / 5.0;

    int16_t x0 = v2px(start.get_x() - wid);
    int16_t y0 = v2py(start.get_y());
    int16_t x1 = v2px(stop.get_x() + wid);
    int16_t y1 = v2py(stop.get_y());

    cout << "Now printing "<< x0 <<','<<y0<<" to "<<x1<<','<<y1<<endl;
    if (x1 < x0)
    {
        swap(x0,x1);
    }
    if (y1 < y0)
    {
        swap(y1,y0);
    }
    for (int i = x0; i <= x1; i++)
    {
        for (int j = y0; j <= y1; j++)
        {
            drawPixel(i,j,BROWN);
        }
    }
}

//Function to draw tree
void drawTree(p2t start, p2t stop, float rate, float angle, int level)
{
    drawTrunk(start, stop);
    
    vector<p2t> tree = {start, stop};
/*
    for (int i = 0; i < level; i++)
    {
        for (int j = 0; j < pow(2,i);j++)
        {
            drawFLine(tree[pow(2,i)],tree[0])
        }
    }*/

    //int limit = pow(2,level)/2;
    //list<p2t> tree = {start, stop};


    /*
    for (int i = 0; i < limit; i ++)
    {
        //cout << "This is no " << i << "branch\n-----------------------------\n";
        p2t start = tree.front();
        tree.pop_front();
        //cout << "The start is ";
        //start.out();
        p2t stop = tree.front();
        tree.pop_front();
        //cout << "The stop is ";
        //stop.out();
        drawFLine(start.get_x(), start.get_y(), stop.get_x(), stop.get_y(), GREEN);
        float a = stop.get_x() + rate * (stop.get_x() - start.get_x());
        float b = stop.get_y() + rate * (stop.get_y() - start.get_y());
        //cout << "a,b is " << a << ' ' << b << endl;
        p2t mid(a,b);
        //cout << "The mid is ";
        //mid.out();
        //tree.push_back(stop);
        //tree.push_back(mid);
        p2t left = mid.rotate(angle,stop);
        //cout << "The left is ";
        //left.out();
        tree.push_back(stop);
        tree.push_back(left);        
        p2t right = mid.rotate(360-angle, stop);
        //cout << "The right is ";
        //right.out();
        tree.push_back(stop);
        tree.push_back(right);
        //cout << endl;
    }
    */
}

//recursive version of drawTree
void recTree(p2t start, p2t stop, float rate, float angle, int level)
{
    if (level == 0)
        return;
    drawFLine(start.get_x(), start.get_y(), stop.get_x(), stop.get_y(), GREEN);
    float a = stop.get_x() + rate * (stop.get_x() - start.get_x());
    float b = stop.get_y() + rate * (stop.get_y() - start.get_y());
    p2t mid(a,b);
    p2t left = mid.rotate(angle,stop);
    p2t right = mid.rotate(360-angle, stop);
    recTree(stop, left, rate, angle, level-1);
    //recTree(stop, mid, rate, angle, level-1);
    recTree(stop, right, rate, angle, level -1);
}

void testTree()
{
    p2t start(0,-1);
    p2t stop(0,-0.5);
    drawTrunk(start, stop);
    //drawTree(start, stop, 0.8,30,10);
}

void testRec()
{
    p2t start(0,-1);
    p2t stop(0,-0.5);
    recTree(start, stop, 0.8,30,10);   
}

//Function to generate forest with randomized location, reduction and angle

void drawForest(int level)
{
    cout << "Input q to stop...."<<endl;
    do
    {
        srand(time(NULL));
        //random location
        p2t start(p2vx(rand()%_width),p2vy(rand()%_height));

        //random root length 
        float len = p2vx(rand()%_height)/3.0;
        p2t stop(start.shift(0,len));

        //random angle
        float angle = rand()%60+30.0;

        //random reduction
        float rate = rand()%10/10.0;
        drawTree(start,stop, rate,angle,level);

    } while (cin.get() != 'q');
    cout << "Stopped!!"<<endl;
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

    //testRec();
    testTree();
    //draw3D();
    //drawShadow();
    //rotateSquare();
	return 0;

}

