#ifndef MORPHOLOGYSTATISTICS_HPP_
#define MORPHOLOGYSTATISTICS_HPP_

#include "NeuronRepresentation/Neuron3D.h"
#include "NeuronStatisticsBasic.h"
#include "MorphologyDataFaults.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Main API for generating statistics data for a neuron
//


// Each function only requires one pass of the neuron hierarchy
void GenerateStatisticsBasic(const Neuron3D& nrn, NeuronStatisticsBasic& stats);
void GenerateFaults(const Neuron3D&, MorphologyDataFaults& faults);

// if you want both stats and faults, it is more efficient to call this
void GenerateStatisticsAndFaults(const Neuron3D& nrn, NeuronStatisticsBasic& stats, MorphologyDataFaults& faults);


#endif //MORPHOLOGYSTATISTICS_HPP_
