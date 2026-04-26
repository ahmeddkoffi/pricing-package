#ifndef LOCALVOLSURFACE_H
#define LOCALVOLSURFACE_H

#include "ImpliedVolSurface.h"

class LocalVolSurface {
private:
    ImpliedVolSurface impliedSurface;
    double spot;
    double rate;
    double hY;
    double hT;

public:
    LocalVolSurface(const ImpliedVolSurface& impliedSurface,
                    double spot,
                    double rate,
                    double hY,
                    double hT);

    double computeForward(double maturity) const;
    double yToStrike(double y, double maturity) const;
    double sigmaImp(double y, double maturity) const;

    double totalVariance(double y, double maturity) const;

    double dwdT(double y, double maturity) const;
    double dwdy(double y, double maturity) const;
    double d2wdy2(double y, double maturity) const;

    double localVariance(double y, double maturity) const;
    double localVol(double y, double maturity) const;
};

#endif
