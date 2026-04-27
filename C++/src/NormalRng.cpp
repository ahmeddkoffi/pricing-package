#include "NormalRng.h"

NormalRng::NormalRng(unsigned int seed)
    : generator(seed), normalDist(0.0, 1.0) {}

double NormalRng::next() const {
    return normalDist(generator);
}
