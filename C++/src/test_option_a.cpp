#include <iostream>
#include <utility>
#include <cmath>

#include "HestonParams.h"
#include "CorrelatedRng.h"
#include "HestonProcess.h"
#include "EuropeanOption.h"
#include "EulerSchemePricer.h"
#include "TruncatedGaussianSchemePricer.h"
#include "QuadraticExponentialSchemePricer.h"
#include "BroadieKayaPricer.h"
#include "HestonSemiAnalyticalPricer.h"

int main() {
    std::cout << "==== Check option A : Euler vs TG vs QE vs BK vs semi-analytique ====" << std::endl;

    HestonParams params;
    params.spot = 100.0;
    params.rate = 0.02;
    params.kappa = 2.0;
    params.theta = 0.04;
    params.xi = 0.30;
    params.rho = -0.70;
    params.v0 = 0.04;

    std::cout << "[Etape 1] Parametres de Heston initialises" << std::endl;

    CorrelatedRng rng(params.rho, 42u);
    HestonProcess process(params);

    double spot = params.spot;
    double variance = params.v0;
    double maturity = 1.0;
    int nStepsPath = 10;
    double dt = maturity / static_cast<double>(nStepsPath);

    std::cout << "[Etape 2] Test trajectoire simple" << std::endl;

    for (int i = 0; i < nStepsPath; ++i) {
        std::pair<double, double> shocks = rng.nextPair();
        std::pair<double, double> nextState =
            process.nextStepEuler(spot, variance, dt, shocks.first, shocks.second);

        spot = nextState.first;
        variance = nextState.second;

        if (spot <= 0.0) {
            throw std::runtime_error("[test_option_a] Erreur : spot <= 0 apres simulation.");
        }

        if (variance < 0.0) {
            throw std::runtime_error("[test_option_a] Erreur : variance < 0 apres simulation.");
        }
    }

    std::cout << "[OK] Simulation Euler trajectoire validee" << std::endl;

    std::cout << "[Etape 3] Comparaison des pricers" << std::endl;

    EuropeanOption call(100.0, 1.0, true);

    EulerSchemePricer eulerPricer(params, 42u);
    TruncatedGaussianSchemePricer tgPricer(params, 42u);
    QuadraticExponentialSchemePricer qePricer(params, 42u);
    BroadieKayaPricer bkPricer(params, 42u);
    HestonSemiAnalyticalPricer semiPricer(params);

    int nPaths = 5000;
    int nStepsPricing = 100;

    MonteCarloResult eulerResult = eulerPricer.price(call, nPaths, nStepsPricing);
    MonteCarloResult tgResult = tgPricer.price(call, nPaths, nStepsPricing);
    MonteCarloResult qeResult = qePricer.price(call, nPaths, nStepsPricing);
    MonteCarloResult bkResult = bkPricer.price(call, nPaths, nStepsPricing);
    double semiPrice = semiPricer.callPrice(call);

    double eulerError = std::fabs(eulerResult.price - semiPrice);
    double tgError = std::fabs(tgResult.price - semiPrice);
    double qeError = std::fabs(qeResult.price - semiPrice);
    double bkError = std::fabs(bkResult.price - semiPrice);

    if (eulerResult.price <= 0.0) {
        throw std::runtime_error("[test_option_a] Erreur : prix Euler <= 0.");
    }
    if (tgResult.price <= 0.0) {
        throw std::runtime_error("[test_option_a] Erreur : prix TG <= 0.");
    }
    if (qeResult.price <= 0.0) {
        throw std::runtime_error("[test_option_a] Erreur : prix QE <= 0.");
    }
    if (bkResult.price <= 0.0) {
        throw std::runtime_error("[test_option_a] Erreur : prix BK <= 0.");
    }
    if (semiPrice <= 0.0) {
        throw std::runtime_error("[test_option_a] Erreur : prix semi-analytique <= 0.");
    }

    if (eulerResult.stdError < 0.0 || tgResult.stdError < 0.0 ||
        qeResult.stdError < 0.0 || bkResult.stdError < 0.0) {
        throw std::runtime_error("[test_option_a] Erreur : stdError negatif.");
    }

    if (eulerResult.lowerCI > eulerResult.upperCI) {
        throw std::runtime_error("[test_option_a] Erreur : IC Euler incoherent.");
    }
    if (tgResult.lowerCI > tgResult.upperCI) {
        throw std::runtime_error("[test_option_a] Erreur : IC TG incoherent.");
    }
    if (qeResult.lowerCI > qeResult.upperCI) {
        throw std::runtime_error("[test_option_a] Erreur : IC QE incoherent.");
    }
    if (bkResult.lowerCI > bkResult.upperCI) {
        throw std::runtime_error("[test_option_a] Erreur : IC BK incoherent.");
    }

    std::cout << "[OK] Pricing Euler valide" << std::endl;
    std::cout << "     Prix Euler = " << eulerResult.price << std::endl;
    std::cout << "     StdError   = " << eulerResult.stdError << std::endl;
    std::cout << "     IC95%      = [" << eulerResult.lowerCI << ", " << eulerResult.upperCI << "]" << std::endl;

    std::cout << "[OK] Pricing TG valide" << std::endl;
    std::cout << "     Prix TG    = " << tgResult.price << std::endl;
    std::cout << "     StdError   = " << tgResult.stdError << std::endl;
    std::cout << "     IC95%      = [" << tgResult.lowerCI << ", " << tgResult.upperCI << "]" << std::endl;

    std::cout << "[OK] Pricing QE valide" << std::endl;
    std::cout << "     Prix QE    = " << qeResult.price << std::endl;
    std::cout << "     StdError   = " << qeResult.stdError << std::endl;
    std::cout << "     IC95%      = [" << qeResult.lowerCI << ", " << qeResult.upperCI << "]" << std::endl;

    std::cout << "[OK] Pricing BK valide" << std::endl;
    std::cout << "     Prix BK    = " << bkResult.price << std::endl;
    std::cout << "     StdError   = " << bkResult.stdError << std::endl;
    std::cout << "     IC95%      = [" << bkResult.lowerCI << ", " << bkResult.upperCI << "]" << std::endl;

    std::cout << "[OK] Pricing semi-analytique valide" << std::endl;
    std::cout << "     Prix Semi  = " << semiPrice << std::endl;

    std::cout << "[OK] Ecarts absolus calcules" << std::endl;
    std::cout << "     |Euler - Semi| = " << eulerError << std::endl;
    std::cout << "     |TG - Semi|    = " << tgError << std::endl;
    std::cout << "     |QE - Semi|    = " << qeError << std::endl;
    std::cout << "     |BK - Semi|    = " << bkError << std::endl;

    std::cout << "==== Option A : check Euler vs TG vs QE vs BK vs semi-analytique valide ====" << std::endl;

    return 0;
}