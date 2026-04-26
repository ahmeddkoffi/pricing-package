#ifndef ARBITRAGECHECKER_H
#define ARBITRAGECHECKER_H

#include "ImpliedVolSurface.h"
#include "BlackScholesPricer.h"

class ArbitrageChecker {
private:
    ImpliedVolSurface impliedSurface;
    BlackScholesPricer bsPricer;
    double spot;
    double rate;
    double tolerance;

public:
    ArbitrageChecker(const ImpliedVolSurface& impliedSurface,
                     double spot,
                     double rate,
                     double tolerance);

    double callPriceFromSurface(double maturity, double strike) const;

    void checkStrikeMonotonicity() const;
    void checkCalendarArbitrage() const;
    void checkStrikeConvexity() const;

    void runAllChecks() const;
};

#endif
