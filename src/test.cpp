#include "2dUtility.h"

int main()
{
    lcd_init();

    vector<p2t> square = {p2t(-0.5,-0.5, BLACK),p2t(0.5,-0.5, BLACK), p2t(0.5,0.5, BLACK),p2t(-0.5,0.5, BLACK)};
    polygon sq(square, GREEN);
    for (int i = 0; i < 11; i++)
    {
        sq.plot();
        sq.shrink(0.2);
    }
    return 0;
}