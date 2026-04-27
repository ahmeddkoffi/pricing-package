#ifndef BROADIEKAYAPRICER_H
#define BROADIEKAYAPRICER_H

#include "HestonParams.h"
#include "EuropeanOption.h"
#include "MonteCarloResult.h"
#include "CorrelatedRng.h"
#include "NormalRng.h"

/*
    BroadieKayaPricer :
    version projet-compatible d'un pricer inspiré de Broadie-Kaya.

    Objectif immédiat :
    - avoir un schéma distinct des autres
    - préparer une version plus exacte ensuite si besoin

    Ici on encapsule :
    - simulation de la variance
    - simulation du spot en log
    - pricing Monte Carlo
*/

class BroadieKayaPricer {
private:
    HestonParams params;
    unsigned int seed;

    double nextVarianceProxy(double currentVariance,
                             double dt,
                             double gaussianShock) const;

public:
    BroadieKayaPricer(const HestonParams& params, unsigned int seed = 42u);

    MonteCarloResult price(const EuropeanOption& option,
                           int nPaths,
                           int nSteps) const;
};

#endif
