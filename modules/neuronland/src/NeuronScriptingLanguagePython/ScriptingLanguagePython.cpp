//
//
//

//#include "python.h"
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python/exec.hpp>

#include "NeuronMorphologyBuilderPython/MorphologyBuilderPython.h"

#include "ScriptRegistrationPython.h"
#include "ScriptingLanguagePython.h"


//#include <wx/wxPython/wxPython.h>


ScriptingLanguagePython::ScriptingLanguagePython()
{
}

void ScriptingLanguagePython::RegisterScript(StringRef script)
{
	RunScript(script);
}

void ScriptingLanguagePython::RunScript(StringRef script)
{
	Initialise();
	assert(Py_IsInitialized() > 0);

	String pyString = "execfile('" + script + "')";

	int result = PyRun_SimpleString( pyString );
}

bool ScriptingLanguagePython::IsValidExtension(StringRef extension) const
{
	return	extension == "py";
}

bool ScriptingLanguagePython::IsPythonInitialised() const
{
	return Py_IsInitialized();
}

PyThreadState* m_mainTState;

void ScriptingLanguagePython::Initialise()
{
	if(!Py_IsInitialized())
	{
		Py_Initialize();
/*		PyEval_InitThreads();

		// Load the wxPython core API.  Imports the wx._core_ module and sets a
		// local pointer to a function table located there.  The pointer is used
		// internally by the rest of the API functions.
		if ( ! wxPyCoreAPI_IMPORT() ) {
			wxLogError(wxT("***** Error importing the wxPython API! *****"));
			PyErr_Print();
			Py_Finalize();
			return;
		}        

		// Save the current Python thread state and release the
		// Global Interpreter Lock.
		m_mainTState = wxPyBeginAllowThreads();
		*/

		AddMorphologyBuilderAPIToPython();
		AddScriptRegistrationAPIToPython();
	}
}

void ScriptingLanguagePython::Shutdown()
{
	if(Py_IsInitialized())
	{
		// Restore the thread state and tell Python to cleanup after itself.
		// wxPython will do its own cleanup as part of that process.  This is done
		// in OnExit instead of ~MyApp because OnExit is only called if OnInit is
		// successful.
		//wxPyEndAllowThreads(m_mainTState);

		Py_Finalize();
	}
}

void ScriptingLanguagePython::ValidateScript(StringRef script)
{
}


