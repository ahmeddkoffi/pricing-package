#include "QuadraticExponentialSchemePricer.h"

#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <random>

QuadraticExponentialSchemePricer::QuadraticExponentialSchemePricer(const HestonParams& params,
                                                                   unsigned int seed,
                                                                   double psiC)
    : params(params), seed(seed), psiC(psiC) {
    std::cout << "[QuadraticExponentialSchemePricer] Construction du pricer QE..." << std::endl;

    if (params.spot <= 0.0) {
        throw std::runtime_error("[QuadraticExponentialSchemePricer] Erreur : spot <= 0.");
    }
    if (params.kappa <= 0.0) {
        throw std::runtime_error("[QuadraticExponentialSchemePricer] Erreur : kappa <= 0.");
    }
    if (params.theta <= 0.0) {
        throw std::runtime_error("[QuadraticExponentialSchemePricer] Erreur : theta <= 0.");
    }
    if (params.xi <= 0.0) {
        throw std::runtime_error("[QuadraticExponentialSchemePricer] Erreur : xi <= 0.");
    }
    if (params.v0 < 0.0) {
        throw std::runtime_error("[QuadraticExponentialSchemePricer] Erreur : v0 < 0.");
    }
    if (params.rho < -1.0 || params.rho > 1.0) {
        throw std::runtime_error("[QuadraticExponentialSchemePricer] Erreur : rho hors de [-1,1].");
    }
    if (psiC <= 0.0) {
        throw std::runtime_error("[QuadraticExponentialSchemePricer] Erreur : psiC <= 0.");
    }
}

double QuadraticExponentialSchemePricer::nextVarianceQE(double currentVariance,
                                                        double dt,
                                                        double gaussianShock,
                                                        double uniformShock) const {
    if (dt <= 0.0) {
        throw std::runtime_error("[QuadraticExponentialSchemePricer] Erreur : dt <= 0.");
    }

    double kappa = params.kappa;
    double theta = params.theta;
    double xi = params.xi;

    double expkdt = std::exp(-kappa * dt);

    // Moments conditionnels exacts du CIR
    double m = theta + (currentVariance - theta) * expkdt;

    double s2 =
        currentVariance * xi * xi * expkdt * (1.0 - expkdt) / kappa
        + theta * xi * xi * std::pow(1.0 - expkdt, 2.0) / (2.0 * kappa);

    if (m <= 0.0) {
        return 0.0;
    }

    double psi = s2 / (m * m);

    // Régime 1 : approximation quadratique
    if (psi <= psiC) {
        double b2 = 2.0 / psi - 1.0 + std::sqrt(2.0 / psi) * std::sqrt(2.0 / psi - 1.0);
        double b = std::sqrt(std::max(b2, 0.0));
        double a = m / (1.0 + b * b);

        double nextV = a * std::pow(b + gaussianShock, 2.0);
        return std::max(nextV, 0.0);
    }

    // Régime 2 : mélange avec masse en 0 + exponentielle
    double p = (psi - 1.0) / (psi + 1.0);
    double beta = (1.0 - p) / m;

    if (uniformShock <= p) {
        return 0.0;
    }

    double nextV = -std::log((1.0 - p) / (1.0 - uniformShock)) / beta;
    return std::max(nextV, 0.0);
}

MonteCarloResult QuadraticExponentialSchemePricer::price(const EuropeanOption& option,
                                                         int nPaths,
                                                         int nSteps) const {
    if (nPaths <= 0) {
        throw std::runtime_error("[QuadraticExponentialSchemePricer] Erreur : nPaths <= 0.");
    }
    if (nSteps <= 0) {
        throw std::runtime_error("[QuadraticExponentialSchemePricer] Erreur : nSteps <= 0.");
    }

    std::cout << "[QuadraticExponentialSchemePricer] Debut pricing Monte Carlo..." << std::endl;
    std::cout << "[QuadraticExponentialSchemePricer] nPaths = " << nPaths
              << ", nSteps = " << nSteps << std::endl;

    CorrelatedRng corrRng(params.rho, seed);
    std::mt19937 gen(seed + 12345u);
    std::uniform_real_distribution<double> uniformDist(0.0, 1.0);

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
            double u = uniformDist(gen);

            double nextVariance = nextVarianceQE(variance, dt, zV, u);

            // Variance moyenne sur le pas
            double vBar = std::max(0.5 * (std::max(variance, 0.0) + nextVariance), 0.0);

            // Schéma log-Euler pour garantir un spot positif
            double nextSpot =
                spot * std::exp(
                    (params.rate - 0.5 * vBar) * dt
                    + std::sqrt(vBar * dt) * zS
                );

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

    std::cout << "[QuadraticExponentialSchemePricer] Pricing termine." << std::endl;
    std::cout << "[QuadraticExponentialSchemePricer] Prix = " << result.price
              << ", StdError = " << result.stdError << std::endl;

    return result;
}

