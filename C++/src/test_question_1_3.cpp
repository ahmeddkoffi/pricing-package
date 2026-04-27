#include <iostream>
#include <vector>

#include "DataLoader.h"
#include "MarketDataPoint.h"
#include "ImpliedVolGrid.h"
#include "ImpliedVolSurface.h"
#include "ArbitrageChecker.h"

int main() {
    std::cout << "==== Debut du check de la question 1.3 ====" << std::endl;

    std::cout << "[Etape 1] Chargement du CSV..." << std::endl;
    DataLoader loader("data/vol_surface_sample.csv");
    std::vector<MarketDataPoint> data = loader.load();
    std::cout << "[OK] Nombre de points charges : " << data.size() << std::endl;

    std::cout << "[Etape 2] Construction de la grille et de la surface implicite..." << std::endl;
    ImpliedVolGrid grid(data);
    ImpliedVolSurface impliedSurface(grid);
    std::cout << "[OK] Surface implicite prete." << std::endl;

    std::cout << "[Etape 3] Construction du checker d'arbitrage..." << std::endl;
    double spot = 100.0;
    double rate = 0.02;
    double tolerance = 1e-10;

    ArbitrageChecker checker(impliedSurface, spot, rate, tolerance);
    std::cout << "[OK] Checker pret." << std::endl;

    std::cout << "[Etape 4] Lancement des checks..." << std::endl;
    checker.runAllChecks();

    std::cout << "==== Tous les checks de la question 1.3 sont passes ====" << std::endl;
    return 0;
}
