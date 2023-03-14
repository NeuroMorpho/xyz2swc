#ifndef TREESTATISTICSBASIC_HPP_
#define TREESTATISTICSBASIC_HPP_

/** 

	Basic properties which can be easily calculated for a neural tree..

*/

struct TreeStatisticsBasic
{
	u32		numBranches;
	u32		numBranchPoints;
	u32		branchingOrder;
	u32		numTerminals;

	u32		numSamples;

	u32		numMarkers;
	u32		numSpines;

	float	totalLength;
	float	maxTerminalDistancePath;
	float	maxTerminalDistancePhys;

	float	maxDiameter;
	float	minDiameter;

	float	totalSurface;
	float	totalVolume;

	TreeStatisticsBasic()
	{
		Clear();
	}

	void Clear()
	{
		numBranches = 0;
		numBranchPoints = 0;
		branchingOrder = 0;
		numTerminals = 0;

		numSamples = 0;
		numMarkers = 0;
		numSpines = 0;

		totalLength = 0;
		maxTerminalDistancePath = 0;
		maxTerminalDistancePhys = 0;

		maxDiameter = 0;
		minDiameter = FLT_MAX;

		totalSurface = 0;
		totalVolume = 0;
	}
};


#endif // TREESTATISTICSBASIC_HPP_
