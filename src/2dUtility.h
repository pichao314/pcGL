#ifndef H_2DUTIL
#define H_2DUTIL

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

class p2t
{
private:
    float _x,_y,rho;
    __uint32_t _color;
public:
    p2t(float x, float y, __uint32_t color);
    p2t(const p2t& other);
    void set(float x, float y, __uint32_t color);
    float get_x() const;
    float get_y() const;
    __uint32_t get_color() const;
    float get_r() const;
    friend ostream& operator<<(ostream& os, const p2t& dt);
    void rotate(float angle, p2t origin);
    ~p2t();
};

class polygon
{
private:
    vector<p2t> corner;
    __uint32_t _color;
public:
    polygon(vector<p2t> plg, __uint32_t color);
    polygon(const polygon& plg);
    ~polygon();
    void set(const vector<p2t> plg, __uint32_t color);
    void shrink(float rate);
    void plot();
};

polygon::polygon(vector<p2t> plg)
{
    
}

polygon::polygon(const polygon& plg)
{

}

polygon::~polygon()
{
    
}


#endif // !H_2DUTIL

