#ifndef BLACKSCHOLESPRICER_H
#define BLACKSCHOLESPRICER_H

class BlackScholesPricer {
public:
    double normalCDF(double x) const;
    double callPrice(double spot,
                     double rate,
                     double maturity,
                     double strike,
                     double vol) const;
};

#endif
