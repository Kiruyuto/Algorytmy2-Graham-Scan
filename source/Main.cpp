#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <chrono>
#include<sstream>  
#include <iomanip> //<== setprecision()
using namespace std::chrono;

// Klasa pojedynczego puinktu
class punkt {
public:
    double x, y;
    int index;
    bool operator > (punkt p) {    //Lowest Y
        if (y != p.y) { return y < p.y; }
        return x < p.x;
    };
};

// Wczytaj punkty z pliku
std::vector<punkt> wczytaj(std::string filePath) {
    std::vector<punkt> output;
    punkt* punkty;
    std::ifstream plik;
    plik.open(filePath);
    int iloscPunktow = 0;
    if (plik.is_open()) {
        plik >> iloscPunktow;
        std::cout << "Ilosc punktow w danym pliku: " << iloscPunktow << std::endl;
        punkty = new punkt[iloscPunktow];
        for (int i = 0; i < iloscPunktow; i++) {
            plik >> punkty[i].x >> punkty[i].y;
            punkty[i].index = i;
        };
        plik.close();
        for (int i = 0; i < iloscPunktow; i++) {
            output.push_back(punkty[i]);
        };
    };
    return output;
};

// Skret 3 punktow
int CounterClockWise(punkt p1, punkt p2, punkt p3) {
    double ar = (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
    if (ar < 0) return 1; //Clockwise
    if (ar > 0) return -1; //CounterClockWise
    return 0; //Collinear
};

// sqr dist
double furtherAway(punkt p1, punkt p2) {
    return (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
};

// Sortuj punkty
void sortPoints(std::vector<punkt>& v1) {
    //Wyszukuje lowestLeft
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
    punkt p0 = v1[0];
    //cout << "\nNajnizszy lewy dolny: " << endl;
    //cout << "(" << p0.x << ", " << p0.y << ")" << endl;
    auto compare = [p0](punkt& p1_2, punkt& p2_2) {
        int returned = CounterClockWise(p0, p1_2, p2_2);
        if (returned == 0) { return  furtherAway(p0, p1_2) < furtherAway(p0, p2_2); }
        else { return returned == -1; };
    };
    sort(v1.begin() + 1, v1.end(), compare);
};

std::vector<punkt> grahamScan(std::vector<punkt> v1) {
    std::vector<punkt> punkty;
    int rozmiarV1 = v1.size();
    //Case dla malych tablic
    if (rozmiarV1 <= 2) { // <=2 punkty = nie mozna utworzyc powloki
        return v1;
    };
    punkty.push_back(v1[0]);
    punkty.push_back(v1[1]);
    punkty.push_back(v1[2]);
    for (int i = punkty.size(); i < v1.size(); i++) {
        auto ps = punkty.size();
        while (punkty.size() > 1 && CounterClockWise(punkty[ps - 2], punkty[ps - 1], v1[i]) != -1) {
            punkty.pop_back();
            ps = punkty.size();
        };
        punkty.push_back(v1[i]);
    };
    return punkty;
};

// Wyswietl rezultaty
void display(std::vector<punkt> v1) {
    //std::cout << "Powloka wypukla: " << std::endl;
    int j = 0;
    for (std::vector<punkt>::iterator::value_type const i : v1) {
        std::cout << "[" << i.index << "] (" << i.x << ", " << i.y << ")" << std::endl;
        j++;
    }
    std::cout << "Ilosc punktow skladajacych sie na powloke: " << j << std::endl;
};

int main() {
    std::vector<punkt> punkty;
    for (int i = 1; i <= 6; i++) {
        if (i != 6) {
            punkty = wczytaj("../example-point-sets/points" + std::to_string(i) + ".txt");
        }
        else {
            punkty = wczytaj("../example-point-sets/pointsTest.txt");
        };
        auto startSort = steady_clock::now();
		sortPoints(punkty);
        auto endSort = steady_clock::now();
        auto startMainLoop = steady_clock::now();
        std::vector<punkt> powloka = grahamScan(punkty);
        auto endMainLoop = steady_clock::now();
        std::cout << "Powloka wpukla\n[index] (wspolrzedna.x, wspolrzedna.y)" << std::endl;
		display(powloka);
        std::cout << "\nCzas sortowania wynosi: " << duration_cast<microseconds>(endSort - startSort).count() << "us" << std::endl;
        std::cout << "Czas glownej petli wynosi: " << duration_cast<microseconds>(endMainLoop - startMainLoop).count() << "us" << std::endl;
        std::cout << "Czas laczny wynosi: ~" << duration_cast<microseconds>((endSort - startSort) + (endMainLoop - startMainLoop)).count() << "us" << std::endl;
        std::cout << "\n=================================================" << std::endl << std::endl;
    }
    //system("pause");
    return 0;
}