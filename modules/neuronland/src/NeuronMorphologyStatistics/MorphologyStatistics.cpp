//
// NeuronStatistics.cpp
//
#include <float.h>
#include "NeuronRepresentation/Neuron3D.h"

#include "NeuronStatisticsBasic.h"
#include "MorphologyDataFaults.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Per tree element accumulators
//

struct AccumulatorTreeStatisticsBasic
{
	// intrinsic geometrical properties
	Neuron3D::DendriteTree::TerminalCounter			terminalCounter;
	Neuron3D::DendriteTree::BranchCounter			branchCounter;
	Neuron3D::DendriteTree::BranchPointCounter		branchPointCounter;
	Neuron3D::DendriteTree::BranchingOrderCounter 	branchingOrderCounter;

	// branch-type and measurement-unit dependent properties
	Neuron3D::LengthAccumulator						lengthAccumulator;
	Neuron3D::SampleCounter							sampleCounter;
	Neuron3D::MaxDistancePathAccumulator			distancePathAccumulator;
	Neuron3D::MaxDistancePhysAccumulator			distancePhysAccumulator;
	Neuron3D::VolumeAccumulator						volumeAccumulator;
	Neuron3D::SurfaceAreaAccumulator				surfaceAccumulator;

	Neuron3D::MarkerCounter							markerCounter;
	Neuron3D::SpineCounter							spineCounter;

	Neuron3D::MaxDiamAccumulator					maxDiamAccumulator;
	Neuron3D::MinDiamAccumulator					minDiamAccumulator;

	AccumulatorTreeStatisticsBasic() {}

	void operator()(Neuron3D::DendriteTreeConstIterator it)
	{
		terminalCounter(it);
		branchCounter(it);
		branchPointCounter(it);
		branchingOrderCounter(it);

		lengthAccumulator(it);
		sampleCounter(it);
		distancePathAccumulator(it);
		distancePhysAccumulator(it);

		volumeAccumulator(it);
		surfaceAccumulator(it);

		markerCounter(it);
		spineCounter(it);

		maxDiamAccumulator(it);
		minDiamAccumulator(it);
	}

	void Extract(TreeStatisticsBasic& stats)
	{
		stats.numBranches				= branchCounter.GetCount();
		stats.numBranchPoints			= branchPointCounter.GetCount();
		stats.branchingOrder			= branchingOrderCounter.GetCount();
		stats.numTerminals				= terminalCounter.GetCount();

		stats.numSamples				= sampleCounter.GetCount() + 1;
		stats.totalLength				= lengthAccumulator.GetValue();
		stats.maxTerminalDistancePath	= distancePathAccumulator.GetValue();
		stats.maxTerminalDistancePhys	= distancePhysAccumulator.GetValue();

		stats.totalSurface				= surfaceAccumulator.GetValue();
		stats.totalVolume				= volumeAccumulator.GetValue();

		stats.numSpines					= spineCounter.GetCount();
		stats.numMarkers				= markerCounter.GetCount();

		stats.minDiameter				= minDiamAccumulator.GetValue();
		stats.maxDiameter				= maxDiamAccumulator.GetValue();
	}

	void Reset()
	{
		terminalCounter.Reset();
		branchCounter.Reset();
		branchPointCounter.Reset();
		branchingOrderCounter.Reset();
		lengthAccumulator.Reset();
		sampleCounter.Reset();
		distancePathAccumulator.Reset();
		distancePhysAccumulator.Reset();

		surfaceAccumulator.Reset();
		volumeAccumulator.Reset();

		spineCounter.Reset();
		markerCounter.Reset();

		minDiamAccumulator.Reset();
		maxDiamAccumulator.Reset();
	}
};


struct AccumulatorSomaStatistics
{
	SomaStatisticsBasic mStats;

	AccumulatorSomaStatistics() {}

	void operator()(Neuron3D::SomaConstIterator it)
	{
		mStats.maxDiameter = it->GetMaxDiam();
		mStats.minDiameter = it->GetMinDiam();

		mStats.numSamples = (u32)it->m_samples.size();
		mStats.numSpines = it->GetNumSpines();
		mStats.numMarkers = it->GetNumMarkers();

		mStats.totalLength = it->GetLength();
	}


	void Extract(SomaStatisticsBasic& stats)
	{
		stats = mStats;
	}

