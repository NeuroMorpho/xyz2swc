
dofile "neuronland.lua"

solution "Neuronland"

	neuronland_common()

	if _ACTION == "vs2017" then
		location "../solution_vs2017"
	elseif _ACTION == "vs2019" then
		location "../solution_vs2019"
	elseif _ACTION == "gmake2" then
		location "../solution_gmake2"
	end

	startproject "App_NeuronMorphologyFormatConverter"

	group "3rdParty"
		dofile "3rdParty_TinyXml.lua"

	group "Core"
		dofile "Core.lua"

	--group "External"
		-- dofile "NeuronMorphologyExternalSoftware.lua"

	group "Morphology"
		dofile "NeuronRepresentation.lua"
		
		dofile "NeuronMorphologyBuilder.lua"
		dofile "NeuronMorphologyBuilderLua.lua"
		dofile "NeuronMorphologyBuilderPython.lua"
		
		dofile "NeuronMorphologyDataManager.lua"
		dofile "NeuronMorphologyFormat.lua"
		dofile "NeuronMorphologyFormatImportExportBuiltIn.lua"
		dofile "NeuronMorphologyFormatImportExportPlugIn.lua"
		dofile "NeuronMorphologyFormatImportExportScript.lua"
		dofile "NeuronMorphologyFormatManager.lua"
		dofile "NeuronMorphologyFormatTest.lua"
		dofile "NeuronMorphologyGeneration.lua"
		-- dofile "NeuronMorphologyReduction.lua"
		dofile "NeuronMorphologyStatistics.lua"

	--group "Extension"
	--	dofile "NeuronSystemPlugIns.lua"
	--	dofile "NeuronPlugInSupportGeneral.lua"
	--	dofile "NeuronPlugInSupportMorphologyFormat.lua"
	--	dofile "NeuronPlugInSupportScriptingLanguage.lua"

	--group "Scripting"
	--	dofile "NeuronScriptingLanguage.lua"
	--	dofile "NeuronScriptingLanguageLua.lua"
	--	dofile "NeuronScriptingLanguageManager.lua"
	--	dofile "NeuronScriptingLanguagePython.lua"

	group "Visualization"
	if _ACTION == "vs2017" then
		dofile "NeuronOpenGLMorphologyVisualisation.lua"
	end

	--group "Simulation"
		-- dofile "NeuronSignalAnalysisSpectral.lua"
		-- dofile "NeuronSimulator.lua"

	--group "PlugIns"
		-- dofile "PlugIn_MorphologyFormat_DerivedFromBuiltIn.lua"
		-- dofile "PlugIn_ScriptingLanguage_Lua_5.1.2.lua"
		-- dofile "PlugIn_ScriptingLanguage_Python_2.5.lua"

	--group "Scripts"
		-- dofile "Scripts_Lua_MorphologyFormat.lua"

	group "AppHelpers"
		dofile "NeuronApplicationLogic.lua"
		--dofile "NeuronFunctionality.lua"

	group "NLMorphologyConverter"
		dofile "App_NeuronMorphologyFormatConverter.lua"

	if _ACTION == "vs2017" then
		group "NLMorphologyViewer"
		dofile "App_NeuronMorphologyViewer.lua"
	end

	group "Misc"
		-- dofile "Tool_PrepareAddon.lua"
	
