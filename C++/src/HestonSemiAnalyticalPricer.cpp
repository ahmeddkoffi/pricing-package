#include "HestonSemiAnalyticalPricer.h"

#include <cmath>
#include <complex>
#include <stdexcept>
#include <iostream>

/*
    Petit alias pour alléger l'écriture
*/
using Complex = std::complex<double>;

/*
    Constructeur :
    on stocke simplement les paramètres.
*/
HestonSemiAnalyticalPricer::HestonSemiAnalyticalPricer(const HestonParams& params)
    : params(params) {
    std::cout << "[HestonSemiAnalyticalPricer] Construction du pricer semi-analytique..." << std::endl;

    if (params.spot <= 0.0) {
        throw std::runtime_error("[HestonSemiAnalyticalPricer] Erreur : spot <= 0.");
    }

    if (params.kappa <= 0.0) {
        throw std::runtime_error("[HestonSemiAnalyticalPricer] Erreur : kappa <= 0.");
    }

    if (params.theta <= 0.0) {
        throw std::runtime_error("[HestonSemiAnalyticalPricer] Erreur : theta <= 0.");
    }

    if (params.xi <= 0.0) {
        throw std::runtime_error("[HestonSemiAnalyticalPricer] Erreur : xi <= 0.");
    }

    if (params.v0 < 0.0) {
        throw std::runtime_error("[HestonSemiAnalyticalPricer] Erreur : v0 < 0.");
    }

    if (params.rho < -1.0 || params.rho > 1.0) {
        throw std::runtime_error("[HestonSemiAnalyticalPricer] Erreur : rho hors de [-1,1].");
    }

    std::cout << "[HestonSemiAnalyticalPricer] Parametres OK." << std::endl;
}

/*
    Fonction caractéristique de log(S_T)
    sous le modèle de Heston.

    Convention standard :
    phi(u) = E[ exp(i u log(S_T)) ]
*/
Complex HestonSemiAnalyticalPricer::characteristicFunction(Complex u,
                                                           double maturity) const {
    Complex i(0.0, 1.0);

    double S0 = params.spot;
    double r = params.rate;
    double kappa = params.kappa;
    double theta = params.theta;
    double xi = params.xi;
    double rho = params.rho;
    double v0 = params.v0;

    Complex d = std::sqrt(
        std::pow(rho * xi * i * u - kappa, 2.0)
        + xi * xi * (i * u + u * u)
    );

    Complex g = (kappa - rho * xi * i * u - d)
              / (kappa - rho * xi * i * u + d);

    Complex one(1.0, 0.0);

    Complex C = i * u * (std::log(S0) + r * maturity)
              + (kappa * theta / (xi * xi))
                * ((kappa - rho * xi * i * u - d) * maturity
                   - 2.0 * std::log((one - g * std::exp(-d * maturity)) / (one - g)));

    Complex D = ((kappa - rho * xi * i * u - d) / (xi * xi))
              * ((one - std::exp(-d * maturity)) / (one - g * std::exp(-d * maturity)));

    return std::exp(C + D * v0);
}

/*
    Intégrande pour P1
*/
double HestonSemiAnalyticalPricer::integrandP1(double phi,
                                               double strike,
                                               double maturity) const {
    Complex i(0.0, 1.0);
    Complex u(phi, 0.0);

    Complex numerator =
        std::exp(-i * u * std::log(strike))
        * characteristicFunction(u - i, maturity);

    Complex denominator =
        i * u * params.spot * std::exp(params.rate * maturity);

    Complex ratio = numerator / denominator;

    return ratio.real();
}

/*
    Intégrande pour P2
*/
double HestonSemiAnalyticalPricer::integrandP2(double phi,
                                               double strike,
                                               double maturity) const {
    Complex i(0.0, 1.0);
    Complex u(phi, 0.0);

    Complex numerator =
        std::exp(-i * u * std::log(strike))
        * characteristicFunction(u, maturity);

    Complex denominator = i * u;

    Complex ratio = numerator / denominator;

    return ratio.real();
}

/*
    Intégration numérique simple par trapèzes.
    Suffisante pour un premier projet/test.
*/
double HestonSemiAnalyticalPricer::integrate(double strike,
                                             double maturity,
                                             bool forP1) const {
    double phiMin = 1e-6;
    double phiMax = 100.0;
    int nIntervals = 10000;

    double h = (phiMax - phiMin) / static_cast<double>(nIntervals);

    double sum = 0.0;

    for (int k = 0; k <= nIntervals; ++k) {
        double phi = phiMin + k * h;

        double value = forP1
                     ? integrandP1(phi, strike, maturity)
                     : integrandP2(phi, strike, maturity);

        if (k == 0 || k == nIntervals) {
            sum += 0.5 * value;
        } else {
            sum += value;
        }
    }

    return h * sum;
}

/*
    Prix du call européen :
    C = S0 P1 - K e^{-rT} P2
*/
double HestonSemiAnalyticalPricer::callPrice(const EuropeanOption& option) const {
    if (!option.getIsCall()) {
        throw std::runtime_error("[HestonSemiAnalyticalPricer] Erreur : seul le call est traite pour l'instant.");
    }

    double K = option.getStrike();
    double T = option.getMaturity();

    std::cout << "[HestonSemiAnalyticalPricer] Debut calcul semi-analytique..." << std::endl;
    std::cout << "[HestonSemiAnalyticalPricer] K = " << K << ", T = " << T << std::endl;

    double P1 = 0.5 + integrate(K, T, true) / M_PI;
    double P2 = 0.5 + integrate(K, T, false) / M_PI;

    double price = params.spot * P1 - K * std::exp(-params.rate * T) * P2;

    std::cout << "[HestonSemiAnalyticalPricer] P1 = " << P1
              << ", P2 = " << P2
              << ", prix = " << price << std::endl;

    return price;
}
