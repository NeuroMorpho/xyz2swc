//
//
//


#include <windows.h>

#include "NeuronScriptingLanguageLua/ScriptingLanguageLua.hpp"

#include "PlugIn.hpp"


ScriptingLanguageLua sg_scriptingLanguageLuaInterface;


/**

	Plug-in functionality

**/

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	/*	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	break;
	}*/
	return TRUE;
}

PlugInDetails GetPlugInDetails()
{
	return PlugInDetails( Version( String("PlugIn(BuiltIn):1.0.0") ), String("ScriptingLanguage") );
}

const ScriptingLanguagePlugInInterface& GetScriptingLanguageInterface()
{
	return sg_scriptingLanguageLuaInterface;
}

