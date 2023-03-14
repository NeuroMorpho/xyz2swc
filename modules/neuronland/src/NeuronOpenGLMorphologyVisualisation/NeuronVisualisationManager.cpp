#if 0
//
//
//
#include <algorithm>

#include "boost/filesystem/operations.h"
#include "boost/filesystem/path.h"

namespace fs = std::experimental::filesystem;

#include "NeuronVisualisationHandler.h"

#include "NeuronVisualisationManager.h"

NeuronVisualisationManager::NeuronVisualisationManager()
{
}

void NeuronVisualisationManager::Scan(const String& rootPath)
{
	// for each source..registerFunctionality(FunctionalityType)

	RegisterVisualisationHandlerBuiltIn();
	RegisterVisualisationHandlerPlugIn(rootPath + "PlugIns/Visualisation/");
	RegisterVisualisationHandlerScript(rootPath + "Scripts/Visualisation/");
}


NeuronVisualisationBase* Create(NeuronVisualisationID id)
{
	return m_visualisationFactory[id].Create();
}

void NeuronVisualisationManager::RegisterVisualisationHandlerBuiltIn()
{
	for(u32 formatBI=0; formatBI<kMorphologyFormatBuiltInNumber; ++formatBI)
	{
		const BuiltInMorphologyFormat			builtInFormat = (BuiltInMorphologyFormat)formatBI;
		//const MorphologyFormatDetails&			formatDetails( GetBuiltInMorphologyFormatDetails(builtInFormat) );

		MorphologyFormatID						formatId = MorphologyFormatBuiltIn2ID(builtInFormat);

		if(IsMorphologyFormatIDRegistered(formatId))
		{
			const MorphologyFormatDetails&			formatDetails( GetMorphologyFormatDetails(formatId) );
			const MorphologyFormatHandlerDetails	handlerDetails(GetBuiltInMorphologyFormatHandlerDetails(builtInFormat));
			MorphologyFormatHandler*				pHandler = new HardCodedMorphologyHandler(handlerDetails, GetBuiltInMorphologyImportFunction(builtInFormat), GetBuiltInMorphologyExportFunction(builtInFormat), GetBuiltInMorphologyValidateFunction(builtInFormat));

			MorphologyHandlerID						handlerId = MorphologyHandlerID( pHandler->GetDetails().GetHandlerName() );

			AddMorphologyFormatHandler(handlerId, pHandler);
		}
	}

	AddVisualisationHandler()
}

void NeuronVisualisationManager::RegisterVisualisationHandlerPlugIn(const String& rootPath)
{
	m_plugIns.Scan(rootPath);

	MorphologyFormatPlugInManager::const_iterator it = m_plugIns.begin();
	MorphologyFormatPlugInManager::const_iterator end = m_plugIns.end();

	for(;it != end;++it)
	{
		MorphologyFormatHandler* pHandler = (*it);
		const MorphologyFormatHandlerDetails handlerDetails = pHandler->GetDetails();
		const MorphologyFormatID formatId = handlerDetails.GetMorphologyFormat();

		if(IsMorphologyFormatIDRegistered(formatId))
		{
			//const MorphologyFormatDetails&			formatDetails = GetMorphologyFormatDetails(formatId);
			MorphologyHandlerID						handlerId = MorphologyHandlerID( pHandler->GetDetails().GetHandlerName() );

			if(!IsMorphologyHandlerIDRegistered(handlerId))
			{
				AddMorphologyFormatHandler(handlerId, pHandler);
			}
			else
			{
				report("Handler name already exists - " + pHandler->GetDetails().GetHandlerName());
			}
		}
		else
		{
			// should we try and keep converters for non-registered formats ? - see if they have the data embedded ?
		}
	}
}

void NeuronVisualisationManager::RegisterVisualisationHandlerScript(const String& rootPath)
{
	m_scripts.Scan(rootPath);

	// NOTE - significant duplication of plugin registration code - need to clean it up!
	MorphologyFormatScriptManager::const_iterator it = m_scripts.begin();
	MorphologyFormatScriptManager::const_iterator end = m_scripts.end();

	for(;it != end;++it)
	{
		MorphologyFormatHandler* pHandler = (*it);
		const MorphologyFormatHandlerDetails handlerDetails = pHandler->GetDetails();
		const MorphologyFormatID formatId = handlerDetails.GetMorphologyFormat();

		if(IsMorphologyFormatIDRegistered(formatId))
		{
			//const MorphologyFormatDetails&			formatDetails = GetMorphologyFormatDetails(formatId);
			MorphologyHandlerID						handlerId = MorphologyHandlerID( pHandler->GetDetails().GetHandlerName() );

			if(!IsMorphologyHandlerIDRegistered(handlerId))
			{
				AddMorphologyFormatHandler(handlerId, pHandler);
			}
			else
			{
				report("Handler name already exists - " + pHandler->GetDetails().GetHandlerName());
			}
		}
		else
		{
			// should we try and keep converters for non-registered formats ? - see if they have the data embedded ?
		}
	}
}


#endif