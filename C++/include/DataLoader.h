#ifndef DATALOADER_H
#define DATALOADER_H

#include <string>
#include <vector>
#include "MarketDataPoint.h"

class DataLoader {
private:
    std::string filename;

public:
    DataLoader(const std::string& filename);
    std::vector<MarketDataPoint> load() const;
};

#endif
