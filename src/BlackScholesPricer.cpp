#include "BlackScholesPricer.h"

#include <cmath>
#include <stdexcept>

double BlackScholesPricer::normalCDF(double x) const {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}

double BlackScholesPricer::callPrice(double spot,
                                     double rate,
                                     double maturity,
                                     double strike,
                                     double vol) const {
    if (spot <= 0.0) {
        throw std::runtime_error("[BlackScholesPricer] Erreur : spot <= 0.");
    }

    if (strike <= 0.0) {
        throw std::runtime_error("[BlackScholesPricer] Erreur : strike <= 0.");
    }

    if (maturity <= 0.0) {
        throw std::runtime_error("[BlackScholesPricer] Erreur : maturity <= 0.");
    }

    if (vol <= 0.0) {
        throw std::runtime_error("[BlackScholesPricer] Erreur : vol <= 0.");
    }

    double sqrtT = std::sqrt(maturity);
    double d1 = (std::log(spot / strike) + (rate + 0.5 * vol * vol) * maturity)
                / (vol * sqrtT);
    double d2 = d1 - vol * sqrtT;

    return spot * normalCDF(d1)
         - strike * std::exp(-rate * maturity) * normalCDF(d2);
}