//
// ApplicationLogic.cpp
//

#include "Core/InputFile.h"
#include "Core/InputMem.h"
#include "Core/OutputFile.h"

#include "ApplicationLogic.h"

#include "NeuronScriptingLanguageManager/ScriptingLanguageManager.h"
#include "NeuronMorphologyFormatManager/MorphologyFormatManager.h"
#include "NeuronMorphologyFormatImportExportBuiltIn/BuiltInMorphologyFormat.h"
#include "NeuronRepresentation/Neuron3D.h"
#include "NeuronRepresentation/NeuronComponentDisplayProperties.h"

#include "NeuronMorphologyStatistics/MorphologyStatistics.h"

void ApplicationLogic::Initialise(const String& rootPath)
{
	SetRootPath(rootPath);

	/**

	 Application functionality can come from a number of sources - BuiltIn, PlugIn, Scripts, and External Applications, and possibly from a network source...
	 First		- initialize any generic support required for these sources
	 Second		- we scan for specific functionality available from these sources

	 TODO - formalise this design

	**/

	ScriptingLanguageManager::Initialise();
	MorphologyFormatManager::Initialise();
//	MorphologyGeneratorManager::Initialise();
//	MorphologyVisualisationManager::Initialise();

	// generic support for functionality sources
	g_pScriptingLanguageManager->Scan(rootPath);
	//m_externalApplicationManager.Scan(rootPath);

	// specific functionality which can come from any of the supported sources
	g_pMorphologyFormatManager->Scan(rootPath);
	//m_morphologyGeneratorManager.Scan(rootPath);
	//m_morphologyVisualisationManager.Scan(rootPath);

	// other functionality for managing application objects
	//m_morphologyInstanceManager.Initialise();

	//AddSpecialisedFunctionality(MorphologyFormatIO, morpholgyManager);
	//AddSpecialisedFunctionality(MorphologyReduction, takesNeuronAsParameter);
}

void ApplicationLogic::SetRootPath(const String& rootPath)
{
	m_rootPath = rootPath;
}

String ApplicationLogic::GetRootPath() const
{
	return m_rootPath;
}

void ApplicationLogic::Refresh()
{

}

bool ApplicationLogic::RunScript(const String& script)
{
	return g_pScriptingLanguageManager->RunScript(script);
}

MorphologyDataManager& ApplicationLogic::GetMorphologyDataManager()
{
	return m_morphologyDataManager;
}

ScriptingLanguageManager& ApplicationLogic::GetScriptingLanguageManager()
{
	assert(g_pScriptingLanguageManager);
	return *g_pScriptingLanguageManager;
}

//
//
//

MorphologyDataManager::NeuronID ApplicationLogic::LoadNeuron(const String& filename)
{
	Input* pIn = InputFile::open( filename );

// faster if we load it into memory first
	u32 size = pIn->remaining();
	u8* buffer = new u8[size];
	InputMem* pInputMem =  new InputMem(buffer, size);
	pIn->read(buffer, size);
	delete pIn;
//

//	assert(pIn.get());

	BuiltInMorphologyFormat inputFormat = kMorphologyFormatError;

	for(u32 n = 0;n < kMorphologyFormatBuiltInNumber;++n)
	{
		const ValidationOptions validationOptions;

		BuiltInMorphologyFormat format =  (BuiltInMorphologyFormat)n;
		const ValidationResult result = ValidateMorphologyFormat( format, *pInputMem, validationOptions);
		pInputMem->ToStart();

		if(result == kValidationTrue)
		{
			report("Format: <" + GetBuiltInMorphologyFormatDetails(format).GetName() + ">  - " + ValidationResultToString(result));
			inputFormat = format;
		}
	}

	if(inputFormat != kMorphologyFormatError)
	{
		MorphologyDataManager::NeuronID id = GetMorphologyDataManager().Create();
		Neuron3D* pNeuron3D = GetMorphologyDataManager().GetNeuron3D(id);

		const HintOptions hintOptions;

		bool success = false;

		if( IsFormatFullyBufferable(inputFormat) )
			success = ImportMorphologyFormat(inputFormat, *pInputMem, *pNeuron3D, hintOptions);
		else
			success = ImportRawMorphologyFormat(inputFormat, filename.c_str(), *pNeuron3D, hintOptions);

		if (success)
		{
			GetMorphologyDataManager().CalculateNeuronData(id);
			delete pInputMem;
			delete[] buffer;
			mLastLoadedNeuronFormat = inputFormat;
			return id;
		}
	}

	//
	delete pInputMem;
	delete [] buffer;
	//

	return -1;
}


