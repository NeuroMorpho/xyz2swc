
-- Basic strategy: derive from ScriptMorphologyHandler, implement the class virtual functions, instantiate the class, and register the instance.
-- C++ side with take control of pointer.

class 'ArborVitaeHandler' (ScriptMorphologyHandler)

function ArborVitaeHandler:__init() super( MorphologyFormatID("ArborVitae"), String("JMOTestLuaArborVitae"), Version("test:1.0.0") )
	print(constructed)
end

function ArborVitaeHandler:Import(rIn, nrn)
	local line
	while(rIn.Remaining()) do
		line = rIn.ReadLine()
		print(s)
	end
	
	return true
end

function ArborVitaeHandler:Export(rOut, nrn)
	print("export")
	return false
end

function ArborVitaeHandler:Validate(rIn)
	print("validate")
	return kValidateFalse
end

function ArborVitaeHandler:IsImportable()
	return true
end

function ArborVitaeHandler:IsExportable()
	return true
end

function ArborVitaeHandler:IsValidatable()
	return true
end


RegisterMorphologyFormatScript(new ArborVitaeHandler)
