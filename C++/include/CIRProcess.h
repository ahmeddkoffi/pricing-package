#ifndef CIRPROCESS_H
#define CIRPROCESS_H

#include "HestonParams.h"

/*
    CIRProcess :
    gère l'évolution de la variance v_t dans le modèle de Heston.

    Forme :
    dv_t = kappa * (theta - v_t) dt + xi * sqrt(v_t) dW_t

    Ici on ne code pas encore un schéma complet de simulation de trajectoire.
    On prépare seulement une brique qui calcule le prochain pas de variance
    selon un schéma d'Euler tronqué simple.
*/

class CIRProcess {
private:
    HestonParams params;

public:
    explicit CIRProcess(const HestonParams& params);

    /*
        Feller condition :
        2 * kappa * theta >= xi^2
        utile pour savoir si le processus reste strictement positif en théorie.
    */
    bool satisfiesFellerCondition() const;

    /*
        Un pas d'Euler tronqué pour la variance.
        On force la positivité via max(v, 0).
    */
    double nextVarianceEuler(double currentVariance,
                             double dt,
                             double gaussianShock) const;
};

#endif
