#ifndef PATHSIMULATOR_H
#define PATHSIMULATOR_H

#include "Model.h"
#include <vector>

#include <random>

// Abstract class 
class PathSimulator
{
public:
	PathSimulator(const std::vector<double>& timePoints, const Model& model);

	std::vector<double> path() const;


protected:
	bool sanityCheck() const; // Check that the time points are well defined

	virtual double nextStep(size_t timeIdx, double assetPrice) const = 0;

	std::vector<double> _timePoints; // {t_0, ... t_M}
	const Model* _modelPtr;

	std::random_device _rd;
	std::mt19937 _mt;   // Mersenne Twister 2^x - 1 
	std::normal_distribution<double> _nd;
};

class EulerPathSimulator : public PathSimulator
{
public:
	double nextStep(size_t timeIdx, double assetPrice) const override;
};





#endif // !PATHSIMULATOR_H