void ApplicationLogic::CreateNeuronDisplayData(MorphologyDataManager::NeuronID aId )
{
	Neuron3D* pNeuron3D = GetMorphologyDataManager().GetNeuron3D(aId);
	NLNeuronDisplayElements& dipsSettings = GetNeuronElementsDisplaySettings();
	NLNeuronElementId elId=0; 
	dipsSettings.clear();
	dipsSettings.reserve(pNeuron3D->CountAxons()+pNeuron3D->CountDendrites());

	for(Neuron3D::SomaIterator it = pNeuron3D->SomaBegin();it != pNeuron3D->SomaEnd();++it)
	{			
		dipsSettings.push_back(NeuronComponentDisplayProperties(kNeuronCompSoma));
	}

	for(Neuron3D::DendriteIterator it = pNeuron3D->DendriteBegin();it != pNeuron3D->DendriteEnd();++it)
	{
		if((*it).root())
			dipsSettings.push_back( NeuronComponentDisplayProperties(kNeuronCompDendrite));			
	}

	// axons
	for(Neuron3D::AxonIterator it = pNeuron3D->AxonBegin();it != pNeuron3D->AxonEnd();++it)
	{
		if((*it).root())
			dipsSettings.push_back(NeuronComponentDisplayProperties(kNeuronCompAxon));
	}

	// markers
	if(pNeuron3D->CountAllMarkers() > 0)
		dipsSettings.push_back(NeuronComponentDisplayProperties(kNeuronCompMarkers));

	// spines
	if(pNeuron3D->CountAllSpines() > 0)
		dipsSettings.push_back(NeuronComponentDisplayProperties(kNeuronCompSpines));
}

/*
bool ApplicationLogic::SaveNeuron(BuiltInMorphologyFormat format, MorphologyDataManager::NeuronID id, const String& filename)
{
	Output* pOut = OutputFile::open(filename);

	if(pOut)
	{
		ExportMorphologyFormat(format, *pOut, *GetMorphologyDataManager().GetObject(id) );
		delete pOut;
		return true;
	}

	return false;
}
*/

bool ApplicationLogic::SaveNeuron(MorphologyHandlerID id, MorphologyDataManager::NeuronID neuronId, const String& filename)
{
	std::auto_ptr<Output> pOut( OutputFile::open(filename) );

	if(pOut.get())
	{
		g_pMorphologyFormatManager->Export(id, *pOut, *GetMorphologyDataManager().GetNeuron3D(neuronId) );
		return true;
	}

	return false;
}

bool ApplicationLogic::GetNeuronTreeStatistics(MorphologyDataManager::NeuronID neuronId, int treeId, TreeStatisticsBasic& treeStatisticsBasic)
{

	Neuron3D* pNeuron3D =GetMorphologyDataManager().GetNeuron3D(neuronId);
	NeuronStatisticsBasic aStats;
	GenerateStatisticsBasic(*pNeuron3D, aStats);
	treeStatisticsBasic = aStats.perTreeStats[treeId];
	return true;
}

bool ApplicationLogic::GetNeuronStatistics(MorphologyDataManager::NeuronID neuronId, NeuronStatisticsBasic& aStats)
{
	Neuron3D* pNeuron3D =GetMorphologyDataManager().GetNeuron3D(neuronId);
	GenerateStatisticsBasic(*pNeuron3D, aStats);
	return true;
}

void ApplicationLogic::GetStatisticsAndFaults(MorphologyDataManager::NeuronID aNeuronId, NeuronStatisticsBasic& stats, MorphologyDataFaults& faults)
{
	Neuron3D* pNeuron3D =GetMorphologyDataManager().GetNeuron3D(aNeuronId);
	GenerateStatisticsAndFaults(*pNeuron3D,stats,faults);
}


bool ApplicationLogic::GetNeuronWarnings(MorphologyDataManager::NeuronID aNeuronId, int treeId, NeuronFaultData& treeFaults	)
{
	Neuron3D* pNeuron3D =GetMorphologyDataManager().GetNeuron3D(aNeuronId);
	MorphologyDataFaults faults;
	GenerateFaults(*pNeuron3D,faults);
	treeFaults= faults.m_faults[treeId];
	return true;
}

bool ApplicationLogic::GetNeuronWarnings(MorphologyDataManager::NeuronID aNeuronId, MorphologyDataFaults& faults	)
{
	Neuron3D* pNeuron3D =GetMorphologyDataManager().GetNeuron3D(aNeuronId);

	GenerateFaults(*pNeuron3D,faults);

	return true;
}


ApplicationLogic g_applicationLogic;

