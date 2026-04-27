#ifndef IMPLIEDVOLSURFACE_H
#define IMPLIEDVOLSURFACE_H

#include "ImpliedVolGrid.h"
#include "BilinearInterpolator.h"

class ImpliedVolSurface {
private:
    ImpliedVolGrid grid;
    BilinearInterpolator interpolator;

public:
    ImpliedVolSurface(const ImpliedVolGrid& grid);

    double getVol(double maturity, double strike) const;
    void printSummary() const;
    const ImpliedVolGrid& getGrid() const;
};

#endif
