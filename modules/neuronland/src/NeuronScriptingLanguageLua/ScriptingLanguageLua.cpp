//
//
//

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luagl.h"
}

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>

#include "Core/Debug.h"

#include "NeuronMorphologyBuilderLua/MorphologyBuilderLua.h"

#include "ScriptRegistrationLua.h"
#include "ScriptingLanguageLua.h"
#include "GraphicalUserInterfaceLua.h"


using namespace luabind;

lua_State* g_L = 0;


static int luaFatalHandler (lua_State *L)
{
	static Strings deathMsg;
	deathMsg += lua_tostring (L, -1);
	fatal( deathMsg.join("\n") );
	return (0);
}

static void InitialiseLua()
{
	g_L = lua_open();

	report("Initialising Lua");

	lua_atpanic (g_L, luaFatalHandler);

	luaopen_base	(g_L);
	luaopen_string	(g_L);
	luaopen_math	(g_L);
	//	luaopen_io		(g_L); // crashes !?
	luaopen_table	(g_L);
	luaopen_debug	(g_L);

	luaopen_luagl	(g_L);

	luabind::open(g_L);
}

ScriptingLanguageLua::ScriptingLanguageLua()
{
}


void ScriptingLanguageLua::RegisterScript(StringRef script)
{
	RunScript(script);
}

void ScriptingLanguageLua::RunScript(StringRef script)
{
	if(!IsLuaInitialised())
	{
		InitialiseLua();
		AddMorphologyBuilderAPIToLua(g_L);
		AddScriptRegistrationAPIToLua(g_L);
		AddGuiAPIToLua(g_L);
	}

	luaL_dofile(g_L, (const char*) String(script));
}

bool ScriptingLanguageLua::IsValidExtension(StringRef extension) const
{
	return	extension == "lua" ||
			extension == "lub";
}

bool ScriptingLanguageLua::IsLuaInitialised() const
{
	return g_L != 0;
}

void ScriptingLanguageLua::Initialise()
{
}

void ScriptingLanguageLua::Shutdown()
{
}

void ScriptingLanguageLua::ValidateScript(StringRef script)
{
}
