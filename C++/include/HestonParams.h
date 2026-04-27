#ifndef HESTONPARAMS_H
#define HESTONPARAMS_H

struct HestonParams {
    double spot;
    double rate;
    double kappa;
    double theta;
    double xi;
    double rho;
    double v0;
};

#endif