	void Reset()
	{
		mStats.Clear();
	}
};


struct AccumulatorTreeFaults
{
	MorphologyDataFaults&	m_faults;

	u32 m_treeId;
	u32 m_branchId;

	AccumulatorTreeFaults(MorphologyDataFaults& faults, u32 treeId) : m_faults(faults), m_treeId(treeId), m_branchId(0)
	{
	}

	void AddFault(NeuronFaultType type, u32 index, const SamplePoint& sample)
	{
		NeuronFaultData data = { type, m_treeId, m_branchId, index, sample};
		m_faults.Add(data);
	}

	void operator()(Neuron3D::DendriteTreeConstIterator it)
	{
		const u32 numSamples = (u32) it->m_samples.size();

		if(numSamples == 0)
		{
			AddFault(kTreeBranchSamplesZero, 0, SamplePoint() );
		}
		else if(numSamples == 1)
		{
			AddFault(kTreeBranchSamplesOnlyOne, 0, it->m_samples[0]);
		}
		else
		{
			const SamplePoint& startSample = it->m_samples[0];
			const SamplePoint& endSample = it->m_samples[numSamples-1];

			const u32 numChildren = Neuron3D::DendriteTree::countchildren(it);
			if(numChildren == 1)
			{
				AddFault(kTreeBranchPointUnary, numSamples-1, endSample);
			}
			else if(numChildren == 3)
			{
				AddFault(kTreeBranchPointTrinary, numSamples-1, endSample);
			}
			else if(numChildren > 3)
			{
				AddFault(kTreeBranchPointNary, numSamples-1, endSample);
			}
			else
			{
				if(endSample.d > 100.0f)
					AddFault(kTreeBranchTerminalDiameterTooBig, numSamples-1, endSample);
			}

			const float xStart = startSample.x;
			const float yStart = startSample.y;
			const float zStart = startSample.z;
			const float dStart = startSample.d;

			const Neuron3D::DendriteTreeConstIterator itParent = it.parent();

			bool doEnclosureTest = false;

			if(itParent)
			{
				const u32 numParentSamples = (u32) (*itParent).m_samples.size();
				const SamplePoint& parentSample = (*itParent).m_samples[numParentSamples-1];

				const float xParent = parentSample.x;
				const float yParent = parentSample.y;
				const float zParent = parentSample.z;
				const float dParent = parentSample.d;

				// might want to put a threshold on this
				if(xParent != xStart || yParent	 != yStart || zParent != zStart)
					AddFault(kTreeBranchDisjointToParent, 0, it->m_samples[0]);

				if(dStart > dParent * 3)
					AddFault(kTreeBranchDiameterLargeJumpFromParent, 0, it->m_samples[0]);

				/*if( (*it).m_samples[numSamples-1].Distance(parentSample) )
				{
					doEnclosureTest = true;
				}*/
			}

			for(u32 n=0;n<numSamples;++n)
			{
				const SamplePoint& sample = (*it).m_samples[n];
				if(sample.d > 1000.0f)
					AddFault(kTreeBranchSampleDiameterTooBig, n, sample);

				if(sample.d == 0.0f)
					AddFault(kTreeBranchSampleDiameterZero, n, sample);
				else if(sample.d < 0.05f)
					AddFault(kTreeBranchSampleDiameterTiny, n, sample);


				if(n>0)
				{
					const SamplePoint lastSample = (*it).m_samples[n-1];
					if( sample.Distance(lastSample) > 10000.0f )
						AddFault(kTreeBranchSampleTooDistant, n, sample);

					if( sample.Distance(lastSample) < 0.005f)
						AddFault(kTreeBranchSamplesTooClose, n, sample);
				}
			}

			if(doEnclosureTest)
			{
				// todo - additional enclosure test..
				AddFault(kTreeBranchFallsWithinParentRadius, 0, it->m_samples[0]);
			}


			{
				Branch3D::Markers::const_iterator mit = (*it).m_markers.begin();
				Branch3D::Markers::const_iterator end = (*it).m_markers.end();

				for(;mit != end;++mit)
				{
					for(u32 n=0;n<mit->second.size();++n)
					{
						if(mit->second[n].x > 10000.0f)
							AddFault(kTreeBranchMarkerTooDistant, n, mit->second[n]);
					}
				}
			}
		}

		++m_branchId;
	}
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Per structure accumulators
//

void CalculateGlobalNeuronStatistics(const Neuron3D& nrn, NeuronStatisticsBasic& stats)
{
	stats.numAxons = nrn.CountAxons();
	stats.numDendrites = nrn.CountDendrites();
	stats.numSomas = nrn.CountSomas();
	stats.numGlobalMarkers = nrn.CountGlobalMarkers();
	stats.numGlobalSpines = nrn.CountGlobalSpines();


	// accumulate tree stats
	std::vector<TreeStatisticsBasic>::const_iterator it = stats.perTreeStats.begin();
	std::vector<TreeStatisticsBasic>::const_iterator end = stats.perTreeStats.end();

	for(;it!=end;++it)
	{
		stats.combinedTreeStats.numTerminals += (*it).numTerminals;
		stats.combinedTreeStats.numBranches += (*it).numBranches;
		stats.combinedTreeStats.numBranchPoints += (*it).numBranchPoints;
		stats.combinedTreeStats.branchingOrder = std::max( (*it).branchingOrder, stats.combinedTreeStats.branchingOrder);

		stats.combinedTreeStats.totalLength += (*it).totalLength;
		stats.combinedTreeStats.numSamples += (*it).numSamples;
		stats.combinedTreeStats.maxTerminalDistancePath = std::max( (*it).maxTerminalDistancePath, stats.combinedTreeStats.maxTerminalDistancePath);
		stats.combinedTreeStats.maxTerminalDistancePhys = std::max( (*it).maxTerminalDistancePhys, stats.combinedTreeStats.maxTerminalDistancePhys);

		stats.combinedTreeStats.totalVolume += (*it).totalVolume;
		stats.combinedTreeStats.totalSurface += (*it).totalSurface;

		stats.combinedTreeStats.numSpines += (*it).numSpines;
		stats.combinedTreeStats.numMarkers += (*it).numMarkers;

		stats.combinedTreeStats.maxDiameter = std::max( (*it).maxDiameter, stats.combinedTreeStats.maxDiameter);
		stats.combinedTreeStats.minDiameter = std::min( (*it).minDiameter, stats.combinedTreeStats.minDiameter);
	}

	// accumulate soma stats
	std::vector<SomaStatisticsBasic>::const_iterator sit = stats.perSomaStats.begin();
	std::vector<SomaStatisticsBasic>::const_iterator send = stats.perSomaStats.end();

	for(;sit!=send;++sit)
	{
		stats.combinedSomaStats.totalLength += (*sit).totalLength;
		stats.combinedSomaStats.numSamples += (*sit).numSamples;

		stats.combinedSomaStats.numSpines += (*sit).numSpines;
		stats.combinedSomaStats.numMarkers += (*sit).numMarkers;

		stats.combinedSomaStats.maxDiameter = std::max( (*sit).maxDiameter, stats.combinedSomaStats.maxDiameter);
		stats.combinedSomaStats.minDiameter = std::min( (*sit).minDiameter, stats.combinedSomaStats.minDiameter);
	}
}

void CalculateGlobalNeuronFaults(const Neuron3D& nrn, MorphologyDataFaults& faults)
{
// Check root branch connections to soma..
	if(nrn.CountSomas())
	{
		Neuron3D::SomaConstIterator itSoma = nrn.SomaBegin();
		Neuron3D::SomaConstIterator endSoma = nrn.SomaEnd();

		float xSomaAccumulator = 0.0f;
		float ySomaAccumulator = 0.0f;
		float zSomaAccumulator = 0.0f;

		float xSomaMax = -FLT_MAX;
		float ySomaMax = -FLT_MAX;
		float zSomaMax = -FLT_MAX;

		float xSomaMin = FLT_MAX;
		float ySomaMin = FLT_MAX;
		float zSomaMin = FLT_MAX;

		u32 nSomaPoints = 0;

		for(;itSoma!=endSoma;++itSoma)
		{
			for(u32 n=0;n<(*itSoma).m_samples.size();++n)
			{
				const float x = (*itSoma).m_samples[n].x;
				const float y = (*itSoma).m_samples[n].y;
				const float z = (*itSoma).m_samples[n].z;

				xSomaAccumulator += x;
				ySomaAccumulator += y;
				zSomaAccumulator += z;

				xSomaMax = std::max(xSomaMax, x);
				ySomaMax = std::max(ySomaMax, y);
				zSomaMax = std::max(zSomaMax, z);

				xSomaMin = std::min(xSomaMin, x);
				ySomaMin = std::min(ySomaMin, y);
				zSomaMin = std::min(zSomaMin, z);

				++nSomaPoints;
			}
		}

		const float invNSomaPoints = 1.0f/(float)nSomaPoints;

		const float xSoma = xSomaAccumulator*invNSomaPoints;
		const float ySoma = ySomaAccumulator*invNSomaPoints;
		const float zSoma = zSomaAccumulator*invNSomaPoints;

		const float xSomaRadius = (xSomaMax - xSomaMin)*0.5f;
		const float ySomaRadius = (ySomaMax - ySomaMin)*0.5f;
		const float zSomaRadius = (zSomaMax - zSomaMin)*0.5f;

		if(nSomaPoints > 1)
		{
			// check each tree is connected-ish to the soma. (TODO - remove code duplication)
			u32 treeCount = 0;

			Neuron3D::AxonConstIterator itAxon	= nrn.AxonBegin();
			Neuron3D::AxonConstIterator endAxon = nrn.AxonEnd();

			for(;itAxon!=endAxon;++itAxon,++treeCount)
			{
				const std::vector<SamplePoint>& samples = (*(*itAxon).root()).m_samples;
				if(samples.size() > 0)
				{
					const float xAxon = samples[0].x;
					const float yAxon = samples[0].y;
					const float zAxon = samples[0].z;

					if(	fabs(xAxon - xSoma) > 100.0f /*xSomaRadius * 2.0f*/ ||
						fabs(yAxon - ySoma) > 100.0f /*ySomaRadius * 2.0f*/ ||
						fabs(zAxon - zSoma) > 100.0f /*zSomaRadius * 2.0f*/ )
					{
						NeuronFaultData data = { kTreeTrunkTooDistant, treeCount, 0, 0, samples[0] };
						faults.Add(data);
					}
				}
			}

			Neuron3D::DendriteConstIterator itDend = nrn.DendriteBegin();
			Neuron3D::DendriteConstIterator endDend = nrn.DendriteEnd();

			for(;itDend!=endDend;++itDend,++treeCount)
			{
				const std::vector<SamplePoint>& samples = (*(*itDend).root()).m_samples;
				if(samples.size() > 0)
				{
					const float xDend = samples[0].x;
					const float yDend = samples[0].y;
					const float zDend = samples[0].z;

					if(	fabs(xDend - xSoma) > 100.0f /*xSomaRadius * 2.0f*/ ||
						fabs(yDend - ySoma) > 100.0f /*ySomaRadius * 2.0f*/ ||
						fabs(zDend - zSoma) > 100.0f /*zSomaRadius * 2.0f*/ )
					{
						NeuronFaultData data = { kTreeTrunkTooDistant, treeCount, 0, 0, samples[0] };
						faults.Add(data);
					}
				}
			}
		}
	}
}

struct AccumulatorNeuronStats
{
	const Neuron3D&							m_rNeuron;

