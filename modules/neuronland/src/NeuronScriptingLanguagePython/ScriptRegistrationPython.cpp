//
// ScriptRegistrationPython.cpp
//

// TODO - investigate Py++ fro auto-generation of binding code.. (and investigate how to adapt it for Luabind too)


#include <memory>
#include <string>

//#include "python.h"
#include <boost/python/detail/wrap_python.hpp>


#include <boost/noncopyable.hpp>

#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/call.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/def.hpp>
#include <boost/python/override.hpp>
#include <boost/python/cast.hpp>
#include <boost/python/implicit.hpp>


using namespace boost::python;

#include "NeuronMorphologyFormatImportExportScript/ScriptMorphologyHandler.h"
#include "NeuronMorphologyFormatManager/MorphologyFormatManager.h"

#include <Windows.h>


/**
	Need to do this to allow Python classes to derive properly, and implement virtual functions
*/


struct ScriptMorphologyHandler_Wrapper : ScriptMorphologyHandler, wrapper<ScriptMorphologyHandler>
{
	//ScriptMorphologyHandler_Wrapper() {}
	ScriptMorphologyHandler_Wrapper(MorphologyFormatID id, String name, Version version) : ScriptMorphologyHandler(id, name, version) {}

	ScriptMorphologyHandler_Wrapper( PyObject* self_, MorphologyFormatID id, String name, Version version) : ScriptMorphologyHandler(id, name, version), self(self_) { Py_IncRef(self); }
	ScriptMorphologyHandler_Wrapper( PyObject* self_, const ScriptMorphologyHandler& copy ) : ScriptMorphologyHandler(copy), self(self_) { Py_IncRef(self); }
	~ScriptMorphologyHandler_Wrapper() { Py_DecRef(self); }

	PyObject *self;

	// wrap 
	virtual bool				Import(Input& rIn, NeuronType& nrn) const					{ if (override Import = this->get_override("Import")) return Import(rIn, nrn); return ScriptMorphologyHandler::Import(rIn, nrn); }
	virtual bool				Export(Output& rOut, const NeuronType& nrn) const			{ if (override Export = this->get_override("Export")) return Export(rOut, nrn); return ScriptMorphologyHandler::Export(rOut, nrn);  }
	virtual ValidationResult 	Validate(Input& rIn) const									{ if (override Validate = this->get_override("Validate")) return Validate(rIn); return ScriptMorphologyHandler::Validate(rIn);  }
	virtual bool 				IsImportable() const										{ if (override IsImportable = this->get_override("IsImportable")) return IsImportable(); return ScriptMorphologyHandler::IsImportable(); }
	virtual bool 				IsExportable() const										{ if (override IsExportable = this->get_override("IsExportable")) return IsExportable(); return ScriptMorphologyHandler::IsExportable(); }
	virtual bool 				IsValidatable() const										{ if (override IsValidatable = this->get_override("IsValidatable")) return IsValidatable(); return ScriptMorphologyHandler::IsValidatable(); }

	// defaults
	bool						DefaultImport(Input& rIn, NeuronType& nrn) const			{ return ScriptMorphologyHandler::Import(rIn, nrn); }
	bool						DefaultExport(Output& rOut, const NeuronType& nrn) const	{ return ScriptMorphologyHandler::Export(rOut, nrn); }
	ValidationResult 			DefaultValidate(Input& rIn) const							{ return ScriptMorphologyHandler::Validate(rIn); }
	bool 						DefaultIsImportable() const									{ return ScriptMorphologyHandler::IsImportable(); }
	bool 						DefaultIsExportable() const									{ return ScriptMorphologyHandler::IsExportable(); }
	bool 						DefaultIsValidatable() const								{ return ScriptMorphologyHandler::IsValidatable(); }
};


static void RegisterScriptMorphologyHandler_Python(ScriptMorphologyHandler* pHandler)
{
	g_pMorphologyFormatManager->RegisterScript(pHandler);
}

static void RegisterScriptMorphologyHandler_Caller(auto_ptr<ScriptMorphologyHandler> obj)
{
	RegisterScriptMorphologyHandler_Python(obj.get());
	obj.release();
}

static void MyPrint(const char* str)
{
	OutputDebugStringA( (LPCSTR) str);
}

BOOST_PYTHON_MODULE_INIT(ScriptRegistration)
{
	class_<String>("String", init<const char*>());

	class_<HString>("HString", init<const char*>());

	class_<Version>("Version", init<String>());

	class_<Input>("Input", no_init)
		.def("ReadLine", &Input::readLine)
		.def("Remaining", &Input::remaining);

	class_<Output>("Output", no_init)
		.def("WriteLine", &Output::writeLine);

	class_<MorphologyFormatID>("MorphologyFormatID", init<HString>());

	class_<ScriptMorphologyHandler, auto_ptr<ScriptMorphologyHandler_Wrapper>/*boost::noncopyable*/>("ScriptMorphologyHandler", init<MorphologyFormatID, String, Version>())
		.def("Import", &ScriptMorphologyHandler::Import, &ScriptMorphologyHandler_Wrapper::DefaultImport)
		.def("Export", &ScriptMorphologyHandler::Export, &ScriptMorphologyHandler_Wrapper::DefaultExport)
		.def("Validate", &ScriptMorphologyHandler::Validate, &ScriptMorphologyHandler_Wrapper::DefaultValidate)
		.def("IsImportable", &ScriptMorphologyHandler::IsImportable, &ScriptMorphologyHandler_Wrapper::DefaultIsImportable)
		.def("IsExportable", &ScriptMorphologyHandler::IsExportable, &ScriptMorphologyHandler_Wrapper::DefaultIsExportable)
		.def("IsValidatable", &ScriptMorphologyHandler::IsValidatable, &ScriptMorphologyHandler_Wrapper::DefaultIsValidatable);
	
	implicitly_convertible<std::auto_ptr<ScriptMorphologyHandler_Wrapper>, std::auto_ptr<ScriptMorphologyHandler> >();

	enum_<ValidationResult>("ValidationResult")
			.value("kValidationFalse", kValidationFalse)
			.value("kValidationTrue", kValidationTrue);

	def("RegisterScriptMorphologyHandler", RegisterScriptMorphologyHandler_Caller);

	def("OutputDebugString", MyPrint);
}

void AddScriptRegistrationAPIToPython()
{
	if (PyImport_AppendInittab("ScriptRegistration", initScriptRegistration) == -1)
		throw std::runtime_error("Failed to add ScriptRegistration to the interpreter's " "builtin modules");
}

