#!/bin/bash

PREMAKE_EXE=./3rdParty/premake/premake5

$PREMAKE_EXE --verbose --file=./premake/workspace.lua gmake2
