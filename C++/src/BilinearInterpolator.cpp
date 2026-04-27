#include "BilinearInterpolator.h"

#include <stdexcept>

std::size_t BilinearInterpolator::findLowerIndex(const std::vector<double>& values, double x) const {
    if (values.size() < 2) {
        throw std::runtime_error("[BilinearInterpolator] Erreur : pas assez de points pour interpoler.");
    }

    if (x < values.front() || x > values.back()) {
        throw std::runtime_error("[BilinearInterpolator] Erreur : valeur hors domaine de la grille.");
    }

    for (std::size_t i = 0; i < values.size() - 1; ++i) {
        if (values[i] <= x && x <= values[i + 1]) {
            return i;
        }
    }

    throw std::runtime_error("[BilinearInterpolator] Erreur : intervalle introuvable.");
}

double BilinearInterpolator::interpolate(const ImpliedVolGrid& grid, double maturity, double strike) const {
    const std::vector<double>& maturities = grid.getMaturities();
    const std::vector<double>& strikes = grid.getStrikes();

    std::size_t i = findLowerIndex(maturities, maturity);
    std::size_t j = findLowerIndex(strikes, strike);

    double T1 = maturities[i];
    double T2 = maturities[i + 1];
    double K1 = strikes[j];
    double K2 = strikes[j + 1];

    double v11 = grid.getVol(i, j);
    double v12 = grid.getVol(i, j + 1);
    double v21 = grid.getVol(i + 1, j);
    double v22 = grid.getVol(i + 1, j + 1);

    if (T2 == T1 || K2 == K1) {
        throw std::runtime_error("[BilinearInterpolator] Erreur : division par zero dans l'interpolation.");
    }

    double wt = (maturity - T1) / (T2 - T1);
    double wk = (strike - K1) / (K2 - K1);

    return (1.0 - wt) * (1.0 - wk) * v11
         + (1.0 - wt) * wk         * v12
         + wt         * (1.0 - wk) * v21
         + wt         * wk         * v22;
}