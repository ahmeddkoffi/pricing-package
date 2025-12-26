#include "PathSimulator.h"

PathSimulator::PathSimulator(const std::vector<double>& timePoints, const Model& model)
	: _timePoints(timePoints), _modelPtr(model.clone())
{
	if (!sanityCheck())
		throw "Time Points are ill-defined";
}

std::vector<double> PathSimulator::path() const
{
	std::vector<double> path;
	path.reserve(_timePoints.size()); // optimization for CPU memory

	// Initialization at t_0
	path.push_back(_modelPtr->initialValue()); // path[0]

	for (size_t timeIdx = 0; timeIdx < _timePoints.size() - 1; ++timeIdx)
		path.push_back(nextStep(timeIdx, path[timeIdx]));

	return path;
}

bool PathSimulator::sanityCheck() const
{
	// TO DO
	return true;
}

double EulerPathSimulator::nextStep(size_t timeIdx, double assetPrice) const
{
	// TO DO 
	// S_{t + Delta_t} = S_t + mu(t, S_t) * Delta_t + sigma(t,S_t) * sqrt(Delta_t) * N(0,1)

	return 0.0;
}
