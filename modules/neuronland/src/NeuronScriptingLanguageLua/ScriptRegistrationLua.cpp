//
// ScriptRegistrationLua.cpp
//

#include <Windows.h>

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
#include <luabind/wrapper_base.hpp>
#include <luabind/adopt_policy.hpp>

using namespace luabind;

#include "NeuronMorphologyFormatImportExportScript/ScriptMorphologyHandler.h"
#include "NeuronMorphologyFormatManager/MorphologyFormatManager.h"

#include "NeuronApplicationLogic/ApplicationLogic.h"

/**
	Need to do this to allow Lua classes to derive properly, and implement virtual functions
*/

class ScriptMorphologyHandler_WrapperLua : public luabind::wrap_base, public ScriptMorphologyHandler
{
public:
	ScriptMorphologyHandler_WrapperLua(MorphologyFormatID id, String name, Version version) : ScriptMorphologyHandler(id, name, version) {}

	// wrap 
	virtual bool				Import(Input& rIn, NeuronType& nrn) const												{ return call<bool>("Import", rIn, nrn); }
	virtual bool				Export(Output& rOut, const NeuronType& nrn) const										{ return call<bool>("Export", rOut, nrn); }
	virtual ValidationResult 	Validate(Input& rIn) const																{ return call<ValidationResult>("Validate", rIn); }
	virtual bool 				IsImportable() const																	{ return call<bool>("IsImportable"); }
	virtual bool 				IsExportable() const																	{ return call<bool>("IsExportable"); }
	virtual bool 				IsValidatable() const																	{ return call<bool>("IsValidatable"); }

	// defaults
	static bool					DefaultImport(ScriptMorphologyHandler* basePtr, Input& rIn, NeuronType& nrn)			{ return basePtr->ScriptMorphologyHandler::Import(rIn, nrn); }
	static bool					DefaultExport(ScriptMorphologyHandler* basePtr, Output& rOut, const NeuronType& nrn)	{ return basePtr->ScriptMorphologyHandler::Export(rOut, nrn); }
	static ValidationResult 	DefaultValidate(ScriptMorphologyHandler* basePtr, Input& rIn)							{ return basePtr->ScriptMorphologyHandler::Validate(rIn); }
	static bool 				DefaultIsImportable(ScriptMorphologyHandler* basePtr)									{ return basePtr->ScriptMorphologyHandler::IsImportable(); }
	static bool 				DefaultIsExportable(ScriptMorphologyHandler* basePtr)									{ return basePtr->ScriptMorphologyHandler::IsExportable(); }
	static bool 				DefaultIsValidatable(ScriptMorphologyHandler* basePtr)									{ return basePtr->ScriptMorphologyHandler::IsValidatable(); }
};


static void RegisterScriptMorphologyHandler_Lua(ScriptMorphologyHandler* pHandler)
{
	g_pMorphologyFormatManager->RegisterScript(pHandler);
}

static void MyPrint(const char* str)
{
	OutputDebugString(str);
}

static Neuron3D& GetNeuron(int id)
{
	return *g_applicationLogic.GetMorphologyDataManager().GetNeuron3D(id);
}

void AddScriptRegistrationAPIToLua(lua_State* L)
{
	module(L)
	[
		def("GetNeuron", &GetNeuron),

		class_<String>("String")
			.def(constructor<const char*>()),

		class_<HString>("HString")
			.def(constructor<const char*>()),

		class_<Version>("Version")
			.def(constructor<String>()),

		class_<Input>("Input")
			.def("ReadLine", &Input::readLine)
			.def("Remaining", &Input::remaining),

		class_<Output>("Output")
			.def("WriteLine", &Output::writeLine),

		class_<MorphologyFormatID>("MorphologyFormatID")
			.def(constructor<HString>()),

		class_<ScriptMorphologyHandler, ScriptMorphologyHandler_WrapperLua>("ScriptMorphologyHandler")
			.def(constructor<MorphologyFormatID, String, Version>())
			.def("Import", &ScriptMorphologyHandler::Import, &ScriptMorphologyHandler_WrapperLua::DefaultImport)
			.def("Export", &ScriptMorphologyHandler::Export, &ScriptMorphologyHandler_WrapperLua::DefaultExport)
			.def("Validate", &ScriptMorphologyHandler::Validate, &ScriptMorphologyHandler_WrapperLua::DefaultValidate)
			.def("IsImportable", &ScriptMorphologyHandler::IsImportable, &ScriptMorphologyHandler_WrapperLua::DefaultIsImportable)
			.def("IsExportable", &ScriptMorphologyHandler::IsExportable, &ScriptMorphologyHandler_WrapperLua::DefaultIsExportable)
			.def("IsValidatable", &ScriptMorphologyHandler::IsValidatable, &ScriptMorphologyHandler_WrapperLua::DefaultIsValidatable)
			.enum_("ValidationResult")
			[
				value("kValidationFalse", kValidationFalse),
				value("kValidationTrue", kValidationTrue)
			],

		def("RegisterScriptMorphologyHandler", &RegisterScriptMorphologyHandler_Lua, adopt(_1) ),

		def("OutputDebugString", MyPrint)

	];
}

