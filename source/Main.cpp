#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <chrono>
#include<sstream>  
// #include <iomanip> //<== setprecision()
using namespace std::chrono;

// Represents single point in 2D space
class point {
public:
    double x, y;
    int index;
    bool operator > (point p) {    //Lowest Y
        if (y != p.y) { return y < p.y; }
        return x < p.x;
    };
};

// Load points from file
std::vector<point> load(std::string filePath) {
    std::vector<point> output;
    point* points;
    std::ifstream plik;
    plik.open(filePath);
    int pointsCount = 0;
    if (plik.is_open()) {
        plik >> pointsCount;
        std::cout << "Ilosc punktow w danym pliku: " << pointsCount << std::endl;
        points = new point[pointsCount];
        for (int i = 0; i < pointsCount; i++) {
            plik >> points[i].x >> points[i].y;
            points[i].index = i;
        };
        plik.close();
        for (int i = 0; i < pointsCount; i++) {
            output.push_back(points[i]);
        };
    };
    return output;
};

// 3 points turn
int counterClockWise(point p1, point p2, point p3) {
    double ar = (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
    if (ar < 0) return 1; //Clockwise
    if (ar > 0) return -1; //CounterClockWise
    return 0; //Collinear
};

// sqr dist
double furtherAway(point p1, point p2) {
    return (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
};

// Sort points
void sortPoints(std::vector<point>& v1) {
    // Find lowestLeft
    double lowestLeft = v1[0].y;
    int min = 0;
    for (int i = 1; i < v1.size(); i++) {
        double y = v1[i].y;
        if ((y < lowestLeft) || (lowestLeft == y && v1[i].x < v1[min].x)) {
            lowestLeft = v1[i].y;
            min = i;
        };
    };
    std::swap(v1[0], v1[min]);
    point p0 = v1[0];
    //cout << "\nNajnizszy lewy dolny: " << endl;
    //cout << "(" << p0.x << ", " << p0.y << ")" << endl;
    auto compare = [p0](point& p1_2, point& p2_2) {
        int returned = counterClockWise(p0, p1_2, p2_2);
        if (returned == 0) { return  furtherAway(p0, p1_2) < furtherAway(p0, p2_2); }
        else { return returned == -1; };
    };
    sort(v1.begin() + 1, v1.end(), compare);
};

std::vector<point> grahamScan(std::vector<point> v1) {
    std::vector<point> points;
    int rozmiarV1 = v1.size();
    if (rozmiarV1 <= 2) { // can't make convex hull with <= 2 points
        return v1;
    };
    points.push_back(v1[0]);
    points.push_back(v1[1]);
    points.push_back(v1[2]);
    for (int i = points.size(); i < v1.size(); i++) {
        auto ps = points.size();
        while (points.size() > 1 && counterClockWise(points[ps - 2], points[ps - 1], v1[i]) != -1) {
            points.pop_back();
            ps = points.size();
        };
        points.push_back(v1[i]);
    };
    return points;
};

// Display results
void display(std::vector<point> v1) {
    //std::cout << "Powloka wypukla: " << std::endl;
    int j = 0;
    for (std::vector<point>::iterator::value_type const i : v1) {
        std::cout << "[" << i.index << "] (" << i.x << ", " << i.y << ")" << std::endl;
        j++;
    }
    std::cout << "Ilosc punktow skladajacych sie na powloke: " << j << std::endl;
};

int main() {
    std::vector<point> points;
    for (int i = 1; i <= 6; i++) {
        if (i != 6) {
            points = load("../example-point-sets/points" + std::to_string(i) + ".txt");
        }
        else {
            points = load("../example-point-sets/pointsTest.txt");
        };
        auto startSort = steady_clock::now();
		sortPoints(points);
        auto endSort = steady_clock::now();
        auto startMainLoop = steady_clock::now();
        std::vector<point> convexHull = grahamScan(points);
        auto endMainLoop = steady_clock::now();
        std::cout << "Powloka wypukla\n[index] (wspolrzedna.x, wspolrzedna.y)" << std::endl;
		display(convexHull);
        std::cout << "\nCzas sortowania wynosi: " << duration_cast<microseconds>(endSort - startSort).count() << "us" << std::endl;
        std::cout << "Czas glownej petli wynosi: " << duration_cast<microseconds>(endMainLoop - startMainLoop).count() << "us" << std::endl;
        std::cout << "Czas laczny wynosi: ~" << duration_cast<microseconds>((endSort - startSort) + (endMainLoop - startMainLoop)).count() << "us" << std::endl;
        std::cout << "\n=================================================" << std::endl << std::endl;
    }
    //system("pause");
    return 0;
}