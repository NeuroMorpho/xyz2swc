#ifndef SCRIPTINGLANGUAGELUA_HPP_
#define SCRIPTINGLANGUAGELUA_HPP_

#include "Core/String.hpp"

#include "NeuronPlugInSupportScriptingLanguage/ScriptingLanguagePlugInInterface.h"


class ScriptingLanguageLua : public ScriptingLanguagePlugInInterface
{
public:
					ScriptingLanguageLua();

	virtual void	Initialise();
	virtual void	Shutdown();

	virtual void	ValidateScript(StringRef script);
	virtual void	RunScript(StringRef script);
	virtual void	RegisterScript(StringRef script);
	virtual bool	IsValidExtension(StringRef extension) const;

private:
	bool			IsLuaInitialised() const;

//	std::vector<MorphologyFormatHandler> m_

};

#endif // SCRIPTINGLANGUAGELUA_HPP_