#include "2dUtility.h"

p2t::p2t(float x = 0, float y = 0, __uint32_t color = 0x000000)
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

void p2t::set(float x = NAN, float y = NAN, __uint32_t color = NAN)
{
    //Values would be set if provided
    _x = x == NAN ? x : _x;
    _y = y == NAN ? y : _y;
    rho = sqrt(_x*_x + _y*_y);
    _color = color == NAN ? color : _color;
}

float p2t::get_x() const
{
    return _x;
}
float p2t::get_y() const
{
    return _y;
}

__uint32_t p2t::get_color() const
{
    return _color;
}

float p2t::get_r() const
{
    return rho;
}

ostream& operator<<(ostream& os, const p2t& pt)
{
    os << '(' << pt.get_x << ' , '<<pt.get_y<<')';
    return os;
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
    p2t point = new p2t( dx*c - dy*s + center.get_x, dx*s + dy*c + center.get_y);
    return point;
}

p2t::~p2t()
{

}

polygon::polygon(vector<p2t> plg, __uint32_t color = 0x000000)
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

void polygon::set(const vector<p2t> plg, __uint32_t color = 0x000000)
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
    float tmp_x = corner[0].get_x;
    float tmp_y = corner[0].get_y;

    //calculate the new location by new = new + rate * (new - old)
    for (int i = 0; i < n - 1; i++)
    {
        corner[i].set(corner[i].get_x + rate * (corner[i].get_x - corner[i+1].get_x), corner[i].get_y + rate * (corner[i+1].get_y-corner[i].get_y));
    }

    //Update the last point
    corner[n-1].set(corner[n-1].get_x+rate*(tmp_x - corner[n-1].get_x),corner[n-1].get_y+rate*(tmp_y - corner[n-1].get_y));
}

void polygon::plot()
{
    int n = corner.size();

    //traverse the corner location and plot each side
    for (int i = 0; i < n; i++)
    {
        drawLine(corner[i].get_x, corner[i].get_y, corner[i%n].get_x, corner[i%n].get_y);
    }
}