#include "HestonProcess.h"

#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>

HestonProcess::HestonProcess(const HestonParams& params)
    : params(params), cirProcess(params) {
    std::cout << "[HestonProcess] Construction du processus de Heston..." << std::endl;

    if (params.spot <= 0.0) {
        throw std::runtime_error("[HestonProcess] Erreur : spot <= 0.");
    }

    if (params.rate < 0.0) {
        std::cout << "[HestonProcess] Alerte : taux negatif, cas accepte." << std::endl;
    }

    std::cout << "[HestonProcess] Parametres OK." << std::endl;
}

std::pair<double, double> HestonProcess::nextStepEuler(double currentSpot,
                                                       double currentVariance,
                                                       double dt,
                                                       double spotShock,
                                                       double varianceShock) const {
    if (currentSpot <= 0.0) {
        throw std::runtime_error("[HestonProcess] Erreur : currentSpot <= 0.");
    }

    if (currentVariance < 0.0) {
        throw std::runtime_error("[HestonProcess] Erreur : currentVariance < 0.");
    }

    if (dt <= 0.0) {
        throw std::runtime_error("[HestonProcess] Erreur : dt <= 0.");
    }

    // 1) Mise à jour de la variance via le bloc CIR
    double nextVariance = cirProcess.nextVarianceEuler(currentVariance, dt, varianceShock);

    // 2) Mise à jour du spot
    //    On utilise ici la variance tronquée courante pour le pas d'Euler
    double vPos = std::max(currentVariance, 0.0);

    double drift = params.rate * currentSpot * dt;
    double diffusion = std::sqrt(vPos) * currentSpot * std::sqrt(dt) * spotShock;

    double nextSpot = currentSpot + drift + diffusion;

    if (nextSpot <= 0.0) {
        std::cout << "[HestonProcess] Alerte : spot negatif apres Euler, troncature a un petit epsilon." << std::endl;
        nextSpot = 1e-12;
    }

    return {nextSpot, nextVariance};
}
