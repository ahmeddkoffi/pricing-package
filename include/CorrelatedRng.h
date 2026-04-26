#ifndef CORRELATEDRNG_H
#define CORRELATEDRNG_H

#include <utility>
#include <stdexcept>

#include "NormalRng.h"

class CorrelatedRng {
private:
    NormalRng normalRng;
    double rho;

public:
    CorrelatedRng(double rho, unsigned int seed = 42u);

    std::pair<double, double> nextPair() const;
};

#endif
