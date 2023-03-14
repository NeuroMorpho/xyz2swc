#ifndef NEURONSTATISTICSBASIC_HPP_
#define NEURONSTATISTICSBASIC_HPP_

#include "Core/Output.h"

#include <vector>
#include <ostream>

#include "TreeStatisticsBasic.h"
#include "SomaStatisticsBasic.h"

#include "StringFormatting.h"

struct NeuronStatisticsBasic
{
	// trees
	std::vector<TreeStatisticsBasic>	perTreeStats;
	TreeStatisticsBasic					combinedTreeStats;
	TreeStatisticsBasic					combinedAxonStats;
	TreeStatisticsBasic					combinedDendStats;
	u32									numAxons;
	u32									numDendrites;

	// somas
	SomaStatisticsBasic					combinedSomaStats;
	std::vector<SomaStatisticsBasic>	perSomaStats;
	u32									numSomas;

	// spines
	u32									numGlobalSpines;

	// markers
	u32									numGlobalMarkers;


	void Clear() 
	{
		numAxons = 0;
		numDendrites = 0;
		numSomas = 0;

		perTreeStats.clear(); 
		combinedTreeStats.Clear();

		perSomaStats.clear();
		combinedSomaStats.Clear();

		numGlobalMarkers = 0;
		numGlobalSpines = 0;
	}

	void PrintTreeStatistics(std::ostream& s, const TreeStatisticsBasic& stats) const
	{
		s << "\tSample Points:                        " << stats.numSamples << std::endl;
		s << "\tBranch Segments:                      " << stats.numBranches << std::endl;
		s << "\tBranchPoints:                         " << stats.numBranchPoints << std::endl;
		s << "\tBranching Order:                      " << stats.branchingOrder << std::endl;
		s << "\tTerminals:                            " << stats.numTerminals << std::endl;
		s << "\tSpines:                               " << stats.numSpines << std::endl;
		s << "\tMarkers:                              " << stats.numMarkers << std::endl;
		s << "\tMax. Terminal Path Distance (um):     " << stats.maxTerminalDistancePath << std::endl;
		s << "\tMax. Terminal Physical Distance (um): " << stats.maxTerminalDistancePhys << std::endl;
		s << "\tMin. Diameter (um):                   " << stats.minDiameter << std::endl;
		s << "\tMax. Diameter (um):                   " << stats.maxDiameter << std::endl;
		s << "\tTotal Tree Length (um):               " << stats.totalLength << std::endl;
		s << "\tVolume (um^3):                        " << stats.totalVolume << std::endl;
		s << "\tSurface Area: (um^2)                  " << stats.totalSurface << std::endl;
	}

	void PrintTreeStatistics(Output* pOut, const TreeStatisticsBasic& stats) const
	{
/*		pOut->writeLine("\tSample Points:                        " + dec(stats.numSamples) );
		pOut->writeLine("\tBranch Segments:                      " + dec(stats.numBranches) );
		pOut->writeLine("\tBranchPoints:                         " + dec(stats.numBranchPoints) );
		pOut->writeLine("\tBranching Order:                      " + dec(stats.branchingOrder) );
		pOut->writeLine("\tTerminals:                            " + dec(stats.numTerminals) );
		pOut->writeLine("\tSpines:                               " + dec(stats.numSpines) );
		pOut->writeLine("\tMarkers:                              " + dec(stats.numMarkers) );
		pOut->writeLine("\tMax. Terminal Path Distance (um):     " + dec(stats.maxTerminalDistancePath) );
		pOut->writeLine("\tMax. Terminal Physical Distance (um): " + dec(stats.maxTerminalDistancePhys) );
		pOut->writeLine("\tMin. Diameter (um):                   " + dec(stats.minDiameter) );
		pOut->writeLine("\tMax. Diameter (um):                   " + dec(stats.maxDiameter) );
		pOut->writeLine("\tTotal Tree Length (um):               " + dec(stats.totalLength) );
		pOut->writeLine("\tVolume (um^3):                        " + dec(stats.totalVolume) );
		pOut->writeLine("\tSurface Area: (um^2)                  " + dec(stats.totalSurface) );*/
	}

