#ifndef APPLICATIONLOGIC_HPP_
#define APPLICATIONLOGIC_HPP_

#include <algorithm>

#include "NeuronMorphologyDataManager/MorphologyDataManager.h"
#include "NeuronMorphologyFormatManager/MorphologyFormatManager.h"
#include "NeuronScriptingLanguageManager/ScriptingLanguageManager.h"

#include "NeuronMorphologyFormatImportExportBuiltIn/BuiltInMorphologyFormat.h"

/** 

	@brief Main interface to full functionality. For use by a UI

**/

class ApplicationLogic
{
public:
	// plug-in management

	// script management

	// neuron management
	void 	Initialise(const String& rootPath);
	void 	Refresh();

	String	GetRootPath() const;
	void	SetRootPath(const String& rootPath);

	bool	RunScript(const String& script);

	MorphologyDataManager::NeuronID	LoadNeuron(const String& filename);
	//bool	SaveNeuron(BuiltInMorphologyFormat format, MorphologyDataManager::NeuronID, const String& filename);
	bool	SaveNeuron(MorphologyHandlerID id, MorphologyDataManager::NeuronID, const String& filename);

	MorphologyDataManager&		GetMorphologyDataManager();
	ScriptingLanguageManager&	GetScriptingLanguageManager();

	/************************************************************************/
	/* Initialize components display data with default values
	/************************************************************************/
	void CreateNeuronDisplayData(MorphologyDataManager::NeuronID aId );

	
#if 1
	BuiltInMorphologyFormat mLastLoadedNeuronFormat;

	// @TODO
	bool		GetNeuronTreeStatistics(MorphologyDataManager::NeuronID, int treeId, TreeStatisticsBasic& treeStatisticsBasic);
	bool		GetNeuronStatistics(MorphologyDataManager::NeuronID, NeuronStatisticsBasic&);
	bool GetNeuronWarnings(MorphologyDataManager::NeuronID aNeuronId, MorphologyDataFaults& faults	);
	bool GetNeuronWarnings(MorphologyDataManager::NeuronID aNeuronId, int treeId, NeuronFaultData& treeFaults);
	// if you want both stats and faults, it is more efficient to call this
	void GetStatisticsAndFaults(MorphologyDataManager::NeuronID aNeuronId, NeuronStatisticsBasic& stats, MorphologyDataFaults& faults);

	String GetLastNeuronLoadedFormatString() const {return GetBuiltInMorphologyFormatDetails(mLastLoadedNeuronFormat).GetName();}

#endif

	//RegisterHandler(nttEVENT_Neuron_Modified, );

	// would like to get a signal when files in a plug-in or script directory change

private:
	//ScriptingLanguageManager			m_scriptingLanguageManager;
	//MorphologyFormatManager			m_morphologyFormatManager;
	//MorphologyGeneratorManager		m_morphologyGeneratorManager;
	//NeuronDatabaseManager				m_neuronDatabaseManager;
	//NeuronVisualisationManager		m_neuronVisualisationManager;
	//ScriptManager						m_scriptManager;
	//NeuronMorphologyInstanceManager	m_morphologyInstanceManager;

	String m_rootPath;

	//Neuron3D m_neuron;
	MorphologyDataManager m_morphologyDataManager;
};

extern ApplicationLogic g_applicationLogic;


#endif // APPLICATIONLOGIC_HPP_

