#ifndef SCRIPTINGLANGUAGEMANAGER_HPP_
#define SCRIPTINGLANGUAGEMANAGER_HPP_

#include <map>

#include "Core/String.hpp"
#include "NeuronPlugInSupportScriptingLanguage/ScriptingLanguagePlugInInterface.h"

class ScriptingLanguagePlugInInterface;
typedef ScriptingLanguagePlugInInterface ScriptingLanguage;

/**

**/

class ScriptingLanguageManager
{
public:

//	typedef				HString ScriptID;

						ScriptingLanguageManager();

	static void			Initialise();

	void				Scan(const String& rootPath);
	ScriptingLanguage*	GetScriptingLanguageForFile(const String& extension) const;
	bool 				IsScriptSupported(const String& scriptFile) const;

	void				RegisterScript(const String& scriptFile);
	bool 				RunScript(const String& scriptFile);

private:
	typedef std::map<String, ScriptingLanguage*> ScriptingLanguageMap;
	ScriptingLanguageMap m_scriptingLanguages;

	void RegisterFormatHandlersBuiltIn();
	void RegisterFormatHandlersPlugIn(const String& rootPath);

//	typedef std::map<ScriptID, Script> ScriptMap;
//	ScriptMap m_registeredScripts;
};

extern ScriptingLanguageManager* g_pScriptingLanguageManager;

#endif // SCRIPTINGLANGUAGEMANAGER_HPP_
