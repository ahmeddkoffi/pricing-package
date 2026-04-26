#include "TruncatedGaussianSchemePricer.h"

#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>

TruncatedGaussianSchemePricer::TruncatedGaussianSchemePricer(const HestonParams& params,
                                                             unsigned int seed)
    : params(params), seed(seed) {
    std::cout << "[TruncatedGaussianSchemePricer] Construction du pricer TG..." << std::endl;

    if (params.spot <= 0.0) {
        throw std::runtime_error("[TruncatedGaussianSchemePricer] Erreur : spot <= 0.");
    }
    if (params.kappa <= 0.0) {
        throw std::runtime_error("[TruncatedGaussianSchemePricer] Erreur : kappa <= 0.");
    }
    if (params.theta <= 0.0) {
        throw std::runtime_error("[TruncatedGaussianSchemePricer] Erreur : theta <= 0.");
    }
    if (params.xi <= 0.0) {
        throw std::runtime_error("[TruncatedGaussianSchemePricer] Erreur : xi <= 0.");
    }
    if (params.v0 < 0.0) {
        throw std::runtime_error("[TruncatedGaussianSchemePricer] Erreur : v0 < 0.");
    }
}

double TruncatedGaussianSchemePricer::nextVarianceTG(double currentVariance,
                                                     double dt,
                                                     double gaussianShock) const {
    if (dt <= 0.0) {
        throw std::runtime_error("[TruncatedGaussianSchemePricer] Erreur : dt <= 0.");
    }

    double vPos = std::max(currentVariance, 0.0);

    double sqrtV = std::sqrt(vPos);

    double driftPart =
        sqrtV + 0.5 * params.kappa * (params.theta - vPos) / std::max(sqrtV, 1e-12) * dt;

    double diffusionPart =
        0.5 * params.xi * std::sqrt(dt) * gaussianShock;

    double truncatedGaussian = std::max(driftPart + diffusionPart, 0.0);

    return truncatedGaussian * truncatedGaussian;
}

MonteCarloResult TruncatedGaussianSchemePricer::price(const EuropeanOption& option,
                                                      int nPaths,
                                                      int nSteps) const {
    if (nPaths <= 0) {
        throw std::runtime_error("[TruncatedGaussianSchemePricer] Erreur : nPaths <= 0.");
    }
    if (nSteps <= 0) {
        throw std::runtime_error("[TruncatedGaussianSchemePricer] Erreur : nSteps <= 0.");
    }

    std::cout << "[TruncatedGaussianSchemePricer] Debut pricing Monte Carlo..." << std::endl;
    std::cout << "[TruncatedGaussianSchemePricer] nPaths = " << nPaths
              << ", nSteps = " << nSteps << std::endl;

    CorrelatedRng rng(params.rho, seed);

    double maturity = option.getMaturity();
    double dt = maturity / static_cast<double>(nSteps);
    double discountFactor = std::exp(-params.rate * maturity);

    double sumPayoff = 0.0;
    double sumPayoff2 = 0.0;

    for (int path = 0; path < nPaths; ++path) {
        double spot = params.spot;
        double variance = params.v0;

        for (int step = 0; step < nSteps; ++step) {
            auto shocks = rng.nextPair();
            double zS = shocks.first;
            double zV = shocks.second;

            double vPos = std::max(variance, 0.0);

            double nextVariance = nextVarianceTG(variance, dt, zV);

            double nextSpot = spot + params.rate * spot * dt
                            + std::sqrt(vPos) * spot * std::sqrt(dt) * zS;

            if (nextSpot <= 0.0) {
                nextSpot = 1e-12;
            }

            spot = nextSpot;
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

    std::cout << "[TruncatedGaussianSchemePricer] Pricing termine." << std::endl;
    std::cout << "[TruncatedGaussianSchemePricer] Prix = " << result.price
              << ", StdError = " << result.stdError << std::endl;

    return result;
}
