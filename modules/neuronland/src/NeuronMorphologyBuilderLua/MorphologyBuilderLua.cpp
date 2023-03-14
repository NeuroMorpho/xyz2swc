//
//
//

#if 0
extern "C"	
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>

using namespace luabind;

#include "NeuronMorphologyBuilder/MorphologyBuilder.h"

/*
	Would prefer to auto-generate this from a specification file (or by tagging the original C++ class)
	Could then auto-generate correct binding code for any scripting language which 
	might be supported.

	Hopefully boost::langbind will eventually appear to give same interface for all scripting languages..
*/

void AddMorphologyBuilderAPIToLua(lua_State* L)
{
	module(L)
	[
		class_<SamplePoint>("SamplePoint")
			.def(constructor<>())
			.def(constructor<float, float, float, float>())
			.def_readwrite("x", &SamplePoint::x)
			.def_readwrite("y", &SamplePoint::y)
			.def_readwrite("z", &SamplePoint::z)
			.def_readwrite("d", &SamplePoint::d),

		class_<MarkerPoint, SamplePoint>("MarkerPoint")
			.def(constructor<>())
			.def(constructor<float, float, float, float>()),

		class_<Neuron3D>("Neuron3D")
			.def(constructor<>()),

		class_<MorphologyBuilder::Soma>("Soma")
			.def("AddSample",					&MorphologyBuilder::Soma::AddSample)
			.def("AddMarker",					&MorphologyBuilder::Soma::AddMarker)
			.def("NumSamples", 					&MorphologyBuilder::Soma::NumSamples)
			.def("NumMarkers", 					&MorphologyBuilder::Soma::NumMarkers)
			.def("GetSample", 					&MorphologyBuilder::Soma::GetSample)
			.def("GetMarker", 					&MorphologyBuilder::Soma::GetMarker)
			.def("GetLastSample",				&MorphologyBuilder::Soma::GetLastSample),

		class_<MorphologyBuilder::Dendrite>("Dendrite")
			.def("GetRoot",						&MorphologyBuilder::Dendrite::GetRoot),

		class_<MorphologyBuilder::Axon>("Axon")
			.def("GetRoot",						&MorphologyBuilder::Axon::GetRoot),

		class_<MorphologyBuilder::Branch>("Branch")
			.def("AddSample",					&MorphologyBuilder::Branch::AddSample)
			.def("AddMarker",					&MorphologyBuilder::Branch::AddMarker)
			.def("NumSamples", 					&MorphologyBuilder::Branch::NumSamples)
			.def("NumMarkers", 					&MorphologyBuilder::Branch::NumMarkers)
			.def("NumChildren", 				&MorphologyBuilder::Branch::NumChildren)
			.def("GetSample", 					&MorphologyBuilder::Branch::GetSample)
			.def("GetMarker", 					&MorphologyBuilder::Branch::GetMarker)
			.def("GetLastSample",				&MorphologyBuilder::Branch::GetLastSample)
			.def("GetChild",					&MorphologyBuilder::Branch::GetChild)
			.def("GetPeer",						&MorphologyBuilder::Branch::GetPeer)
			.def("GetPeerBack",					&MorphologyBuilder::Branch::GetPeerBack)
			.def("GetParent",					&MorphologyBuilder::Branch::GetParent)
			.def("NewChildLast",				&MorphologyBuilder::Branch::NewChildLast)
			.def("NewChildFirst",				&MorphologyBuilder::Branch::NewChildFirst)
			.def("NewPeer",						&MorphologyBuilder::Branch::NewPeer)
			.def("Split",						&MorphologyBuilder::Branch::Split)
			.def("AscendToUnaryBranchPoint",	&MorphologyBuilder::Branch::AscendToUnaryBranchPoint)
			.def(const_self <=	other<MorphologyBuilder::Branch>())
			.def(const_self <	other<MorphologyBuilder::Branch>())
			.def(const_self ==	other<MorphologyBuilder::Branch>()),

		class_<MorphologyBuilder>("MorphologyBuilder")
			.def(constructor<Neuron3D&>())
			.def("NewSomaOpenContour",			&MorphologyBuilder::NewSomaOpenContour)
			.def("NewSomaClosedContour",		&MorphologyBuilder::NewSomaClosedContour)
			.def("NewSomaSinglePoint",			&MorphologyBuilder::NewSomaSinglePoint)
			.def("NewAxon",						&MorphologyBuilder::NewAxon)
			.def("NewAxonWithSample",			&MorphologyBuilder::NewAxonWithSample)
			.def("NewDendrite",					&MorphologyBuilder::NewDendrite)
			.def("NewDendriteWithSample",		&MorphologyBuilder::NewDendriteWithSample)
			.def("NumSomas",					&MorphologyBuilder::NumSomas)
			.def("NumAxons",					&MorphologyBuilder::NumAxons)
			.def("NumDendrites",				&MorphologyBuilder::NumDendrites)
			.def("DeleteAll",					&MorphologyBuilder::DeleteAll)
			.def("DeleteSoma",					&MorphologyBuilder::DeleteSoma)
			.def("DeleteAxon",					&MorphologyBuilder::DeleteAxon)
			.def("DeleteDendrite",				&MorphologyBuilder::DeleteDendrite)
			.def("DeleteSubTree",				&MorphologyBuilder::DeleteSubTree)
			.def("SomaBegin",					&MorphologyBuilder::SomaBegin)
			.def("SomaEnd",						&MorphologyBuilder::SomaEnd)
			.def("NextSoma",					&MorphologyBuilder::NextSoma)
			.def("CurrentSoma",					&MorphologyBuilder::CurrentSoma)
			.def("AxonBegin",					&MorphologyBuilder::AxonBegin)
			.def("AxonEnd",						&MorphologyBuilder::AxonEnd)
			.def("NextAxon",					&MorphologyBuilder::NextAxon)
			.def("CurrentAxon",					&MorphologyBuilder::CurrentAxon)
			.def("DendriteBegin",				&MorphologyBuilder::DendriteBegin)
			.def("DendriteEnd",					&MorphologyBuilder::DendriteEnd)
			.def("NextDendrite",				&MorphologyBuilder::NextDendrite)
			.def("CurrentDendrite",				&MorphologyBuilder::CurrentDendrite)
			.def("SetCurrentSoma",				&MorphologyBuilder::SetCurrentSoma)
			.def("SetCurrentDendrite",			&MorphologyBuilder::SetCurrentDendrite)
			.def("SetCurrentAxon",				&MorphologyBuilder::SetCurrentAxon)
			//.def("AddMarker",					&MorphologyBuilder::AddMarker)
			.def("CsAddSample",					&MorphologyBuilder::CsAddSample)
		//	.def("CsAddMarker",					&MorphologyBuilder::CsAddMarker)
			.def("NullBranch",					&MorphologyBuilder::NullBranch)
			.def("CurrentBranch",				&MorphologyBuilder::CurrentBranch)
			.def("SetCurrentBranch",			&MorphologyBuilder::SetCurrentBranch)
			.def("CbToParent",					&MorphologyBuilder::CbToParent)
			.def("CbToChild",					&MorphologyBuilder::CbToChild)
			.def("CbToPeer",					&MorphologyBuilder::CbToPeer)
			.def("CbToPeerBack",				&MorphologyBuilder::CbToPeerBack)
			.def("CbNewChildLast",				&MorphologyBuilder::CbNewChildLast)
			.def("CbNewChildFirst",				&MorphologyBuilder::CbNewChildFirst)
			.def("CbNewPeer",					&MorphologyBuilder::CbNewPeer)
			.def("CbSplit",						&MorphologyBuilder::CbSplit)
			.def("CbAddSample",					&MorphologyBuilder::CbAddSample)
			//.def("CbAddMarker",					&MorphologyBuilder::CbAddMarker)
			.def("ClearTags",					&MorphologyBuilder::ClearTags)
			.def("CbSetTag",					&MorphologyBuilder::CbSetTag)
			.def("CbGetTag",					&MorphologyBuilder::CbGetTag)
			.def("CbAscendToUnaryBranchPoint",	&MorphologyBuilder::CbAscendToUnaryBranchPoint)
			.def("CbAscendToBranchWithTag",		&MorphologyBuilder::CbAscendToBranchWithTag)
			.def("ApplyChanges",				&MorphologyBuilder::ApplyChanges)
			.def("HasFailed",					&MorphologyBuilder::HasFailed)
	];
}

#endif
