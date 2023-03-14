//
//
//

#include <stdlib.h>
#include <float.h>

#include "MorphologyDataManager.h"
#include "NeuronMorphologyStatistics/MorphologyStatistics.h"

void MorphologyDataManager::CalculateNeuronData(MorphologyDataManager::NeuronID neuronId)
{
	NeuronData& rNeuronData = *GetObject(neuronId);

	MorphologyDataFaults faults;
	rNeuronData.stats.Clear();

	GenerateStatisticsAndFaults(rNeuronData.neuron, rNeuronData.stats, faults);
}