	NeuronStatisticsBasic&					m_statistics;
	u32										m_treeId;
	u32										m_somaId;

	AccumulatorNeuronStats(const Neuron3D& neuron, NeuronStatisticsBasic& stats) : m_rNeuron(neuron), m_statistics(stats)
	{
		m_treeId = 0;
	}

	void operator() (Neuron3D::DendriteConstIterator it)
	{
		AccumulatorTreeStatisticsBasic	treeStatsAccumulator;

		// efficient - only have to parse the tree once to calculate all statistics and fault data.
		m_rNeuron.DoForDendriteElements( (*it).root(), treeStatsAccumulator );

		// extract stats
		TreeStatisticsBasic stats;
		treeStatsAccumulator.Extract(stats);
		m_statistics.perTreeStats.push_back(stats);

		++m_treeId;
	}

	void operator() (Neuron3D::SomaConstIterator sit)
	{
		AccumulatorSomaStatistics	somaStatsAccumulator;
		somaStatsAccumulator(sit);

		SomaStatisticsBasic stats;
		somaStatsAccumulator.Extract(stats);
		m_statistics.perSomaStats.push_back(stats);

		++m_somaId;
	}

	void Finish()
	{
		CalculateGlobalNeuronStatistics(m_rNeuron, m_statistics);
	}
};

struct AccumulatorNeuronFaults
{
	const Neuron3D&							m_rNeuron;

