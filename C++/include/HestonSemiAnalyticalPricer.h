#ifndef HESTONSEMIANALYTICALPRICER_H
#define HESTONSEMIANALYTICALPRICER_H

#include <complex>

#include "HestonParams.h"
#include "EuropeanOption.h"

/*
    HestonSemiAnalyticalPricer :
    calcule un prix de référence semi-analytique
    pour une option européenne call sous Heston.

    Idée :
    - on utilise la fonction caractéristique
    - puis une intégration numérique des probabilités P1 et P2
*/

class HestonSemiAnalyticalPricer {
private:
    HestonParams params;

    std::complex<double> characteristicFunction(std::complex<double> u,
                                                double maturity) const;

    double integrandP1(double phi,
                       double strike,
                       double maturity) const;

    double integrandP2(double phi,
                       double strike,
                       double maturity) const;

    double integrate(double strike,
                     double maturity,
                     bool forP1) const;

public:
    explicit HestonSemiAnalyticalPricer(const HestonParams& params);

    double callPrice(const EuropeanOption& option) const;
};

#endif
