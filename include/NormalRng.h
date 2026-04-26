#ifndef NORMALRNG_H
#define NORMALRNG_H

#include <random>

class NormalRng {
private:
    mutable std::mt19937 generator;
    mutable std::normal_distribution<double> normalDist;

public:
    explicit NormalRng(unsigned int seed = 42u);

    double next() const;
};

#endif
