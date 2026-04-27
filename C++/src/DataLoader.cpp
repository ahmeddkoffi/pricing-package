#include "DataLoader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

DataLoader::DataLoader(const std::string& filename) : filename(filename) {}

std::vector<MarketDataPoint> DataLoader::load() const {
    std::vector<MarketDataPoint> data;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir le fichier : " + filename);
    }

    std::string line;
    std::getline(file, line); // saute l'en-tete

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> cells;

        while (std::getline(ss, cell, ',')) {
            cells.push_back(cell);
        }

        if (cells.size() < 8) {
            throw std::runtime_error("Ligne mal formee dans le CSV : " + line);
        }

        MarketDataPoint point;
        point.spot = std::stod(cells[0]);
        point.rate = std::stod(cells[1]);
        point.maturity = std::stod(cells[2]);
        point.strike = std::stod(cells[3]);
        point.forward = std::stod(cells[4]);
        point.logMoneyness = std::stod(cells[5]);
        point.impliedVol = std::stod(cells[6]);
        point.totalVariance = std::stod(cells[7]);

        data.push_back(point);
    }

    return data;
}
