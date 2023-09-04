#include "Data.hpp"
#include <algorithm>
#include <limits>

void Hardblock::rotate()
{
    isRotated = (isRotated + 1) % 4;
    int temp = height;
    height = width;
    width = temp;
}

signed long long Net::HPWL()
{
    signed long long minX = std::numeric_limits<signed long long>::max(), minY = std::numeric_limits<signed long long>::max(),
        maxX = std::numeric_limits<signed long long>::min(), maxY = std::numeric_limits<signed long long>::min();
    for (auto pin : pins)
    {
        if (minX > pin->x)
            minX = pin->x;
        if (minY > pin->y)
            minY = pin->y;
        if (maxX < pin->x)
            maxX = pin->x;
        if (maxY < pin->y)
            maxY = pin->y;
    }
    return (maxX - minX) + (maxY - minY);
}
