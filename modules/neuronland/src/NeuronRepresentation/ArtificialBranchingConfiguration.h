#ifndef ARTIFICIALBRANCHINGCONFIGURATION_HPP_
#define ARTIFICIALBRANCHINGCONFIGURATION_HPP_

/** 

	Given a 1D neuron (length + diameter), these parameters descibe how to create 
	an artificial branching configuration.

*/

enum Pattern
{
	kPatternPlanar,
	KPattern3DRandomSwivel,
};

struct ArtificialBranchingConfiguration
{
	float rootCenterAngle;
	float rootRangeAngle;
	
	float rootCenterAngleApical;
	float rootRangeAngleApical;

	float rootCenterAngleAxon;
	float rootRangeAngleAxon;
	
	float childBranchRangeAngle;
	float childBranchRangeAngleApical;
	float childBranchRangeAngleAxon;

	float childBranchSwivel;
	float childBranchTilt;
	Pattern pattern;

	ArtificialBranchingConfiguration() :
		rootCenterAngle(90.0f),
		rootRangeAngle(150.0f),
		rootCenterAngleApical(270.0f),
		rootRangeAngleApical(45.0f),
		rootCenterAngleAxon(180.0f),
		rootRangeAngleAxon(45.0f),

		childBranchRangeAngle(36.0f),
		childBranchRangeAngleApical(36.0f),
		childBranchRangeAngleAxon(36.0f),

		pattern(kPatternPlanar)
	{

	}
};

#endif // ARTIFICIALBRANCHINGCONFIGURATION_HPP_

