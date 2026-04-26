#ifndef IMPLIEDVOLGRID_H
#define IMPLIEDVOLGRID_H

#include <vector>
#include "MarketDataPoint.h"

class ImpliedVolGrid {
private:
    std::vector<double> maturities;
    std::vector<double> strikes;
    std::vector<std::vector<double>> volGrid;

public:
    ImpliedVolGrid(const std::vector<MarketDataPoint>& data);

    const std::vector<double>& getMaturities() const;
    const std::vector<double>& getStrikes() const;
    const std::vector<std::vector<double>>& getVolGrid() const;

    double getVol(std::size_t i, std::size_t j) const;
    void printSummary() const;
};

#endif
