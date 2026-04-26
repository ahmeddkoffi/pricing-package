#include "ArbitrageChecker.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>

ArbitrageChecker::ArbitrageChecker(const ImpliedVolSurface& impliedSurface,
                                   double spot,
                                   double rate,
                                   double tolerance)
    : impliedSurface(impliedSurface),
      spot(spot),
      rate(rate),
      tolerance(tolerance) {}

double ArbitrageChecker::callPriceFromSurface(double maturity, double strike) const {
    double vol = impliedSurface.getVol(maturity, strike);
    return bsPricer.callPrice(spot, rate, maturity, strike, vol);
}

void ArbitrageChecker::checkStrikeMonotonicity() const {
    const std::vector<double>& maturities = impliedSurface.getGrid().getMaturities();
    const std::vector<double>& strikes = impliedSurface.getGrid().getStrikes();

    for (double T : maturities) {
        for (std::size_t j = 0; j + 1 < strikes.size(); ++j) {
            double K1 = strikes[j];
            double K2 = strikes[j + 1];

            double C1 = callPriceFromSurface(T, K1);
            double C2 = callPriceFromSurface(T, K2);

            if (C2 - C1 > tolerance) {
                throw std::runtime_error(
                    "[ArbitrageChecker] Echec monotonie strike : "
                    "T=" + std::to_string(T) +
                    ", K1=" + std::to_string(K1) +
                    ", K2=" + std::to_string(K2) +
                    ", C(K1)=" + std::to_string(C1) +
                    ", C(K2)=" + std::to_string(C2)
                );
            }
        }
    }

    std::cout << "[OK] Monotonie en strike verifiee" << std::endl;
}

void ArbitrageChecker::checkCalendarArbitrage() const {
    const std::vector<double>& maturities = impliedSurface.getGrid().getMaturities();
    const std::vector<double>& strikes = impliedSurface.getGrid().getStrikes();

    for (double K : strikes) {
        for (std::size_t i = 0; i + 1 < maturities.size(); ++i) {
            double T1 = maturities[i];
            double T2 = maturities[i + 1];

            double C1 = callPriceFromSurface(T1, K);
            double C2 = callPriceFromSurface(T2, K);

            if (C1 - C2 > tolerance) {
                throw std::runtime_error(
                    "[ArbitrageChecker] Echec arbitrage calendaire : "
                    "K=" + std::to_string(K) +
                    ", T1=" + std::to_string(T1) +
                    ", T2=" + std::to_string(T2) +
                    ", C(T1)=" + std::to_string(C1) +
                    ", C(T2)=" + std::to_string(C2)
                );
            }
        }
    }

    std::cout << "[OK] Arbitrage calendaire verifie" << std::endl;
}

void ArbitrageChecker::checkStrikeConvexity() const {
    const std::vector<double>& maturities = impliedSurface.getGrid().getMaturities();
    const std::vector<double>& strikes = impliedSurface.getGrid().getStrikes();

    if (strikes.size() < 3) {
        throw std::runtime_error("[ArbitrageChecker] Impossible de tester la convexite : moins de 3 strikes.");
    }

    for (double T : maturities) {
        for (std::size_t j = 1; j + 1 < strikes.size(); ++j) {
            double Kminus = strikes[j - 1];
            double K0     = strikes[j];
            double Kplus  = strikes[j + 1];

            double Cminus = callPriceFromSurface(T, Kminus);
            double C0     = callPriceFromSurface(T, K0);
            double Cplus  = callPriceFromSurface(T, Kplus);

            double hLeft  = K0 - Kminus;
            double hRight = Kplus - K0;

            if (hLeft <= 0.0 || hRight <= 0.0) {
                throw std::runtime_error("[ArbitrageChecker] Grille de strikes non strictement croissante.");
            }

            double leftSlope  = (C0 - Cminus) / hLeft;
            double rightSlope = (Cplus - C0) / hRight;

            if (rightSlope < leftSlope - tolerance) {
                throw std::runtime_error(
                    "[ArbitrageChecker] Echec convexite strike : "
                    "T=" + std::to_string(T) +
                    ", K-=" + std::to_string(Kminus) +
                    ", K0=" + std::to_string(K0) +
                    ", K+=" + std::to_string(Kplus) +
                    ", leftSlope=" + std::to_string(leftSlope) +
                    ", rightSlope=" + std::to_string(rightSlope)
                );
            }
        }
    }

    std::cout << "[OK] Convexite en strike verifiee" << std::endl;
}

void ArbitrageChecker::runAllChecks() const {
    checkStrikeMonotonicity();
    checkCalendarArbitrage();
    checkStrikeConvexity();
}