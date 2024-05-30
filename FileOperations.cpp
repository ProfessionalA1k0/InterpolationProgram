#include "FileOperations.h"
#include "Point.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cmath>

void FileOperations::generateGnuplotScript(const std::string& filename, const std::vector<Point>& interpolationPoints, const std::vector<Point>& interpolated_graf, const std::vector<Point>& input_points, double xmin, double xmax, double ymin, double ymax) {
    std::ofstream scriptFile(filename);
    if (scriptFile.is_open()) {
        scriptFile << "set title \"Interpolation Result\"\n";
        scriptFile << "set xlabel \"x\"\n";
        scriptFile << "set ylabel \"y\"\n";
        scriptFile << "set xrange [" << xmin << ":" << xmax << "]\n";
        scriptFile << "set yrange [" << ymin << ":" << ymax << "]\n";
        scriptFile << "set grid\n";
        scriptFile << "set zeroaxis\n";
        scriptFile << "set xtics 10\n";
        scriptFile << "set ytics 10\n";
        scriptFile << "plot '-' title 'Input Points' with points pointtype 7 pointsize 0.7, ";
        scriptFile << "'-' title 'Interpolated Points' with points pointtype 7 pointsize 0.7, ";
        scriptFile << "'-' title 'Polynomial' with lines linecolor rgb 'red'\n";

        for (const auto& point : input_points) {
            scriptFile << point.x << " " << point.y << "\n";
        }
        scriptFile << "e\n";

        for (const auto& point : interpolationPoints) {
            scriptFile << point.x << " " << point.y << "\n";
        }
        scriptFile << "e\n";

        for (const auto& point : interpolated_graf) {
            scriptFile << point.x << " " << point.y << "\n";
        }
        scriptFile << "e\n";

        scriptFile.close();
        std::cout << "Gnuplot script has been successfully generated to the file '" << filename << "'." << std::endl;
    } else {
        std::cout << "Unable to open the file '" << filename << "' for writing." << std::endl;
    }
}

void FileOperations::saveResultToFile(const std::string& filename, const std::vector<double>& interpolated_results, const std::vector<double>& values) {
    std::ofstream outputFile(filename);
    if (outputFile.is_open()) {
        outputFile << "All interpolation results:" << std::endl;
        for (size_t i = 0; i < interpolated_results.size(); ++i) {
            outputFile << "Interpolation result for x = " << values[i] << ": " << interpolated_results[i] << std::endl;
        }
        outputFile.close();
        std::cout << "All interpolation results have been successfully saved to the file '" << filename << "'." << std::endl;
    } else {
        std::cout << "Unable to open the file '" << filename << "' for writing." << std::endl;
    }
}

bool FileOperations::pointAlreadyExists(const std::vector<Point>& points, const Point& newPoint) {
    return (std::find_if(points.begin(), points.end(), [&](const Point& p){ return pointsMatch(p, newPoint); }) != points.end());
}

bool FileOperations::pointsMatch(const Point& p1, const Point& p2) {
    return (p1.x == p2.x && p1.y == p2.y);
}

bool FileOperations::isNumber(const std::string& str) {
    std::istringstream iss(str);
    double num;
    iss >> std::noskipws >> num;
    return iss.eof() && !iss.fail();
}

double FileOperations::roundToThreeDecimalPlaces(double value) {
    return round(value * 1000.0) / 1000.0;
}