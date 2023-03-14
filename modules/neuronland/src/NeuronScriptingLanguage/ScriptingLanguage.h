#ifndef SCRIPTINGLANGUAGE_HPP_ 
#define SCRIPTINGLANGUAGE_HPP_ 

#include "Core/String.hpp"
/*
class ScriptingInstance
{
public:
	virtual void RunScript(StringRef script);

private:

};
*/

/**

	

**/

class ScriptingLanguage
{
public:
	virtual void				ValidateScript(StringRef script) = 0;
	virtual void				RunScript(StringRef script) = 0;
	virtual bool				IsValidExtension(StringRef extension) const = 0;

	//virtual ScriptingInstance*	CreateInstance();
private:

};

#endif // SCRIPTINGLANGUAGE_HPP_ 
