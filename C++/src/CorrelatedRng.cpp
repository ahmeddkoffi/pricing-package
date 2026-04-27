#include "CorrelatedRng.h"

#include <cmath>

CorrelatedRng::CorrelatedRng(double rho, unsigned int seed)
    : normalRng(seed), rho(rho) {
    if (rho < -1.0 || rho > 1.0) {
        throw std::runtime_error("[CorrelatedRng] Erreur : rho doit etre dans [-1,1].");
    }
}

std::pair<double, double> CorrelatedRng::nextPair() const {
    double z1 = normalRng.next();
    double z2 = normalRng.next();

    double x = z1;
    double y = rho * z1 + std::sqrt(1.0 - rho * rho) * z2;

    return {x, y};
}
