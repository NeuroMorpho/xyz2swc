#ifndef MORPHOLOGYFORMATMANAGER_HPP_
#define MORPHOLOGYFORMATMANAGER_HPP_

#include <vector>
#include <map>

#include "NeuronMorphologyFormat/MorphologyFormatID.h"
#include "NeuronMorphologyFormat/MorphologyHandlerID.h"

#include "NeuronMorphologyFormatImportExportPlugIn/MorphologyFormatPlugInManager.h"
#include "NeuronMorphologyFormatImportExportScript/MorphologyFormatScriptManager.h"

#include "NeuronMorphologyFormat/MorphologyFormatDetails.h"

using namespace std;

class MorphologyFormatHandler;

/**

	@brief a central point for querying supported formats, and available handlers for those formats.

**/

class MorphologyFormatManager
{
public:

	enum FormatActions
	{
		kExportable,
		kImportable,
		kValidatable,
		kImportValidatable,
		kImportExportable,
		kImportExportValidatable
	};

	enum HandlerChooser
	{
		kStandard,
		kLatest
	};

	typedef std::vector<MorphologyFormatID>		MorphologyFormatIDVector;
	typedef std::vector<MorphologyHandlerID>	MorphologyHandlerIDVector;

											MorphologyFormatManager();

	// setup
	static void								Initialise();

	void									Scan(const String& pathname);
	void									RegisterScript(ScriptMorphologyHandler* pHandler);
	bool									IsMorphologyFormatIDRegistered(MorphologyFormatID id) const;
	bool									IsMorphologyHandlerIDRegistered(MorphologyHandlerID id) const;

	// query
	void									GetMorphologyFormatIDs(MorphologyFormatIDVector& formatIds) const;
	void									GetMorphologyHandlerIDs(MorphologyHandlerIDVector& handlerIds, FormatActions actions) const;
	void									GetMorphologyHandlerIDsForFormatID(MorphologyFormatID id, MorphologyHandlerIDVector& handlerIds, FormatActions actions) const;
	MorphologyHandlerID						GetMorphologyHandlerIDForFormatID(MorphologyFormatID id, MorphologyFormatManager::FormatActions actions, MorphologyFormatManager::HandlerChooser) const;

	const MorphologyFormatDetails&			GetMorphologyFormatDetails(MorphologyFormatID) const;
	const MorphologyFormatHandlerDetails&	GetMorphologyFormatHandlerDetails(MorphologyHandlerID) const;

	// use
	bool									Export(MorphologyHandlerID, Output& rOut, Neuron3D& nrn);
	bool									Import(MorphologyHandlerID, Input& rOut, Neuron3D& nrn);
	bool									Validate(MorphologyHandlerID, Input& rOut);

private:

	void	RegisterFormatDetails(const String& rootPath);
	void	RegisterFormatHandlersBuiltIn();
	void	RegisterFormatHandlersPlugIn(const String& rootPath);
	void	RegisterFormatHandlersScript(const String& rootPath);

	bool	AddMorphologyFormatDetails(MorphologyFormatID id, const MorphologyFormatDetails& details);
	bool	AddMorphologyFormatHandler(MorphologyHandlerID id, MorphologyFormatHandler* pHandler);

	void	OutputStatus();


	MorphologyFormatPlugInManager		m_plugIns;
	MorphologyFormatScriptManager		m_scripts;

	// properties of the morphology formats
	typedef std::map<MorphologyFormatID, MorphologyFormatDetails> FormatDetailsMap;
	FormatDetailsMap					m_formatDetails;

	// overall source-transparent set of handlers (potentially
	typedef std::map<MorphologyHandlerID, MorphologyFormatHandler*> FormatHandlerPtrMap;
	std::map<MorphologyHandlerID, MorphologyFormatHandler*>/*FormatHandlerPtrMap*/					m_formatHandlers;
};

extern MorphologyFormatManager* g_pMorphologyFormatManager;

#endif // MORPHOLOGYFORMATMANAGER_HPP_

