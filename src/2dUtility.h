#ifndef H_2DUTIL
#define H_2DUTIL

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "pcGL.h"

using namespace std;

// 2D point class, have location, distance to the origin(rho), and color value
class p2t
{
private:
    // the location value is saved as float, that is the standardized value from -1 to 1
    float _x,_y,rho;
    __uint32_t _color;

public:
    // Constructor
    p2t(float x, float y, __uint32_t color);

    //Copy constructor
    p2t(const p2t& other);

    //Setter
    void set(float x, float y, __uint32_t color);

    //Getter
    float get_x() const;
    float get_y() const;
    __uint32_t get_color() const;
    float get_r() const;

    //Overwrite the << operator to print the point location
    friend ostream& operator<<(ostream& os, const p2t& dt);

    //Rotate the point with respect to angle and center and return as a new point
    p2t rotate(float angle, p2t center);

    //Destructor
    ~p2t();
};

// 2D polygon class, have the location of each corner and color, can be plotted and shrinked
class polygon
{
private:
    //The corner locations are stored in vector for dynamic allocating
    vector<p2t> corner;
    __uint32_t _color;

public:
    //Constructor, input should be vector also
    polygon(vector<p2t> plg, __uint32_t color);

    //Copy constructor
    polygon(const polygon& plg);

    //Destructor
    ~polygon();

    //Setter
    void set(const vector<p2t> plg, __uint32_t color);

    //Getter
    vector<p2t> get()const;

    //Shrink the polygon with specified rate
    void shrink(float rate);

    //Plot the polygon
    void plot();
};


#endif // !H_2DUTIL

