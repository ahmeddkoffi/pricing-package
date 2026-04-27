#ifndef QUADRATICEXPONENTIALSCHEMEPRICER_H
#define QUADRATICEXPONENTIALSCHEMEPRICER_H

#include "HestonParams.h"
#include "EuropeanOption.h"
#include "MonteCarloResult.h"
#include "CorrelatedRng.h"
#include "NormalRng.h"

/*
    QuadraticExponentialSchemePricer :
    pricing Monte Carlo sous Heston avec schéma QE (Andersen)
    pour la variance.

    Idée :
    - on approxime la loi conditionnelle de v_{t+dt} en faisant
      matcher ses deux premiers moments
    - puis on simule S_t avec la variance mise à jour
*/

class QuadraticExponentialSchemePricer {
private:
    HestonParams params;
    unsigned int seed;
    double psiC;

    double nextVarianceQE(double currentVariance,
                          double dt,
                          double gaussianShock,
                          double uniformShock) const;

public:
    QuadraticExponentialSchemePricer(const HestonParams& params,
                                     unsigned int seed = 42u,
                                     double psiC = 1.5);

    MonteCarloResult price(const EuropeanOption& option,
                           int nPaths,
                           int nSteps) const;
};

#endif
