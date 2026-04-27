#include "ImpliedVolSurface.h"

#include <cmath>

ImpliedVolSurface::ImpliedVolSurface(const ImpliedVolGrid& grid) : grid(grid) {}

double ImpliedVolSurface::getVol(double maturity, double strike) const {
    const std::vector<double>& maturities = grid.getMaturities();
    const std::vector<double>& strikes = grid.getStrikes();

    for (std::size_t i = 0; i < maturities.size(); ++i) {
        if (std::fabs(maturity - maturities[i]) < 1e-12) {
            for (std::size_t j = 0; j < strikes.size(); ++j) {
                if (std::fabs(strike - strikes[j]) < 1e-12) {
                    return grid.getVol(i, j);
                }
            }
        }
    }

    return interpolator.interpolate(grid, maturity, strike);
}

void ImpliedVolSurface::printSummary() const {
    grid.printSummary();
}

const ImpliedVolGrid& ImpliedVolSurface::getGrid() const {
    return grid;
}