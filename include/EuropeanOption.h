#ifndef EUROPEANOPTION_H
#define EUROPEANOPTION_H

#include <algorithm>
#include <stdexcept>

class EuropeanOption {
private:
    double strike;
    double maturity;
    bool isCall;

public:
    EuropeanOption(double strike, double maturity, bool isCall)
        : strike(strike), maturity(maturity), isCall(isCall) {
        if (strike <= 0.0) {
            throw std::runtime_error("[EuropeanOption] Erreur : strike <= 0.");
        }
        if (maturity <= 0.0) {
            throw std::runtime_error("[EuropeanOption] Erreur : maturity <= 0.");
        }
    }

    double getStrike() const {
        return strike;
    }

    double getMaturity() const {
        return maturity;
    }

    bool getIsCall() const {
        return isCall;
    }

    double payoff(double spotAtMaturity) const {
        if (isCall) {
            return std::max(spotAtMaturity - strike, 0.0);
        }
        return std::max(strike - spotAtMaturity, 0.0);
    }
};

#endif
