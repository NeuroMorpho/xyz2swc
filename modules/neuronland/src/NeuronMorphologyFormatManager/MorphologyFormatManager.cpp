//
//
//
#include <algorithm>
#include <filesystem>

#include "Core/InputFile.h"

#include "NeuronMorphologyFormat/MorphologyFormatHandler.h"

#include "NeuronMorphologyFormatImportExportBuiltIn/HardCodedMorphologyHandler.h"
#include "NeuronMorphologyFormatImportExportBuiltIn/BuiltInMorphologyFormat.h"
#include "NeuronMorphologyFormatImportExportBuiltIn/MorphologyFormatImportExport.h"

#include "MorphologyFormatManager.h"

//temp
bool hasEnding(const String& fullString, const String& ending)
{
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}

#if defined(__linux__)
namespace fs = std::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif


MorphologyFormatManager::MorphologyFormatManager()
{
}

void MorphologyFormatManager::Scan(const String& rootPath)
{
	RegisterFormatDetails(rootPath + "Data/MorphologyFormats/");

	RegisterFormatHandlersBuiltIn();
	RegisterFormatHandlersPlugIn(rootPath + "PlugIns/MorphologyFormats/");
	RegisterFormatHandlersScript(rootPath + "Scripts/MorphologyFormats/");

	OutputStatus();
}

// @@@todo generalise - perform X operation on Y files where filename satisfied Z condition
void MorphologyFormatManager::RegisterFormatDetails(const String& rootPath)
{
	String filename(rootPath);
	const char* filePath = filename.c_str();

	if ( fs::is_directory(filePath) )
	{
		fs::directory_iterator end_iter;
		fs::directory_iterator dir_itr(filePath);

		for(; dir_itr != end_iter; ++dir_itr )
		{
			if ( fs::is_regular_file( dir_itr->status() ) )
			{
				const String filename = dir_itr->path().string().c_str();
				if(hasEnding(filename, ".nmf"))
				{
					Input* pIn = InputFile::open( dir_itr->path().string().c_str() );
					if(pIn)
					{
						MorphologyFormatDetails		formatDetails(*pIn);
						MorphologyFormatID			formatId(formatDetails.GetName());

						AddMorphologyFormatDetails(formatId, formatDetails);
					}
				}
			}
		}
	}
}

void MorphologyFormatManager::RegisterFormatHandlersBuiltIn()
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
}

void MorphologyFormatManager::RegisterFormatHandlersPlugIn(const String& rootPath)
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

void MorphologyFormatManager::RegisterFormatHandlersScript(const String& rootPath)
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

bool MorphologyFormatManager::AddMorphologyFormatDetails(MorphologyFormatID id, const MorphologyFormatDetails& details)
{
//	if(std::find(m_formatDetails.begin(), m_formatDetails.end(), id) != m_formatDetails.end())
//		return false;

	m_formatDetails[id] = details;

	return true;
}

bool MorphologyFormatManager::AddMorphologyFormatHandler(MorphologyHandlerID id, MorphologyFormatHandler* pHandler)
{
//	if(std::find(m_formatHandlers.begin(), m_formatHandlers.end(), id) != m_formatHandlers.end())
//		return false;

	m_formatHandlers[id] = pHandler;

	return true;
}

bool MorphologyFormatManager::IsMorphologyFormatIDRegistered(MorphologyFormatID formatId) const
{
	return m_formatDetails.find(formatId) != m_formatDetails.end();
}

bool MorphologyFormatManager::IsMorphologyHandlerIDRegistered(MorphologyHandlerID handlerId) const
{
	return m_formatHandlers.find(handlerId) != m_formatHandlers.end();
}

void MorphologyFormatManager::RegisterScript(ScriptMorphologyHandler* pHandler)
{
	m_scripts.RegisterScript(pHandler);
}

