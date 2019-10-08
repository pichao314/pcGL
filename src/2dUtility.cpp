#include "2dUtility.h"

p2t::p2t(float x = 0, float y = 0, __uint32_t color = 0x000000)
{
    _x = x;
    _y = y;
    _color = color;
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

void p2t::rotate(float angle, p2t origin)
{

}

p2t::~p2t()
{

}