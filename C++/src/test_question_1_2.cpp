#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>

#include "DataLoader.h"
#include "MarketDataPoint.h"
#include "ImpliedVolGrid.h"
#include "ImpliedVolSurface.h"
#include "LocalVolSurface.h"

bool almostEqual(double a, double b, double tol = 1e-8) {
    return std::fabs(a - b) < tol;
}

double computeForward(double spot, double rate, double maturity) {
    return spot * std::exp(rate * maturity);
}

double computeLogMoneyness(double strike, double forward) {
    return std::log(strike / forward);
}

int main() {
    std::cout << "==== Check question 1.2 ====" << std::endl;

    DataLoader loader("data/vol_surface_sample.csv");
    std::vector<MarketDataPoint> data = loader.load();

    ImpliedVolGrid grid(data);
    ImpliedVolSurface impliedSurface(grid);

    double hY = 1e-3;
    double hT = 1e-3;
    double spot = 100.0;
    double rate = 0.02;

    LocalVolSurface localSurface(impliedSurface, spot, rate, hY, hT);

    double T1 = 0.25;
    double K1 = 100.0;
    double forward1 = computeForward(spot, rate, T1);
    double y1 = computeLogMoneyness(K1, forward1);

    double sigma1 = impliedSurface.getVol(T1, K1);
    double expectedW1 = sigma1 * sigma1 * T1;
    double obtainedW1 = localSurface.totalVariance(y1, T1);

    assert(almostEqual(obtainedW1, expectedW1, 1e-10));
    std::cout << "[OK] Definition de w(y,T) verifiee" << std::endl;

    double T2 = 0.375;
    double K2 = 102.5;
    double forward2 = computeForward(spot, rate, T2);
    double y2 = computeLogMoneyness(K2, forward2);

    double dT = localSurface.dwdT(y2, T2);
    double dy = localSurface.dwdy(y2, T2);
    double dyy = localSurface.d2wdy2(y2, T2);

    assert(std::isfinite(dT));
    assert(std::isfinite(dy));
    assert(std::isfinite(dyy));
    std::cout << "[OK] Derivees numeriques finies" << std::endl;

    double localVar = localSurface.localVariance(y2, T2);
    double localVol = localSurface.localVol(y2, T2);

    assert(localVar >= 0.0);
    assert(localVol >= 0.0);
    assert(std::isfinite(localVar));
    assert(std::isfinite(localVol));
    std::cout << "[OK] Positivite de la variance locale verifiee" << std::endl;

    std::vector<std::pair<double, double>> testPoints = {
        {0.25, 95.0},
        {0.25, 105.0},
        {0.50, 100.0},
        {1.00, 110.0}
    };

    for (const auto& p : testPoints) {
        double T = p.first;
        double K = p.second;
        double F = computeForward(spot, rate, T);
        double y = computeLogMoneyness(K, F);

        double w = localSurface.totalVariance(y, T);
        double v = localSurface.localVariance(y, T);
        double s = localSurface.localVol(y, T);

        assert(std::isfinite(w));
        assert(std::isfinite(v));
        assert(std::isfinite(s));
        assert(v >= 0.0);
        assert(s >= 0.0);
    }

    std::cout << "[OK] Stabilite multi-points verifiee" << std::endl;
    std::cout << "[OK] Question 1.2 validee" << std::endl;

    return 0;
}