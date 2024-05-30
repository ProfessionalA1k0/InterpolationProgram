#include "Interpolation.h"
#include "Point.h"

double Interpolation::lagrangeInterpolation(const std::vector<Point>& points, double x, bool& error, int& operationCount) {
    double result = 0.0;
    error = false;
    operationCount = 0;
    for (size_t i = 0; i < points.size(); ++i) {
        double term = points[i].y;
        for (size_t j = 0; j < points.size(); ++j) {
            if (i != j) {
                if (points[i].x == points[j].x) {
                    error = true;
                    return 0.0;
                }
                term *= (x - points[j].x) / (points[i].x - points[j].x);
                operationCount += 3;
            }
        }
        result += term;
        operationCount += 1;
    }
    return result;
}

double Interpolation::aitkenInterpolation(const std::vector<Point>& points, double x, bool& error, int& operationCount) {
    std::vector<double> f(points.size());
    error = false;
    operationCount = 0;

    for (size_t i = 0; i < points.size(); ++i) {
        f[i] = points[i].y;
    }

    for (size_t k = 1; k < points.size(); ++k) {
        for (size_t i = 0; i < points.size() - k; ++i) {
            if (points[i].x == points[i + k].x) {
                error = true;
                return 0.0;
            }
            f[i] = ((x - points[i + k].x) * f[i] + (points[i].x - x) * f[i + 1]) / (points[i].x - points[i + k].x);
            operationCount += 6;
        }
    }

    return f[0];
}