#ifndef SCRIPTINGLANGUAGEPLUGININTERFACE_HPP_
#define SCRIPTINGLANGUAGEPLUGININTERFACE_HPP_

#include "Core/String.hpp"


class ScriptingLanguagePlugInInterface
{
public:
	virtual void	Initialise() = 0;
	virtual void	Shutdown() = 0;

	virtual void	ValidateScript(const String& script) = 0;
	virtual void	RegisterScript(const String& script) = 0;
	virtual void	RunScript(const String& script) = 0;
	virtual bool	IsValidExtension(const String& extension) const = 0;
};


#endif // SCRIPTINGLANGUAGEPLUGININTERFACE_HPP_