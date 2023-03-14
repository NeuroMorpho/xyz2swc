//
// PlugInHandler.cpp
//

#include "PlugInHandler.h"


PlugInHandler::PlugInHandler(const String& path) :
	m_filePath(path),
	m_valid(false)
{
	m_valid = Load();
}

PlugInHandler::~PlugInHandler(void)
{
	Free();
}

bool PlugInHandler::Load()
{	
	const std::wstring copyW(m_filePath.begin(), m_filePath.end());

	HMODULE hModule = LoadLibrary(copyW.c_str());

	if (hModule)
	{
		m_pDLLhInstance = hModule;

		typedef PlugInDetails (*GETDETAILSFUNC)();

		GETDETAILSFUNC GetDetailsDLL;

		GetDetailsDLL = (GETDETAILSFUNC) GetProcAddress(m_pDLLhInstance, "GetPlugInDetails");

		if(GetDetailsDLL)
		{
			m_details = GetDetailsDLL();
//			report("Found Compatible Plugin: " + String(m_details.GetType()) );
			return true;
		}
	}

	return false;
}

bool PlugInHandler::Free()
{
	if (m_pDLLhInstance)
	{
		FreeLibrary(m_pDLLhInstance);
		m_pDLLhInstance = 0;
	}

	return true;
}

HINSTANCE PlugInHandler::GetHandle() const
{
	return m_pDLLhInstance;
}

const String& PlugInHandler::GetFilePath() const
{
	return m_filePath;
}

bool PlugInHandler::IsValid() const
{
	return m_pDLLhInstance && m_valid;
}

const PlugInDetails& PlugInHandler::GetDetails() const
{
	return m_details;
}
