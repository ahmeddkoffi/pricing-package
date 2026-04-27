#ifndef MODEL_H 
#define MODEL_H

// Abstract class
class Model
{
public:
	// Special methods
	// 1) Default constructor
	// 2) Copy constructor
	// 3) Assignement operator
	// 4) Destructor

	// Constructor with parameters
	Model(double initValue);

	// Model(const Model& model);
	Model& operator=(const Model& model);
	~Model() = default;

	virtual double driftTerm(double time, double assetPrice) const = 0;
	virtual double diffusionTerm(double time, double assetPrice) const = 0;

	virtual Model* clone() const = 0;

	// Getter
	inline double initialValue() const
	{
		return _initialValue;
	}

//private:
protected:
	double _initialValue;
};


class BlackScholesModel : public Model
{
public:
	// Constructor with parameters
	BlackScholesModel(double initValue, double drift, double volatility);

	BlackScholesModel& operator=(const BlackScholesModel& model);

	double driftTerm(double time, double assetPrice) const override;
	double diffusionTerm(double time, double assetPrice) const override;

	BlackScholesModel* clone() const override;

private:
	double _drift;
	double _volatility;
};

class DupireLocalVolatility : public Model
{
public:
	double driftTerm(double time, double assetPrice) const override;
	double diffusionTerm(double time, double assetPrice) const override;

	DupireLocalVolatility* clone() const override;

private:
	// Methods : Dupire local volatility method
	double localVolatility(double time, double assetPrice) const;
	
	// Members : Information on implied volatility surface
};






#endif // !MODEL_H 

