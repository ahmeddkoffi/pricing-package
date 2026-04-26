#include "ImpliedVolGrid.h"

#include <set>
#include <map>
#include <stdexcept>
#include <iostream>

ImpliedVolGrid::ImpliedVolGrid(const std::vector<MarketDataPoint>& data) {
    if (data.empty()) {
        throw std::runtime_error("[ImpliedVolGrid] Erreur : les donnees sont vides.");
    }

    std::set<double> maturitySet;
    std::set<double> strikeSet;

    for (const auto& point : data) {
        maturitySet.insert(point.maturity);
        strikeSet.insert(point.strike);
    }

    maturities.assign(maturitySet.begin(), maturitySet.end());
    strikes.assign(strikeSet.begin(), strikeSet.end());

    volGrid.resize(maturities.size(), std::vector<double>(strikes.size(), -1.0));

    std::map<double, std::size_t> maturityIndex;
    std::map<double, std::size_t> strikeIndex;

    for (std::size_t i = 0; i < maturities.size(); ++i) {
        maturityIndex[maturities[i]] = i;
    }

    for (std::size_t j = 0; j < strikes.size(); ++j) {
        strikeIndex[strikes[j]] = j;
    }

    for (const auto& point : data) {
        std::size_t i = maturityIndex[point.maturity];
        std::size_t j = strikeIndex[point.strike];

        if (volGrid[i][j] != -1.0) {
            throw std::runtime_error("[ImpliedVolGrid] Erreur : doublon detecte dans la grille.");
        }

        volGrid[i][j] = point.impliedVol;
    }

    for (std::size_t i = 0; i < maturities.size(); ++i) {
        for (std::size_t j = 0; j < strikes.size(); ++j) {
            if (volGrid[i][j] < 0.0) {
                throw std::runtime_error("[ImpliedVolGrid] Erreur : grille incomplete, une case est vide.");
            }
        }
    }
}

const std::vector<double>& ImpliedVolGrid::getMaturities() const {
    return maturities;
}

const std::vector<double>& ImpliedVolGrid::getStrikes() const {
    return strikes;
}

const std::vector<std::vector<double>>& ImpliedVolGrid::getVolGrid() const {
    return volGrid;
}

double ImpliedVolGrid::getVol(std::size_t i, std::size_t j) const {
    if (i >= maturities.size() || j >= strikes.size()) {
        throw std::runtime_error("[ImpliedVolGrid] Erreur : indices hors limites dans getVol(i, j).");
    }
    return volGrid[i][j];
}

void ImpliedVolGrid::printSummary() const {
    std::cout << "===== Resume ImpliedVolGrid =====" << std::endl;

    std::cout << "Maturites : ";
    for (double T : maturities) {
        std::cout << T << " ";
    }
    std::cout << std::endl;

    std::cout << "Strikes   : ";
    for (double K : strikes) {
        std::cout << K << " ";
    }
    std::cout << std::endl;

    std::cout << "Vol grid :" << std::endl;
    for (std::size_t i = 0; i < maturities.size(); ++i) {
        std::cout << "T = " << maturities[i] << " : ";
        for (std::size_t j = 0; j < strikes.size(); ++j) {
            std::cout << volGrid[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "=================================" << std::endl;
}