//
// MorphologyBuilder.cpp
//

#include "MorphologyBuilder.h"
#include "Core/StringFormatting.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//
// Soma
//

MorphologyBuilder::Soma::Soma()
{
}

void MorphologyBuilder::Soma::AddSample(SamplePoint sp)
{
	(*m_somaIt).m_samples.push_back(sp);
}

void MorphologyBuilder::Soma::AddMarker(const char* name, MarkerPoint mp)
{
	(*m_somaIt).m_markers[String(name)].push_back(mp);
}

void MorphologyBuilder::Soma::SetColour(u8 r, u8 g, u8 b)
{
	(*m_somaIt).SetColour(r,g,b);
}

u32 MorphologyBuilder::Soma::NumSamples() const
{
	return (u32) (*m_somaIt).m_samples.size();
}

u32 MorphologyBuilder::Soma::NumMarkers() const
{
	return (u32) (*m_somaIt).m_markers.size();
}

SamplePoint MorphologyBuilder::Soma::GetSample(u32 index) const
{
	return (*m_somaIt).m_samples[index];
}

MarkerPoint MorphologyBuilder::Soma::GetMarker(const char* name, u32 index) const
{
	return (*m_somaIt).m_markers[String(name)][index];
}

SamplePoint MorphologyBuilder::Soma::GetLastSample() const
{
	return GetSample(NumSamples()-1);
}

MorphologyBuilder::Soma::Soma(Neuron3D::SomaIterator it) : m_somaIt(it)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//
// Dendrite
//

MorphologyBuilder::Dendrite::Dendrite()
{
}

MorphologyBuilder::Dendrite::Dendrite(Neuron3D::DendriteIterator it) : m_dendIt(it)
{
}

MorphologyBuilder::Branch MorphologyBuilder::Dendrite::GetRoot()
{
	return (*m_dendIt).root();
}

void MorphologyBuilder::Dendrite::SetColour(u8 r, u8 g, u8 b)
{
	(*m_dendIt).root()->SetColour(r,g,b);
}

