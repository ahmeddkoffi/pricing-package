#ifndef EULERSCHEMEPRICER_H
#define EULERSCHEMEPRICER_H

#include "HestonParams.h"
#include "EuropeanOption.h"
#include "MonteCarloResult.h"
#include "CorrelatedRng.h"
#include "HestonProcess.h"

/*
    EulerSchemePricer :
    price une option européenne sous Heston
    par Monte Carlo avec schéma d'Euler.
*/

class EulerSchemePricer {
private:
    HestonParams params;
    unsigned int seed;

public:
    EulerSchemePricer(const HestonParams& params, unsigned int seed = 42u);

    MonteCarloResult price(const EuropeanOption& option,
                           int nPaths,
                           int nSteps) const;
};

#endif
