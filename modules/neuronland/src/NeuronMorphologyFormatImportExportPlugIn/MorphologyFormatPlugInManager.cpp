//
//
//

#include <filesystem>

#include "MorphologyFormatPlugInManager.h"

#if 0
#include "NeuronMorphologyFormatImportExportPlugIn/PlugInMorphologyHandler.h"
#include "NeuronSystemPlugIns/PlugInHandler.h"

namespace fs = std::experimental::filesystem;
#endif

void MorphologyFormatPlugInManager::Scan(const String& location)
{
#if 0
	if ( fs::is_directory( location ) )
	{
		fs::directory_iterator end_iter;
		for(fs::directory_iterator dir_itr( location ); dir_itr != end_iter; ++dir_itr )
		{
			if ( fs::is_regular_file( dir_itr->status() ) )
			{
				PlugInHandler *pPIHandler = new PlugInHandler(location + String(dir_itr->path().filename().string().c_str()));

				if(pPIHandler)
				{
					//m_plugIns[pPIHandler->GetName()] = pPIHandler;
					String pluginType = pPIHandler->GetDetails().GetType();

					if( pluginType == "Morphology3D")
					{
						typedef const MorphologyFormatPlugInInterface& (*GETINTERFACEFUNC)();
						GETINTERFACEFUNC GetInterface;
						GetInterface = (GETINTERFACEFUNC) GetProcAddress(pPIHandler->GetHandle(), "GetMorphologyFormatInterface");

						if(GetInterface)
						{
							push_back( (MorphologyFormatHandler *) ( new PlugInMorphologyHandler(pPIHandler, GetInterface()) ) );

							report("Found morphology plugin - " + GetInterface().GetHandlerDetails().GetHandlerName() );
						}
					}
					else
					{
						report("Unknown plugin type: - " + pluginType);
					}
				}
			}
		}
	}
#endif
}

