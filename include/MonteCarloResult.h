#ifndef MONTECARLORESULT_H
#define MONTECARLORESULT_H

struct MonteCarloResult {
    double price;
    double stdError;
    double lowerCI;
    double upperCI;
};

#endif
