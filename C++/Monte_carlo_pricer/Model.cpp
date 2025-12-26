#include "Model.h"

Model::Model(double initValue)
	: _initialValue(initValue)
{
}

Model& Model::operator=(const Model& model)
{
	if (this != &model)
	{
		_initialValue = model._initialValue;
	}
	return *this;
}

BlackScholesModel::BlackScholesModel(double initValue, double drift, double volatility)
	: Model(initValue), _drift(drift), _volatility(volatility)
{
}

BlackScholesModel& BlackScholesModel::operator=(const BlackScholesModel& model)
{
	if (this != &model)
	{
		Model::operator=(model);
		_drift = model._drift;
		_volatility = model._volatility;
	}
}

double BlackScholesModel::driftTerm(double time, double assetPrice) const
{
	return _drift * assetPrice;
}

double BlackScholesModel::diffusionTerm(double time, double assetPrice) const
{
	return _volatility * assetPrice;
}

BlackScholesModel* BlackScholesModel::clone() const
{
	return new BlackScholesModel(*this);
}

double DupireLocalVolatility::driftTerm(double time, double assetPrice) const
{
	// TO DO
	return 0.0;
}

double DupireLocalVolatility::diffusionTerm(double time, double assetPrice) const
{
	return localVolatility(time, assetPrice) * assetPrice;
}

DupireLocalVolatility* DupireLocalVolatility::clone() const
{
	return new DupireLocalVolatility(*this);
}

double DupireLocalVolatility::localVolatility(double time, double assetPrice) const
{
	// TO DO
	return 0.0;
}