void MorphologyBuilder::Dendrite::SetApical()
{
	(*m_dendIt).SetTreeType(Neuron3D::DendriteTree::Apical);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//
// Axon
//

MorphologyBuilder::Axon::Axon()
{
}

MorphologyBuilder::Axon::Axon(Neuron3D::AxonIterator it) : m_axonIt(it)
{
}

MorphologyBuilder::Branch MorphologyBuilder::Axon::GetRoot()
{
	return (*m_axonIt).root();
}

void MorphologyBuilder::Axon::SetColour(u8 r, u8 g, u8 b)
{
	(*m_axonIt).root()->SetColour(r,g,b);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//
// Branch
//

MorphologyBuilder::Branch::Branch()
{
	m_branIt = Neuron3D::DendriteTree::null();
}

void MorphologyBuilder::Branch::AddSample(SamplePoint point)
{
	(*m_branIt).m_samples.push_back(point);
}

void MorphologyBuilder::Branch::AddMarker(const char* name, MarkerPoint point)
{
	(*m_branIt).m_markers[String(name)].push_back(point);
}

void MorphologyBuilder::Branch::AddMarker(const String& name, MarkerPoint point)
{
	(*m_branIt).m_markers[name].push_back(point);
}

void MorphologyBuilder::Branch::AddSpine(SpinePoint point)
{
	for(std::vector<SpinePoint>::iterator it = (*m_branIt).m_spines.begin(); it!=(*m_branIt).m_spines.end();++it )
	{
		if( point.associatedSegment < (*it).associatedSegment)
		{
			(*m_branIt).m_spines.insert(it, point);
			return;
		}
	}

	// otherwise, just needs to be added at the end...
	(*m_branIt).m_spines.push_back(point);
}

void MorphologyBuilder::Branch::SetTerminalType(TerminalType type)
{
	(*m_branIt).SetTerminalType(type);
}

u32 MorphologyBuilder::Branch::NumSamples() const
{
	return (u32) (*m_branIt).m_samples.size();
}

u32 MorphologyBuilder::Branch::NumMarkers() const
{
	return (u32) (*m_branIt).m_markers.size();
}

u32 MorphologyBuilder::Branch::NumSpines() const
{
	return (u32) (*m_branIt).m_spines.size();
}

u32 MorphologyBuilder::Branch::NumChildren() const
{
	return (u32) Neuron3D::DendriteTree::countchildren(m_branIt);
}

SamplePoint MorphologyBuilder::Branch::GetSample(u32 index) const
{
	assert(index < (*m_branIt).m_samples.size());
	if(index >= (*m_branIt).m_samples.size())
		return SamplePoint();

	return (*m_branIt).m_samples[index];
}

SamplePoint MorphologyBuilder::Branch::GetLastSample() const
{
	return GetSample( (u32) (*m_branIt).m_samples.size() - 1 );
}

MarkerPoint MorphologyBuilder::Branch::GetMarker(const char* name, u32 index) const
{
	Branch3D::Markers::const_iterator mit = (*m_branIt).m_markers.find(String(name) );
	Branch3D::Markers::const_iterator end = (*m_branIt).m_markers.end();
	if(mit == end || index >= mit->second.size())
		return MarkerPoint();

	return mit->second[index];
}

SpinePoint MorphologyBuilder::Branch::GetSpine(u32 index) const
{
	assert(index < (*m_branIt).m_spines.size());
	if(index >= (*m_branIt).m_spines.size())
		return SpinePoint();

	return (*m_branIt).m_spines[index];
}

void MorphologyBuilder::Branch::ClearSamples()
{
	(*m_branIt).m_samples.clear();
}

void MorphologyBuilder::Branch::ClearMarkers()
{
	(*m_branIt).m_markers.clear();
}

void MorphologyBuilder::Branch::ClearSpines()
{
	(*m_branIt).m_spines.clear();
}

MorphologyBuilder::Branch MorphologyBuilder::Branch::GetChild() const
{
	return MorphologyBuilder::Branch(m_branIt.child());
}

MorphologyBuilder::Branch MorphologyBuilder::Branch::GetPeer() const
{
	return MorphologyBuilder::Branch(m_branIt.peer());
}

MorphologyBuilder::Branch MorphologyBuilder::Branch::GetPeerBack() const
{
	Neuron3D::DendriteTreeIterator it = m_branIt.parent();
	if(it)
	{
		it = it.child();

		if(it != m_branIt)
		{
			while(it.peer() != m_branIt)
			{
				it = it.peer();
			}

			return Branch(it);
		}
	}

	return MorphologyBuilder::NullBranch();
}

MorphologyBuilder::Branch MorphologyBuilder::Branch::GetParent() const
{
	return MorphologyBuilder::Branch(m_branIt.parent());
}

MorphologyBuilder::Branch MorphologyBuilder::Branch::NewChildLast(bool autoAddParentSample)
{
	Branch child = Branch( Neuron3D::DendriteTree::insert_child_last(m_branIt, Neuron3D::DendriteElement() ) );
	if(autoAddParentSample)
		child.AddSample( GetLastSample() );
	return child;
}

MorphologyBuilder::Branch MorphologyBuilder::Branch::NewChildFirst(bool autoAddParentSample)
{
	Branch child = Branch( Neuron3D::DendriteTree::insert_child_first(m_branIt, Neuron3D::DendriteElement() ) );
	if(autoAddParentSample)
		child.AddSample( GetLastSample() );
	return child;
}

MorphologyBuilder::Branch MorphologyBuilder::Branch::NewPeer(bool autoAddParentSample)
{
	Branch peer = Branch( Neuron3D::DendriteTree::insert_peer_after(m_branIt, Neuron3D::DendriteElement() ) );

	if(autoAddParentSample && GetParent() != NullBranch())
		AddSample( GetParent().GetLastSample() );

	return peer;
}

MorphologyBuilder::Branch MorphologyBuilder::Branch::Split(u32 index)
{
	// @@@ warning:  not splitting markers or spines here yet !!!
	assert( (*m_branIt).m_spines.empty() && (*m_branIt).m_markers.empty());

	if(index == 0 || index >= NumSamples()-1)
		return NullBranch();


//	Neuron3D::DendriteTreeIterator upperBran = Neuron3D::SplitBranch(m_branIt, index);

	//Neuron3D::DendriteTreeIterator lowerBran = m_branIt;
	//Neuron3D::DendriteTreeIterator upperBran = Neuron3D::DendriteTree::insert(m_branIt, Neuron3D::DendriteElement());


	Neuron3D::DendriteTreeIterator lowerBran = m_branIt;
	Neuron3D::DendriteTreeIterator upperBran = Neuron3D::DendriteTree::insert(m_branIt, Neuron3D::DendriteElement());

	std::vector<SamplePoint>::iterator it = (*lowerBran).m_samples.begin() + index;

	// copy upper elements
	(*upperBran).m_samples.insert( (*upperBran).m_samples.end(), it, (*lowerBran).m_samples.end() );

	// erase them from lower
	(*lowerBran).m_samples.erase(it+1, (*lowerBran).m_samples.end());

	return MorphologyBuilder::Branch(upperBran);
}

MorphologyBuilder::Branch MorphologyBuilder::Branch::AscendToUnaryBranchPoint()
{
	Branch newBranch = GetParent();

	while(newBranch != NullBranch())
	{
		if( newBranch.NumChildren() == 1)
		{
			//newBranch.NewChildFirst(true);
			break;
		}

		newBranch = newBranch.GetParent();
	}

	return newBranch;
}

MorphologyBuilder::Branch::Branch(Neuron3D::DendriteTreeIterator it) : m_branIt(it)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//
// MorphologyBuilder
//

MorphologyBuilder::MorphologyBuilder(Neuron3D& neuron/*, MorphologyLimitations limitations*/) : 
	m_rOrigNeuron(neuron),
	m_currentBranch(Neuron3D::DendriteTree::null()),
	m_currentDendrite(neuron.DendriteBegin()),
	m_currentAxon(neuron.AxonBegin()),
	m_currentSoma(neuron.SomaBegin())
{
	m_neuron = neuron;
	m_error = kErrorNone;
}

MorphologyBuilder::Soma MorphologyBuilder::NewSomaOpenContour()
{
	SetCurrentSoma( Soma(m_neuron.CreateSoma()) );
	CurrentSoma().m_somaIt->somaType = Soma3D::kContourOpen;
	return CurrentSoma();
}

MorphologyBuilder::Soma MorphologyBuilder::NewSomaClosedContour()
{
	SetCurrentSoma( Soma(m_neuron.CreateSoma()) );
	CurrentSoma().m_somaIt->somaType = Soma3D::kContourClosed;
	return CurrentSoma();
}

MorphologyBuilder::Soma MorphologyBuilder::NewSomaSegment()
{
	SetCurrentSoma( Soma(m_neuron.CreateSoma()) );
	CurrentSoma().m_somaIt->somaType = Soma3D::kSegment;
	return CurrentSoma();
}

MorphologyBuilder::Soma MorphologyBuilder::NewSomaSinglePoint(SamplePoint sp)
{
	SetCurrentSoma( Soma(m_neuron.CreateSoma()) );
	CurrentSoma().m_somaIt->somaType = Soma3D::kPoint;
	CsAddSample(sp);
	return CurrentSoma();
}

MorphologyBuilder::Axon MorphologyBuilder::NewAxon()
{
	Neuron3D::AxonIterator ait		= m_neuron.CreateAxon();
	Neuron3D::AxonTreeIterator atit = (*ait).set_root( Neuron3D::AxonElement() );
	SetCurrentBranch( Branch(atit) );
	SetCurrentAxon( Axon(ait) );

	return CurrentAxon();
}

MorphologyBuilder::Axon MorphologyBuilder::NewAxonWithSample(SamplePoint sp)
{
	NewAxon();
	CbAddSample(sp);
	return CurrentAxon();
}

MorphologyBuilder::Dendrite MorphologyBuilder::NewDendrite()
{
	Neuron3D::DendriteIterator dit		= m_neuron.CreateDendrite();
	Neuron3D::DendriteTreeIterator dtit = (*dit).set_root( Neuron3D::DendriteElement() );
	SetCurrentBranch( Branch(dtit) );
	SetCurrentDendrite( Dendrite(dit) );

	return CurrentDendrite();
}

MorphologyBuilder::Dendrite MorphologyBuilder::NewDendriteWithSample(SamplePoint sp)
{
	NewDendrite();
	CbAddSample(sp);
	return CurrentDendrite();
}

u32 MorphologyBuilder::NumSomas() const
{
	return m_neuron.CountSomas();
}

u32 MorphologyBuilder::NumAxons() const
{
	return m_neuron.CountAxons();
}

u32 MorphologyBuilder::NumDendrites() const
{
	return m_neuron.CountDendrites();
}

void MorphologyBuilder::DeleteAll()
{
	m_neuron.Clear();
	m_currentBranch		= NullBranch();
	m_currentSoma		= Soma( m_neuron.SomaBegin() );
	m_currentDendrite	= Dendrite( m_neuron.DendriteBegin() );
	m_currentAxon		= Axon( m_neuron.AxonBegin() );

	m_branchTagInfo.clear();
}

void MorphologyBuilder::DeleteSoma(Soma s)
{
	m_neuron.DeleteSoma(s.m_somaIt);
	// WARNING - CurrentSoma may be invalid now
}

void MorphologyBuilder::DeleteAxon(Axon a)
{
	m_neuron.DeleteAxon(a.m_axonIt);
	// WARNING - CurrentAxon may be invalid now
}

void MorphologyBuilder::DeleteDendrite(Dendrite d)
{
	m_neuron.DeleteDendrite(d.m_dendIt);
	// WARNING - CurrentDendrite may be invalid now
}

void MorphologyBuilder::DeleteSubTree(Branch b)
{
	assert(0);
}

MorphologyBuilder::Dendrite		MorphologyBuilder::DendriteBegin()
{
	return Dendrite( m_neuron.DendriteBegin() );
}

MorphologyBuilder::Dendrite		MorphologyBuilder::DendriteEnd()
{
	return Dendrite( m_neuron.DendriteEnd() );
}

MorphologyBuilder::Dendrite		MorphologyBuilder::NextDendrite()
{
	m_currentDendrite = Dendrite(++CurrentDendrite().m_dendIt);

	if(CurrentDendrite() != DendriteEnd())
		SetCurrentBranch(CurrentDendrite().GetRoot());
	else
		SetCurrentBranch(NullBranch());

	return CurrentDendrite();
}

MorphologyBuilder::Dendrite		MorphologyBuilder::CurrentDendrite()
{
	return m_currentDendrite;
}

MorphologyBuilder::Axon			MorphologyBuilder::AxonBegin()
{
	return Axon(m_neuron.AxonBegin());
}

MorphologyBuilder::Axon			MorphologyBuilder::AxonEnd()
{
	return Axon( m_neuron.AxonEnd() );
}

MorphologyBuilder::Axon			MorphologyBuilder::NextAxon()
{
	m_currentAxon = Axon(++CurrentAxon().m_axonIt);

	if(CurrentAxon() != AxonEnd())
		SetCurrentBranch(CurrentAxon().GetRoot());
	else
		SetCurrentBranch(NullBranch());

	return CurrentAxon();
}

MorphologyBuilder::Axon			MorphologyBuilder::CurrentAxon()
{
	return m_currentAxon;
}

MorphologyBuilder::Soma			MorphologyBuilder::SomaBegin()
{
	return Soma(m_neuron.SomaBegin());
}

MorphologyBuilder::Soma			MorphologyBuilder::SomaEnd()
{
	return Soma(m_neuron.SomaEnd());
}

MorphologyBuilder::Soma			MorphologyBuilder::NextSoma()
{
	m_currentSoma = Soma(++CurrentSoma().m_somaIt);
	return CurrentSoma();
}

MorphologyBuilder::Soma MorphologyBuilder::CurrentSoma()
{
	return m_currentSoma;
}

MorphologyBuilder::Soma MorphologyBuilder::SetCurrentSoma(MorphologyBuilder::Soma s)
{
	m_currentSoma = s;
	return s;
}

MorphologyBuilder::Dendrite MorphologyBuilder::SetCurrentDendrite(MorphologyBuilder::Dendrite d)
{
	m_currentDendrite = d;
	SetCurrentBranch(d.GetRoot());
	return d;
}

MorphologyBuilder::Axon MorphologyBuilder::SetCurrentAxon(MorphologyBuilder::Axon a)
{
	m_currentAxon = a;
	SetCurrentBranch(a.GetRoot());
	return a;
}

void MorphologyBuilder::CsAddSample(SamplePoint sp)
{
	CurrentSoma().AddSample(sp);
}

void MorphologyBuilder::CsAddMarker(const char* name, MarkerPoint mp)
{
	CurrentSoma().AddMarker(name, mp);
}

void MorphologyBuilder::CsAddMarker(const String& name, MarkerPoint mp)
{
	CsAddMarker(name.c_str(), mp);
}

void MorphologyBuilder::CsSetColour(u8 r, u8 g, u8 b)
{
	CurrentSoma().SetColour(r,g,b);
}

void MorphologyBuilder::AddMarker(const char* name, MarkerPoint mp)
{
	m_neuron.AddMarker(name, mp);
}

void MorphologyBuilder::AddMarker(const String& name, MarkerPoint mp)
{
	m_neuron.AddMarker(name.c_str(), mp);
}

void MorphologyBuilder::AddSpine(SpinePoint sp)
{
	m_neuron.AddSpine(sp);
}

MorphologyBuilder::Branch MorphologyBuilder::NullBranch()
{
	return Branch( Neuron3D::DendriteTree::null() );
}

MorphologyBuilder::Branch MorphologyBuilder::CurrentBranch() const
{
	return m_currentBranch;
}

MorphologyBuilder::Branch MorphologyBuilder::SetCurrentBranch(MorphologyBuilder::Branch b)
{
	m_currentBranch = b;
	return b;
}

MorphologyBuilder::Branch MorphologyBuilder::CbToParent()
{
	m_currentBranch = m_currentBranch.GetParent();
	return m_currentBranch;
}

MorphologyBuilder::Branch MorphologyBuilder::CbToChild()
{
	m_currentBranch = m_currentBranch.GetChild();
	return m_currentBranch;
}

MorphologyBuilder::Branch MorphologyBuilder::CbToPeer()
{
	m_currentBranch = m_currentBranch.GetPeer();
	return m_currentBranch;
}

MorphologyBuilder::Branch MorphologyBuilder::CbToPeerBack()
{
	m_currentBranch = m_currentBranch.GetPeerBack();
	return m_currentBranch;
}

MorphologyBuilder::Branch MorphologyBuilder::CbNewChildLast(bool autoAddParentSample)
{
	m_currentBranch = CurrentBranch().NewChildLast(autoAddParentSample);
	return m_currentBranch;
}

MorphologyBuilder::Branch MorphologyBuilder::CbNewChildFirst(bool autoAddParentSample)
{
	m_currentBranch = CurrentBranch().NewChildFirst(autoAddParentSample);
	return m_currentBranch;
}

MorphologyBuilder::Branch MorphologyBuilder::CbNewPeer(bool autoAddParentSample)
{
	m_currentBranch = CurrentBranch().NewPeer(autoAddParentSample);
	return m_currentBranch;
}

MorphologyBuilder::Branch MorphologyBuilder::CbSplit(u32 index)
{
	m_currentBranch = CurrentBranch().Split(index);
	return m_currentBranch;
}

void MorphologyBuilder::CbAddSample(SamplePoint sp)
{
	CurrentBranch().AddSample(sp);
}

void MorphologyBuilder::CbAddMarker(const char* name, MarkerPoint mp)
{
	CurrentBranch().AddMarker(name, mp);
}

void MorphologyBuilder::CbAddMarker(const String& name, MarkerPoint mp)
{
	CurrentBranch().AddMarker(name.c_str(), mp);
}

void MorphologyBuilder::CbAddSpine(SpinePoint sp)
{
	CurrentBranch().AddSpine(sp);
}

void MorphologyBuilder::CbSetTerminalType(TerminalType type)
{
	CurrentBranch().SetTerminalType(type);
}

void MorphologyBuilder::CbClearSamples()
{
	CurrentBranch().ClearSamples();
}

void MorphologyBuilder::CdSetColour(u8 r, u8 g, u8 b)
{
	CurrentDendrite().SetColour(r,g,b);
}

void MorphologyBuilder::CdSetApical()
{
	CurrentDendrite().SetApical();
}

void MorphologyBuilder::SetTagForBranch(Branch bran, HString name, u32 tag)
{
	for(u32 n=0;n<m_branchTagInfo.size();++n)
	{
		if(	m_branchTagInfo[n].name == name && 
			m_branchTagInfo[n].it == bran.m_branIt)
		{
			m_branchTagInfo[n].tag = tag;
			//report("Set Tag - " + name.toStringForm() + " : " + dec(tag) + " : " + m_currentBranch.m_branIt.toString());
			return;
		}
	}

	branchTagInfo info = { name, bran.m_branIt, tag };
	m_branchTagInfo.push_back(info);

	// _OSX link prob
	//report("Set Tag - " + name.toStringForm() + " : " + dec(tag) + " : " + m_currentBranch.m_branIt.toString());

	//(m_tagmaps[name])[CurrentBranch().m_branIt] = tag;
}

void MorphologyBuilder::CbSetTag(HString name, u32 tag)
{
	SetTagForBranch(CurrentBranch(), name, tag);
}

u32 MorphologyBuilder::CbGetTag(HString name) const
{
	return GetTagForBranch(CurrentBranch(), name);
}

u32 MorphologyBuilder::GetTagForBranch(MorphologyBuilder::Branch bran, HString name) const
{
	for(u32 n=0;n<m_branchTagInfo.size();++n)
	{
		if(	m_branchTagInfo[n].name == name && 
			m_branchTagInfo[n].it == bran.m_branIt)
		{
			//report("Get Tag - " + name.toStringForm() + " : " + dec(m_branchTagInfo[n].tag) + " : " + m_currentBranch.m_branIt.toString());
			return m_branchTagInfo[n].tag;
		}
	}

#if 0
	NameMap::const_iterator it = m_tagmaps.find(name);
	if(it != m_tagmaps.end())
	{
		BranchTagMap::const_iterator bit = (*it).second.find(CurrentBranch().m_branIt);
		if(bit != (*it).second.end())
		{
			report("Get Tag - " + name.toStringForm() + " : " + dec((*bit).second) + " : " + m_currentBranch.m_branIt.toString() + " : iterator " + dec(*(u32*)(&bit)) );
			return (*bit).second;
		}
	}
#endif

	return 99999999;
}

void MorphologyBuilder::ClearTags()
{
	m_branchTagInfo.clear();
}


MorphologyBuilder::Branch MorphologyBuilder::CbAscendToUnaryBranchPoint()
{
	m_currentBranch = CurrentBranch().AscendToUnaryBranchPoint();
	return m_currentBranch;
}

MorphologyBuilder::Branch MorphologyBuilder::CbAscendToBranchWithTag(HString name, u32 tag)
{
	Branch branch = CbToParent();

	while(branch != NullBranch())
	{
		if( CbGetTag(name) == tag)
			break;

		branch = CbToParent();
	}

	return branch;
}

void MorphologyBuilder::ChangeDendriteToAxon(Dendrite d)
{
	assert(0);
}

void MorphologyBuilder::ChangeAxonToDendrite(Axon a)
{
	assert(0);
}

void MorphologyBuilder::ApplyChanges()
{
	m_rOrigNeuron = m_neuron;
}

bool MorphologyBuilder::HasFailed() const
{
	return m_error != kErrorNone;
}


String MorphologyBuilder::ConstructionError2String(ConstructionError error) const
{
	switch(error)
	{
		case kErrorNone:
			return "No Error";
		case kErrorNullSoma:
			return "Non-existent Soma";
		case kErrorNullBranch:
			return "Non-existent Branch";
		case kErrorNullDendrite:
			return "Non-existent Dendrite";
		case kErrorNullAxon:
			return "Non-existent Axon";
		case kErrorPointTypeHasOneSample:
			return "Point type has one sample";
		case kErrorOutOfMemory:
			return "Out of Memory";
		default:
			return "Unknown Error";
	}
}
