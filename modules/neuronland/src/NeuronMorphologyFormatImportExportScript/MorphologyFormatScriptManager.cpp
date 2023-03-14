//
//
//

#include "MorphologyFormatScriptManager.h"

#if 0
#include "NeuronScriptingLanguageManager/ScriptingLanguageManager.h"
#include "NeuronMorphologyFormatImportExportScript/ScriptMorphologyHandler.h"


#include "NeuronSystemPlugIns/PlugInHandler.h"
#endif

void MorphologyFormatScriptManager::Scan(const String& location)
{
#if 0
	long				lFile = 0L;
	struct _finddata_t	sFILE;
	String				searchPath = location + "*.*";

	lFile = (long) _findfirst(searchPath.c_str(), &sFILE);

	u32 scriptCount = 0;

	if (lFile != -1L)
	{
		do
		{
			const String fileName(sFILE.name);

			ScriptingLanguage* pLanguage = g_pScriptingLanguageManager->GetScriptingLanguageForFile(fileName);

			if(pLanguage)
			{
				pLanguage->RegisterScript(location + fileName);
				//pLanguage->ExecuteScript(fileName);

				if(scriptCount+1 == size())
					++scriptCount;
				else
				{
					report("Script didnt register a morphology handler!");
					assert(0);
				}
			}
		} while (_findnext(lFile, &sFILE) == 0);

		_findclose(lFile);
	}
#endif
}


void MorphologyFormatScriptManager::RegisterScript(ScriptMorphologyHandler* pHandler)
{
#if 0
	assert(pHandler);
	push_back(pHandler);
#endif
}
