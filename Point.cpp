#include "Point.h"
#include "FileOperations.h"

Point::Point(double x_val, double y_val) : x(FileOperations::roundToThreeDecimalPlaces(x_val)), y(FileOperations::roundToThreeDecimalPlaces(y_val)) {}