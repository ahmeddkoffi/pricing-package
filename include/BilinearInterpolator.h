#ifndef BILINEARINTERPOLATOR_H
#define BILINEARINTERPOLATOR_H

#include "ImpliedVolGrid.h"

class BilinearInterpolator {
public:
    double interpolate(const ImpliedVolGrid& grid, double maturity, double strike) const;

private:
    std::size_t findLowerIndex(const std::vector<double>& values, double x) const;
};

#endif
