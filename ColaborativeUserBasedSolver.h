#pragma once
#include "ColaborativeSolver.h"
#include <unordered_map>

class ColaborativeUserBasedParameters : public ColaborativeParameters
{
public:
	ColaborativeUserBasedParameters(int neighboorsAmount, float accuracyThresholdValue, Normalizer * normalizationStrategy);

	virtual string getAlgorithmName() override;
};

class ColaborativeUserBasedSolver : public ColaborativeSolver
{
public:
	ColaborativeUserBasedSolver(data_input * input, Parameters * params);
protected:
	virtual string chooseMainDimension(string userId, string itemId) override;
	virtual string chooseAuxDimension(string userId, string itemId) override;
	virtual vector<data_node*> * getMainDimensionFunc(string dimensionId) override;
	virtual vector<data_node*> * getAuxDimensionFunc(string dimensionId) override;
	virtual bool sortBeforeSolveFunc(data_node i, data_node j) override;
};
