//
//
//

#include <boost/bind.hpp>
#include <algorithm>

#include "NeuronPlugInSupportScriptingLanguage/ScriptingLanguagePlugInInterface.h"

#include "NeuronScriptingLanguageLua/ScriptingLanguageLua.h"
#include "NeuronScriptingLanguagePython/ScriptingLanguagePython.h"

#include "ScriptingLanguageManager.h"


ScriptingLanguageManager::ScriptingLanguageManager()
{
}

void ScriptingLanguageManager::Scan(const String& rootPath)
{
	RegisterFormatHandlersBuiltIn();
	RegisterFormatHandlersPlugIn(rootPath + "PlugIns/ScriptingLanguages/");
}

void ScriptingLanguageManager::RegisterFormatHandlersBuiltIn()
{
	// Built-in Lua and Python for the moment
	// TODO: plug-in instead (need to move some NeuroTic functionality into a DLL first)
//	m_scriptingLanguages[ String("Lua-5.1.2") ] = new ScriptingLanguageLua();
//	m_scriptingLanguages[ String("Python25") ]	= new ScriptingLanguagePython();
}

void ScriptingLanguageManager::RegisterFormatHandlersPlugIn(const String& rootPath)
{
}

void ScriptingLanguageManager::RegisterScript(const String& scriptFile)
{
	RunScript(scriptFile);
}

bool ScriptingLanguageManager::RunScript(const String& scriptFile)
{
	ScriptingLanguage* pScriptingLanguage = GetScriptingLanguageForFile(scriptFile);
	if(pScriptingLanguage)
	{
		pScriptingLanguage->RunScript(scriptFile);
		return true;
	}
	else
		return false;
}

ScriptingLanguage* ScriptingLanguageManager::GetScriptingLanguageForFile(const String& fileName) const
{
	StringRef extension = fileName.substr(fileName.find_last_of('.') + 1);
#if 0
	ScriptingLanguageMap::const_iterator it = std::find_if(	m_scriptingLanguages.begin(), 
															m_scriptingLanguages.end(),
															std::bind(&ScriptingLanguage::IsValidExtension, _1, extension) );

	return it != m_scriptingLanguages.end() ? (*it).second : 0;

#else

	// @TODO check for multiple plugins which support same extensions!

	ScriptingLanguageMap::const_iterator it = m_scriptingLanguages.begin();
	ScriptingLanguageMap::const_iterator end = m_scriptingLanguages.end();

	for(;it!=end;++it)
	{
		if( (*it).second->IsValidExtension(extension) )
			return (*it).second;
	}

	return 0;
#endif
}

bool ScriptingLanguageManager::IsScriptSupported(const String& scriptFile) const
{
	return GetScriptingLanguageForFile(scriptFile) != 0;
}


ScriptingLanguageManager* g_pScriptingLanguageManager;

void ScriptingLanguageManager::Initialise()
{
	g_pScriptingLanguageManager = new ScriptingLanguageManager;
}

