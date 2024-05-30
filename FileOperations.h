#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <vector>
#include <string>

class Point;

class FileOperations {
public:
    static void generateGnuplotScript(const std::string& filename, const std::vector<Point>& interpolationPoints, const std::vector<Point>& interpolated_graf, const std::vector<Point>& input_points, double xmin, double xmax, double ymin, double ymax);
    static void saveResultToFile(const std::string& filename, const std::vector<double>& interpolated_results, const std::vector<double>& values);
    static bool pointAlreadyExists(const std::vector<Point>& points, const Point& newPoint);
    static bool pointsMatch(const Point& p1, const Point& p2);
    static bool isNumber(const std::string& str);
    static double roundToThreeDecimalPlaces(double value);
};

#endif // FILEOPERATIONS_H