#ifndef SOMASTATISTICSBASIC_HPP_
#define SOMASTATISTICSBASIC_HPP_

/** 

	Basic properties which can be easily calculated for a soma 

*/

struct SomaStatisticsBasic
{
	u32		numSamples;
	u32		numMarkers;
	u32		numSpines;

	float	totalLength;

	float	maxDiameter;
	float	minDiameter;

	//float	totalSurface;
	//float	totalVolume;

	SomaStatisticsBasic()
	{
		Clear();
	}

	void Clear()
	{
		numSamples = 0;
		numMarkers = 0;
		numSpines = 0;

		totalLength = 0;

		maxDiameter = 0;
		minDiameter = FLT_MAX;

		//totalSurface = 0;
		//totalVolume = 0;
	}
};


#endif // SOMASTATISTICSBASIC_HPP_
