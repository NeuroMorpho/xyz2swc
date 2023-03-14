#ifndef _PLUGINHANDLER_H__TWIG__
#define _PLUGINHANDLER_H__TWIG__


#include "Core/String.hpp"

#include "NeuronPlugInSupportGeneral/PlugInDetails.h"

#include <windows.h>

class PlugInHandler
{
public:
					PlugInHandler(const String& path);
	virtual			~PlugInHandler();

	HINSTANCE				GetHandle() const;
	const String&			GetFilePath() const;
	bool					IsValid() const;
	const PlugInDetails&	GetDetails() const;

private:
	bool 			Load();
	bool 			Free();

	HINSTANCE		m_pDLLhInstance;
	String			m_filePath;
	bool			m_valid;
	PlugInDetails   m_details;
};

#endif // _PLUGINHANDLER_H__TWIG__