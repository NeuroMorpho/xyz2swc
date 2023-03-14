#ifndef MORPHOLOGYDATAMANAGER_HPP_
#define MORPHOLOGYDATAMANAGER_HPP_

#include <map>
#include <vector>

#include "Core/SimpleIncrementIdGenerator.h"
#include "Core/ObjectManager.h"

#include "NeuronRepresentation/Neuron3D.h"

#include "NeuronMorphologyStatistics/NeuronStatisticsBasic.h"
#include "NeuronMorphologyStatistics/MorphologyDataFaults.h"


struct NeuronLog
{
};

struct NeuronData
{
	Neuron3D				neuron;
	NeuronStatisticsBasic	stats;
	NeuronLog				log;
};

class MorphologyDataManager : public ObjectManager<NeuronData, SimpleIncrementIdGenerator>
{
public:
	typedef typename SimpleIncrementIdGenerator::id_type NeuronID;

	NeuronID						DuplicateNeuron(NeuronID);

	Neuron3D*						GetNeuron3D(NeuronID id)				{ return &GetObject(id)->neuron; }
	const NeuronStatisticsBasic*	GetNeuronStatistics(NeuronID id) const	{ return &GetObject(id)->stats; }
	const NeuronLog*				GetNeuronLog(NeuronID id)const			{ return &GetObject(id)->log; }
	const MorphologyDataFaults*		GetNeuronFaults(NeuronID id) const		{ return (m_faults.find(id) != m_faults.end()) ? &(m_faults.find(id)->second) : 0; }

	bool							DoesNeuronHaveFaults(NeuronID id) const	{ return GetNeuronFaults(id) != 0; }

	void							CalculateNeuronData(NeuronID id);

//	void							ResolveFault(NeuronID neuronId, FaultId faultId, FaultResolver& resolver);

private:

	std::map<NeuronID, MorphologyDataFaults> m_faults;
};


#endif // MORPHOLOGYDATAMANAGER_HPP_