	MorphologyDataFaults&					m_faults;
	u32										m_treeId;

	AccumulatorNeuronFaults(const Neuron3D& neuron, MorphologyDataFaults& faults) : m_rNeuron(neuron), m_faults(faults)
	{
		m_treeId = 0;
	}

	void operator() (Neuron3D::DendriteConstIterator it)
	{
		AccumulatorTreeFaults			treeFaultsAccumulator(m_faults, m_treeId);

		m_rNeuron.DoForDendriteElements( (*it).root(), treeFaultsAccumulator );

		++m_treeId;
	}

	void operator() (Neuron3D::SomaConstIterator)
	{
		//AccumulatorSomaFaults		somaFaultsAccumulator;
	}

	void Finish()
	{
		CalculateGlobalNeuronFaults(m_rNeuron, m_faults);
	}
};


struct AccumulatorNeuronStatsAndFaults
{
	const Neuron3D&							m_rNeuron;
	NeuronStatisticsBasic&					m_statistics;
	MorphologyDataFaults&					m_faults;
	u32										m_treeId;
	u32										m_somaId;

	AccumulatorNeuronStatsAndFaults(const Neuron3D& neuron, NeuronStatisticsBasic& stats, MorphologyDataFaults& faults) : 
		m_rNeuron(neuron), 
		m_statistics(stats), 
		m_faults(faults),
		m_treeId(0)
	{
	}

