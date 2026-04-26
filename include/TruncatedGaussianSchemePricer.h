#ifndef TRUNCATEDGAUSSIANSCHEMEPRICER_H
#define TRUNCATEDGAUSSIANSCHEMEPRICER_H

#include "HestonParams.h"
#include "EuropeanOption.h"
#include "MonteCarloResult.h"
#include "CorrelatedRng.h"

/*
    TruncatedGaussianSchemePricer :
    pricing Monte Carlo d'une option européenne sous Heston
    avec schéma Truncated Gaussian pour la variance.
*/

class TruncatedGaussianSchemePricer {
private:
    HestonParams params;
    unsigned int seed;

    double nextVarianceTG(double currentVariance,
                          double dt,
                          double gaussianShock) const;

public:
    TruncatedGaussianSchemePricer(const HestonParams& params, unsigned int seed = 42u);

    MonteCarloResult price(const EuropeanOption& option,
                           int nPaths,
                           int nSteps) const;
};

#endif
