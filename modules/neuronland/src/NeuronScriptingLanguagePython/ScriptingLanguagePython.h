#ifndef SCRIPTINGLANGUAGEPYTHON_HPP_
#define SCRIPTINGLANGUAGEPYTHON_HPP_

#include "Core/String.hpp"

#include "NeuronPlugInSupportScriptingLanguage/ScriptingLanguagePlugInInterface.h"


class ScriptingLanguagePython : public ScriptingLanguagePlugInInterface
{
public:
	ScriptingLanguagePython();

	virtual void	Initialise();
	virtual void	Shutdown();

	virtual void	ValidateScript(StringRef script);
	virtual void	RunScript(StringRef script);
	virtual void	RegisterScript(StringRef script);
	virtual bool	IsValidExtension(StringRef extension) const;

private:
	bool			IsPythonInitialised() const;

};

#endif // SCRIPTINGLANGUAGEPYTHON_HPP_