	// override
	void operator() (Neuron3D::DendriteConstIterator it)
	{
		AccumulatorTreeStatisticsBasic	treeStatsAccumulator;
		AccumulatorTreeFaults			treeFaultsAccumulator(m_faults, m_treeId);

		// efficient - only have to parse the tree once to calculate all statistics and fault data.
		m_rNeuron.DoForDendriteElements( (*it).root(), Neuron3D::DendriteTree::multiple_functor<AccumulatorTreeStatisticsBasic, AccumulatorTreeFaults>(treeStatsAccumulator, treeFaultsAccumulator) );

		// extract stats
		TreeStatisticsBasic stats;
		treeStatsAccumulator.Extract(stats);
		m_statistics.perTreeStats.push_back(stats);

		++m_treeId;
	}

	void operator() (Neuron3D::SomaConstIterator sit)
	{
		AccumulatorSomaStatistics	somaStatsAccumulator;
		somaStatsAccumulator(sit);

		SomaStatisticsBasic stats;
		somaStatsAccumulator.Extract(stats);
		m_statistics.perSomaStats.push_back(stats);

		++m_somaId;

		//AccumulatorSomaFaults		somaFaultsAccumulator;
	}

	void Finish()
	{
		CalculateGlobalNeuronStatistics(m_rNeuron, m_statistics);
		CalculateGlobalNeuronFaults(m_rNeuron, m_faults);
	}
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//
// API
//

void GenerateStatisticsBasic(const Neuron3D& nrn, NeuronStatisticsBasic& stats)
{
	stats.Clear();

	AccumulatorNeuronStats dataAccumulator(nrn, stats);

	nrn.DoForAllTrees(dataAccumulator);
	nrn.DoForAllSomas(dataAccumulator);

	dataAccumulator.Finish();
}

void GenerateFaults(const Neuron3D& nrn, MorphologyDataFaults& faults)
{
	faults.Clear();

	AccumulatorNeuronFaults dataAccumulator(nrn, faults);

	nrn.DoForAllTrees(dataAccumulator);
	nrn.DoForAllSomas(dataAccumulator);

	dataAccumulator.Finish();
}

void GenerateStatisticsAndFaults(const Neuron3D& nrn, NeuronStatisticsBasic& stats, MorphologyDataFaults& faults)
{
	stats.Clear();
	faults.Clear();

	AccumulatorNeuronStatsAndFaults dataAccumulator(nrn, stats, faults);

	nrn.DoForAllTrees(dataAccumulator);
	nrn.DoForAllSomas(dataAccumulator);

	dataAccumulator.Finish();
}
