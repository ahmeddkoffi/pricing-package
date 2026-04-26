#include "LocalVolSurface.h"

#include <cmath>
#include <stdexcept>
#include <algorithm>

LocalVolSurface::LocalVolSurface(const ImpliedVolSurface& impliedSurface,
                                 double spot,
                                 double rate,
                                 double hY,
                                 double hT)
    : impliedSurface(impliedSurface), spot(spot), rate(rate), hY(hY), hT(hT) {}

double LocalVolSurface::computeForward(double maturity) const {
    return spot * std::exp(rate * maturity);
}

double LocalVolSurface::yToStrike(double y, double maturity) const {
    double forward = computeForward(maturity);
    return forward * std::exp(y);
}

double LocalVolSurface::sigmaImp(double y, double maturity) const {
    double strike = yToStrike(y, maturity);
    return impliedSurface.getVol(maturity, strike);
}

double LocalVolSurface::totalVariance(double y, double maturity) const {
    double sigma = sigmaImp(y, maturity);
    return sigma * sigma * maturity;
}

double LocalVolSurface::dwdT(double y, double maturity) const {
    if (maturity - hT <= 0.0) {
        throw std::runtime_error("[LocalVolSurface] Erreur : maturity - hT <= 0 dans dwdT.");
    }

    double wPlus = totalVariance(y, maturity + hT);
    double wMinus = totalVariance(y, maturity - hT);
    return (wPlus - wMinus) / (2.0 * hT);
}

double LocalVolSurface::dwdy(double y, double maturity) const {
    double wPlus = totalVariance(y + hY, maturity);
    double wMinus = totalVariance(y - hY, maturity);
    return (wPlus - wMinus) / (2.0 * hY);
}

double LocalVolSurface::d2wdy2(double y, double maturity) const {
    double wPlus = totalVariance(y + hY, maturity);
    double w0 = totalVariance(y, maturity);
    double wMinus = totalVariance(y - hY, maturity);
    return (wPlus - 2.0 * w0 + wMinus) / (hY * hY);
}

double LocalVolSurface::localVariance(double y, double maturity) const {
    double w = totalVariance(y, maturity);
    double dT = dwdT(y, maturity);
    double dy = dwdy(y, maturity);
    double dyy = d2wdy2(y, maturity);

    if (w <= 0.0) {
        throw std::runtime_error("[LocalVolSurface] Erreur : w <= 0 dans localVariance.");
    }

    double denominator =
        1.0
        - (y / w) * dy
        + 0.5 * dyy
        + 0.25 * (-0.25 - 1.0 / w + (y * y) / (w * w)) * dy * dy;

    if (std::fabs(denominator) < 1e-14) {
        throw std::runtime_error("[LocalVolSurface] Erreur : denominateur trop proche de zero dans Dupire.");
    }

    double vLocal = dT / denominator;
    return std::max(0.0, vLocal);
}

double LocalVolSurface::localVol(double y, double maturity) const {
    return std::sqrt(localVariance(y, maturity));
}