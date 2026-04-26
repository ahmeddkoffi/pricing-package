#include "BroadieKayaPricer.h"

#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>

BroadieKayaPricer::BroadieKayaPricer(const HestonParams& params, unsigned int seed)
    : params(params), seed(seed) {
    std::cout << "[BroadieKayaPricer] Construction du pricer Broadie-Kaya..." << std::endl;

    if (params.spot <= 0.0) {
        throw std::runtime_error("[BroadieKayaPricer] Erreur : spot <= 0.");
    }
    if (params.kappa <= 0.0) {
        throw std::runtime_error("[BroadieKayaPricer] Erreur : kappa <= 0.");
    }
    if (params.theta <= 0.0) {
        throw std::runtime_error("[BroadieKayaPricer] Erreur : theta <= 0.");
    }
    if (params.xi <= 0.0) {
        throw std::runtime_error("[BroadieKayaPricer] Erreur : xi <= 0.");
    }
    if (params.v0 < 0.0) {
        throw std::runtime_error("[BroadieKayaPricer] Erreur : v0 < 0.");
    }
    if (params.rho < -1.0 || params.rho > 1.0) {
        throw std::runtime_error("[BroadieKayaPricer] Erreur : rho hors de [-1,1].");
    }
}

/*
    Proxy simple pour la variance :
    on garde une dynamique positive, plus stable qu'Euler additif brut.
*/
double BroadieKayaPricer::nextVarianceProxy(double currentVariance,
                                            double dt,
                                            double gaussianShock) const {
    if (dt <= 0.0) {
        throw std::runtime_error("[BroadieKayaPricer] Erreur : dt <= 0.");
    }

    double vPos = std::max(currentVariance, 0.0);

    double meanReversion = params.theta + (vPos - params.theta) * std::exp(-params.kappa * dt);
    double volTerm = params.xi * std::sqrt(std::max(vPos, 0.0)) * std::sqrt(dt) * gaussianShock;

    double nextV = meanReversion + volTerm;
    return std::max(nextV, 0.0);
}

MonteCarloResult BroadieKayaPricer::price(const EuropeanOption& option,
                                          int nPaths,
                                          int nSteps) const {
    if (nPaths <= 0) {
        throw std::runtime_error("[BroadieKayaPricer] Erreur : nPaths <= 0.");
    }
    if (nSteps <= 0) {
        throw std::runtime_error("[BroadieKayaPricer] Erreur : nSteps <= 0.");
    }

    std::cout << "[BroadieKayaPricer] Debut pricing Monte Carlo..." << std::endl;
    std::cout << "[BroadieKayaPricer] nPaths = " << nPaths
              << ", nSteps = " << nSteps << std::endl;

    CorrelatedRng corrRng(params.rho, seed);

    double maturity = option.getMaturity();
    double dt = maturity / static_cast<double>(nSteps);
    double discountFactor = std::exp(-params.rate * maturity);

    double sumPayoff = 0.0;
    double sumPayoff2 = 0.0;

    for (int path = 0; path < nPaths; ++path) {
        double spot = params.spot;
        double variance = params.v0;

        for (int step = 0; step < nSteps; ++step) {
            auto shocks = corrRng.nextPair();
            double zS = shocks.first;
            double zV = shocks.second;

            double nextVariance = nextVarianceProxy(variance, dt, zV);

            double integratedVariance =
                std::max(0.5 * (std::max(variance, 0.0) + nextVariance) * dt, 0.0);

            double diffusionVariance = std::max((1.0 - params.rho * params.rho) * integratedVariance, 0.0);

            double logNextSpot =
                std::log(spot)
                + (params.rate - 0.5 * integratedVariance / dt) * dt
                + params.rho / params.xi * (nextVariance - variance - params.kappa * (params.theta - variance) * dt)
                + std::sqrt(diffusionVariance) * zS;

            spot = std::exp(logNextSpot);
            variance = nextVariance;
        }

        double payoff = option.payoff(spot);
        double discountedPayoff = discountFactor * payoff;

        sumPayoff += discountedPayoff;
        sumPayoff2 += discountedPayoff * discountedPayoff;
    }

    double mean = sumPayoff / static_cast<double>(nPaths);
    double secondMoment = sumPayoff2 / static_cast<double>(nPaths);
    double varianceEstimator = secondMoment - mean * mean;

    if (varianceEstimator < 0.0) {
        varianceEstimator = 0.0;
    }

    double stdError = std::sqrt(varianceEstimator / static_cast<double>(nPaths));
    double halfCI = 1.96 * stdError;

    MonteCarloResult result;
    result.price = mean;
    result.stdError = stdError;
    result.lowerCI = mean - halfCI;
    result.upperCI = mean + halfCI;

    std::cout << "[BroadieKayaPricer] Pricing termine." << std::endl;
    std::cout << "[BroadieKayaPricer] Prix = " << result.price
              << ", StdError = " << result.stdError << std::endl;

    return result;
}
