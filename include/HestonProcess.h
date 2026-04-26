#ifndef HESTONPROCESS_H
#define HESTONPROCESS_H

#include <utility>

#include "HestonParams.h"
#include "CIRProcess.h"

/*
    HestonProcess :
    gère l'évolution jointe de :
    - S_t : prix du sous-jacent
    - v_t : variance instantanée

    Ici on construit une brique simple de simulation par Euler :
    - la variance est mise à jour via CIRProcess
    - puis on met à jour le spot avec la variance courante
*/

class HestonProcess {
private:
    HestonParams params;
    CIRProcess cirProcess;

public:
    explicit HestonProcess(const HestonParams& params);

    /*
        Renvoie le couple :
        (spot suivant, variance suivante)
    */
    std::pair<double, double> nextStepEuler(double currentSpot,
                                            double currentVariance,
                                            double dt,
                                            double spotShock,
                                            double varianceShock) const;
};

#endif
