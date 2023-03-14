//
//
//

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
#include <luabind/wrapper_base.hpp>
#include <luabind/adopt_policy.hpp>
using namespace luabind;

#include "wx/wx.h"

#include "wxLua/include/wxlua.h"

extern bool wxLuaBinding_wxlua_init();
extern bool wxLuaBinding_wx_init();
//extern bool wxLuaBinding_wxstc_init();
//extern bool wxLuaBinding_wxluasocket_init();

#include "App_NeuroTic_wxWidgets/wxNeuronMainFrame.h"

wxLuaState* sg_pWxLuaState = 0;

static wxNeuronMainFrame& GetMainFrame()
{
	return *g_pMainFrame;
}

void AddNeuronGuiAPIToLua(struct lua_State* L)
{
	module(L)
	[
		def("GetMainFrame", &GetMainFrame),

		class_<wxNeuronMainFrame>("MainFrame")
			.def("RefreshGUI", &wxNeuronMainFrame::RefreshGUI)
	];
}

void AddGuiAPIToLua(struct lua_State* L)
{
	wxLuaBinding_wxlua_init();
	wxLuaBinding_wx_init();
	//wxLuaBinding_wxstc_init();
	//wxLuaBinding_wxluasocket_init();

	AddNeuronGuiAPIToLua(L);

	sg_pWxLuaState = new wxLuaState(L, WXLUASTATE_SETSTATE );
}
