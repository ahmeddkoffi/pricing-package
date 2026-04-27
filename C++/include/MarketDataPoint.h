#ifndef MARKETDATAPOINT_H
#define MARKETDATAPOINT_H

struct MarketDataPoint {
    double spot;
    double rate;
    double maturity;
    double strike;
    double forward;
    double logMoneyness;
    double impliedVol;
    double totalVariance;
};

#endif
