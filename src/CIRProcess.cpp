#include "CIRProcess.h"

#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>

CIRProcess::CIRProcess(const HestonParams& params)
    : params(params) {
    std::cout << "[CIRProcess] Construction du processus CIR..." << std::endl;

    if (params.kappa <= 0.0) {
        throw std::runtime_error("[CIRProcess] Erreur : kappa <= 0.");
    }

    if (params.theta <= 0.0) {
        throw std::runtime_error("[CIRProcess] Erreur : theta <= 0.");
    }

    if (params.xi <= 0.0) {
        throw std::runtime_error("[CIRProcess] Erreur : xi <= 0.");
    }

    if (params.v0 < 0.0) {
        throw std::runtime_error("[CIRProcess] Erreur : v0 < 0.");
    }

    std::cout << "[CIRProcess] Parametres OK." << std::endl;
    std::cout << "[CIRProcess] Feller condition = "
              << (satisfiesFellerCondition() ? "satisfaite" : "non satisfaite")
              << std::endl;
}

bool CIRProcess::satisfiesFellerCondition() const {
    return 2.0 * params.kappa * params.theta >= params.xi * params.xi;
}

double CIRProcess::nextVarianceEuler(double currentVariance,
                                     double dt,
                                     double gaussianShock) const {
    if (dt <= 0.0) {
        throw std::runtime_error("[CIRProcess] Erreur : dt <= 0.");
    }

    double vPos = std::max(currentVariance, 0.0);

    double drift = params.kappa * (params.theta - vPos) * dt;
    double diffusion = params.xi * std::sqrt(vPos) * std::sqrt(dt) * gaussianShock;

    double nextV = vPos + drift + diffusion;

    // Troncature pour éviter une variance négative après le pas
    nextV = std::max(nextV, 0.0);

    return nextV;
}