	void PrintSomaStatistics(std::ostream& s, const SomaStatisticsBasic& stats) const
	{
		s << "\tSample Points:                        " << stats.numSamples << std::endl;
		s << "\tSpines:                               " << stats.numSpines << std::endl;
		s << "\tMarkers:                              " << stats.numMarkers << std::endl;
		s << "\tTotal Length (um):                    " << stats.totalLength << std::endl;
		s << "\tMin. Diameter (um):                   " << stats.minDiameter << std::endl;
		s << "\tMax. Diameter (um):                   " << stats.maxDiameter << std::endl;
	}

	void PrintSomaStatistics(Output* pOut, const SomaStatisticsBasic& stats) const
	{
		pOut->writeLine("\tSample Points:                        " + dec(stats.numSamples) );
		pOut->writeLine("\tSpines:                               " + dec(stats.numSpines) );
		pOut->writeLine("\tMarkers:                              " + dec(stats.numMarkers) );
		pOut->writeLine("\tTotal Length (um):                    " + dec(stats.totalLength) );
		pOut->writeLine("\tMin. Diameter (um):                   " + dec(stats.minDiameter) );
		pOut->writeLine("\tMax. Diameter (um):                   " + dec(stats.maxDiameter) );
	}

	void Print(std::ostream& s) const
	{
		s << "Number of Axons:          " << numAxons << std::endl;
		s << "Number of Dends:          " << numDendrites << std::endl;
		s << "Number of Somas:          " << numSomas << std::endl;
		s << "Number of Global Spines:  " << numGlobalSpines << std::endl;
		s << "Number of Global Markers: " << numGlobalMarkers << std::endl;

		if(numAxons + numDendrites > 0)
		{
			s << "Tree Statistics Global:" << std::endl;
			PrintTreeStatistics(s, combinedTreeStats);

			for(u32 n=0;n<numAxons+numDendrites;++n)
			{
				if(n < numAxons)
					s << "Tree Statistics for AXON TREE #" << n+1 << std::endl;
				else
					s << "Tree Statistics for DEND TREE #" << n+1 << std::endl;

				PrintTreeStatistics(s, perTreeStats[n]);
			}
		}

		if(numSomas > 0)
		{
			s << "Soma Statistics Global:" << std::endl;
			PrintSomaStatistics(s, combinedSomaStats);

			for(u32 n=0;n<numSomas;++n)
			{
				s << "Soma Statistics for SOMA #" << n+1 << std::endl;

				PrintSomaStatistics(s, perSomaStats[n]);
			}
		}
	}

	void Print(Output* pOut) const
	{
		pOut->writeLine("Number of Axons:          " + dec(numAxons) );
		pOut->writeLine("Number of Dends:          " + dec(numDendrites) );
		pOut->writeLine("Number of Somas:          " + dec(numSomas) );
		pOut->writeLine("Number of Global Spines:  " + dec(numGlobalSpines) );
		pOut->writeLine("Number of Global Markers: " + dec(numGlobalMarkers) );
		pOut->writeLine("Tree Statistics Global:");

		if(numAxons + numDendrites > 0)
		{
			PrintTreeStatistics(pOut, combinedTreeStats);

			for(u32 n=0;n<numAxons+numDendrites;++n)
			{
				if(n < numAxons)
					pOut->writeLine("Tree Statistics for AXON TREE #" + dec(n+1) );
				else
					pOut->writeLine("Tree Statistics for DEND TREE #" + dec(n+1) );

				PrintTreeStatistics(pOut, perTreeStats[n]);
			}
		}

		if(numSomas > 0)
		{
			pOut->writeLine("Soma Statistics Global:");
			PrintSomaStatistics(pOut, combinedSomaStats);

			for(u32 n=0;n<numSomas;++n)
			{
				pOut->writeLine("Soma Statistics for SOMA #" + dec(n+1) );

				PrintSomaStatistics(pOut, perSomaStats[n]);
			}
		}
	}
};

#endif // NEURONSTATISTICSBASIC_HPP_
