#include "EulerSchemePricer.h"

#include <cmath>
#include <stdexcept>
#include <iostream>

/*
    Constructeur :
    - stocke les paramètres du modèle
    - stocke le seed pour rendre les simulations reproductibles
*/
EulerSchemePricer::EulerSchemePricer(const HestonParams& params, unsigned int seed)
    : params(params), seed(seed) {
    std::cout << "[EulerSchemePricer] Construction du pricer Euler..." << std::endl;
}

/*
    Pricing Monte Carlo :
    1) on simule nPaths trajectoires
    2) sur chaque trajectoire, on fait nSteps pas d'Euler
    3) on calcule le payoff terminal
    4) on actualise
    5) on calcule prix + erreur standard + IC 95%
*/
MonteCarloResult EulerSchemePricer::price(const EuropeanOption& option,
                                          int nPaths,
                                          int nSteps) const {
    if (nPaths <= 0) {
        throw std::runtime_error("[EulerSchemePricer] Erreur : nPaths <= 0.");
    }

    if (nSteps <= 0) {
        throw std::runtime_error("[EulerSchemePricer] Erreur : nSteps <= 0.");
    }

    std::cout << "[EulerSchemePricer] Debut pricing Monte Carlo..." << std::endl;
    std::cout << "[EulerSchemePricer] nPaths = " << nPaths
              << ", nSteps = " << nSteps << std::endl;

    CorrelatedRng rng(params.rho, seed);
    HestonProcess process(params);

    double maturity = option.getMaturity();
    double dt = maturity / static_cast<double>(nSteps);
    double discountFactor = std::exp(-params.rate * maturity);

    double sumDiscountedPayoffs = 0.0;
    double sumSquaredDiscountedPayoffs = 0.0;

    for (int path = 0; path < nPaths; ++path) {
        double spot = params.spot;
        double variance = params.v0;

        for (int step = 0; step < nSteps; ++step) {
            std::pair<double, double> shocks = rng.nextPair();

            std::pair<double, double> nextState =
                process.nextStepEuler(spot, variance, dt, shocks.first, shocks.second);

            spot = nextState.first;
            variance = nextState.second;
        }

        double payoff = option.payoff(spot);
        double discountedPayoff = discountFactor * payoff;

        sumDiscountedPayoffs += discountedPayoff;
        sumSquaredDiscountedPayoffs += discountedPayoff * discountedPayoff;
    }

    double mean = sumDiscountedPayoffs / static_cast<double>(nPaths);
    double secondMoment = sumSquaredDiscountedPayoffs / static_cast<double>(nPaths);
    double varianceEstimator = secondMoment - mean * mean;

    if (varianceEstimator < 0.0) {
        varianceEstimator = 0.0;
    }

    double stdError = std::sqrt(varianceEstimator / static_cast<double>(nPaths));
    double ciHalfWidth = 1.96 * stdError;

    MonteCarloResult result;
    result.price = mean;
    result.stdError = stdError;
    result.lowerCI = mean - ciHalfWidth;
    result.upperCI = mean + ciHalfWidth;

    std::cout << "[EulerSchemePricer] Pricing termine." << std::endl;
    std::cout << "[EulerSchemePricer] Prix = " << result.price
              << ", StdError = " << result.stdError << std::endl;

    return result;
}
