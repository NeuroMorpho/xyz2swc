

function neuronland_common()

	configurations { "Debug", "Profile", "Release" }
	platforms { "x86", "x64" }
	cppdialect "C++17"
end

function neuronland_proj_common()


	configuration {"Debug"}
		defines { "DEBUG" }

	configuration {"Profile"}
		defines { "NDEBUG", "PROFILE" }
	--	optimize { "Full" }
	--	vectorextensions {"SSE2"}
	
	configuration {"Release"}
		defines { "NDEBUG", "RELEASE" }
	--	optimize { "Full" }
	--	vectorextensions {"SSE2"}

	-- todo, objdir, targetdir

	configuration { }
		includedirs 
		{ 	
			"../src/",
			"../src/Core/",
			"../3rdParty/boost/boost_1_46_1",
			"../3rdParty/Lua/src",
			"../3rdParty/LuaGlut",
			"../3rdParty/LuaBind",
			"../3rdParty/wxWidgets/include",
			"../3rdParty/wxLua",
			"../3rdParty/Python/include",
		}
end

function neuronland_proj_lib()
	
	language    "C++"
	kind        "StaticLib"

	neuronland_proj_common()
end


function neuronland_link_common()
	links 
	{
		"Core"
	}
end

function neuronland_proj_app_console()

	language    "C++"
	kind        "ConsoleApp"

	neuronland_proj_common()
	neuronland_link_common()
	
end

function neuronland_proj_app_windowed()

	language    "C++"
	kind        "WindowedApp"

	neuronland_proj_common()
	neuronland_link_common()

end
