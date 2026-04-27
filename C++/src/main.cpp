#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>

#include "DataLoader.h"
#include "MarketDataPoint.h"
#include "ImpliedVolGrid.h"
#include "ImpliedVolSurface.h"

bool almostEqual(double a, double b, double tol = 1e-10) {
    return std::fabs(a - b) < tol;
}

int main() {
    std::cout << "==== Debut du check de la question 1.1 ====" << std::endl;

    std::cout << "[Etape 1] Chargement du fichier CSV..." << std::endl;
    DataLoader loader("data/vol_surface_sample.csv");
    std::vector<MarketDataPoint> data = loader.load();
    std::cout << "[OK] Nombre de points charges : " << data.size() << std::endl;

    std::cout << "[Etape 2] Construction de la grille de volatilite implicite..." << std::endl;
    ImpliedVolGrid grid(data);
    std::cout << "[OK] Grille construite." << std::endl;
    grid.printSummary();

    std::cout << "[Etape 3] Construction de la surface continue..." << std::endl;
    ImpliedVolSurface surface(grid);
    std::cout << "[OK] Surface construite." << std::endl;

    std::cout << "[Etape 4] Test sur un point exact de la grille..." << std::endl;
    double vol_grid = surface.getVol(0.25, 100.0);
    std::cout << "    Valeur obtenue  : " << vol_grid << std::endl;
    std::cout << "    Valeur attendue : 0.22" << std::endl;
    assert(almostEqual(vol_grid, 0.22, 1e-12));
    std::cout << "[OK] Le point de grille est retrouve exactement." << std::endl;

    std::cout << "[Etape 5] Test interpolation en strike..." << std::endl;
    std::cout << "    Point teste     : (T=0.25, K=102.5)" << std::endl;
    double vol_interp_strike = surface.getVol(0.25, 102.5);
    std::cout << "    Valeur obtenue  : " << vol_interp_strike << std::endl;
    std::cout << "    Valeur attendue : 0.221" << std::endl;
    assert(almostEqual(vol_interp_strike, 0.221, 1e-10));
    std::cout << "[OK] Interpolation en strike valide." << std::endl;

    std::cout << "[Etape 6] Test interpolation en maturite..." << std::endl;
    std::cout << "    Point teste     : (T=0.375, K=100)" << std::endl;
    double vol_interp_maturity = surface.getVol(0.375, 100.0);
    std::cout << "    Valeur obtenue  : " << vol_interp_maturity << std::endl;
    std::cout << "    Valeur attendue : 0.22" << std::endl;
    assert(almostEqual(vol_interp_maturity, 0.22, 1e-10));
    std::cout << "[OK] Interpolation en maturite valide." << std::endl;

    std::cout << "[Etape 7] Test interpolation bilineaire complete..." << std::endl;
    std::cout << "    Point teste     : (T=0.375, K=102.5)" << std::endl;
    double vol_bilinear = surface.getVol(0.375, 102.5);
    std::cout << "    Valeur obtenue  : " << vol_bilinear << std::endl;
    std::cout << "    Valeur attendue : 0.221" << std::endl;
    assert(almostEqual(vol_bilinear, 0.221, 1e-10));
    std::cout << "[OK] Interpolation bilineaire valide." << std::endl;

    std::cout << "==== Tous les checks de la question 1.1 sont passes ====" << std::endl;

    return 0;
}