void MorphologyFormatManager::GetMorphologyFormatIDs(MorphologyFormatManager::MorphologyFormatIDVector& formatIds) const
{
	formatIds.clear();

	FormatDetailsMap::const_iterator it = m_formatDetails.begin();
	FormatDetailsMap::const_iterator end = m_formatDetails.end();

	for(;it != end;++it)
		formatIds.push_back( it->first );
}

void MorphologyFormatManager::GetMorphologyHandlerIDs(MorphologyFormatManager::MorphologyHandlerIDVector& handlerIds, MorphologyFormatManager::FormatActions actions) const
{
	handlerIds.clear();

	FormatHandlerPtrMap::const_iterator it = m_formatHandlers.begin();
	FormatHandlerPtrMap::const_iterator end = m_formatHandlers.end();

	for(;it != end;++it)
	{
		handlerIds.push_back( it->first );
	}
}

void MorphologyFormatManager::GetMorphologyHandlerIDsForFormatID(MorphologyFormatID id, MorphologyFormatManager::MorphologyHandlerIDVector& handlerIds, MorphologyFormatManager::FormatActions actions) const
{
	handlerIds.clear();

	FormatHandlerPtrMap::const_iterator it = m_formatHandlers.begin();
	FormatHandlerPtrMap::const_iterator end = m_formatHandlers.end();

	for(;it != end;++it)
	{
		if(it->second->GetDetails().GetMorphologyFormat() == id)
			handlerIds.push_back( it->first );
	}
}

MorphologyHandlerID	MorphologyFormatManager::GetMorphologyHandlerIDForFormatID(MorphologyFormatID id, MorphologyFormatManager::FormatActions actions, MorphologyFormatManager::HandlerChooser) const
{
	MorphologyHandlerIDVector handlerIds;
	GetMorphologyHandlerIDsForFormatID(id, handlerIds, actions);

	assert(handlerIds.size() > 0);
	return handlerIds[0];
}

const MorphologyFormatDetails&	MorphologyFormatManager::GetMorphologyFormatDetails(MorphologyFormatID formatId) const
{
	return m_formatDetails.find(formatId)->second;
}

const MorphologyFormatHandlerDetails& MorphologyFormatManager::GetMorphologyFormatHandlerDetails(MorphologyHandlerID handlerId) const
{
	return m_formatHandlers.find(handlerId)->second->GetDetails();
}


void MorphologyFormatManager::OutputStatus()
{
	// formats
	{
		FormatDetailsMap::iterator it = m_formatDetails.begin();
		FormatDetailsMap::iterator end = m_formatDetails.end();

		report("Supported Formats - ");
		u32 count = 1;
//		for(;it != end;++it, ++count)
//			report(dec(count) + ": " + (*it).second.GetName().c_str());
	}

	// handlers
	{
		FormatHandlerPtrMap::iterator it = m_formatHandlers.begin();
		FormatHandlerPtrMap::iterator end = m_formatHandlers.end();

		report("Available Handlers - ");
		u32 count = 1;
//		for(;it != end;++it, ++count)
//			report(dec(count) + ": " + (*it).second->GetDetails().GetHandlerName() + "  <" + m_formatDetails.find((*it).second->GetDetails().GetMorphologyFormat())->second.GetName() + ">");
	}
}

bool MorphologyFormatManager::Export(MorphologyHandlerID id, Output& rOut, Neuron3D& nrn)
{
	MorphologyFormatHandler* pHandler = m_formatHandlers[id];
	return pHandler->Export(rOut, nrn);
}

bool MorphologyFormatManager::Import(MorphologyHandlerID id, Input& rIn, Neuron3D& nrn)
{
	return m_formatHandlers[id]->Import(rIn, nrn);
}

bool MorphologyFormatManager::Validate(MorphologyHandlerID id, Input& rIn)
{
	return (m_formatHandlers[id]->Validate(rIn) == kValidationTrue);
}

void MorphologyFormatManager::Initialise()
{
	g_pMorphologyFormatManager = new MorphologyFormatManager();
}

MorphologyFormatManager* g_pMorphologyFormatManager;
