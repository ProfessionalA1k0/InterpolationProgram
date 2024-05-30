#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <vector>
#include "Point.h"

class Interpolation {
public:
    static double lagrangeInterpolation(const std::vector<Point>& points, double x, bool& error, int& operationCount);
    static double aitkenInterpolation(const std::vector<Point>& points, double x, bool& error, int& operationCount);
};

#endif // INTERPOLATION_H