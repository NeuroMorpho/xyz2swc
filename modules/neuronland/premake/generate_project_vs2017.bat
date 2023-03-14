
pushd "%~dp0"

set PREMAKE_EXE="../3rdParty/Premake/premake5.exe"

@call %PREMAKE_EXE% --verbose --file=workspace.lua vs2017

popd