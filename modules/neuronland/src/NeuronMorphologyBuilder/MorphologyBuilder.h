#ifndef MORPHOLOGYBUILDER_HPP_
#define MORPHOLOGYBUILDER_HPP_

#include <map>
#include <vector>

#include "Core/HString.h"

#include "NeuronRepresentation/Neuron3D.h"

/** 

	@brief	The idea here is to present the most simple possible interface for 
			building up a neuron structure. Plenty of error checking to stop bad 
			things happening.

	@note	Relies heavily on the functionality of Neuron3D.
	@note	This is the interface used by built-in morphology importers.
	@note	This is part of the C++ interface to be exposed in SDK.
	@note	This interface is exposed (directly or indirectly) to scripting languages.

	@todo	tag lookup optimisation

**/

class MorphologyBuilder
{
public:

	/*
	enum SomaType
	{
		kPoint,
		kSegment,
		kContourOpen,
		kContourClosed
	};*/

	enum ConstructionError
	{
		kErrorNone,
		kErrorNullSoma,
		kErrorNullBranch,
		kErrorNullDendrite,
		kErrorNullAxon,
		kErrorPointTypeHasOneSample,
		kErrorOutOfMemory,
	};

	class Branch
	{
	public:

					Branch();

		// samples and markers
		void		AddSample(SamplePoint point);
		void		AddMarker(const char* name, MarkerPoint point);
		void		AddMarker(const String& name, MarkerPoint point);
		void		AddSpine(SpinePoint point);

		void		SetTerminalType(TerminalType type);

		u32			NumSamples() const;
		u32			NumMarkers() const;
		u32			NumSpines() const;

		u32			NumChildren() const;

		SamplePoint GetSample(u32 index) const;
		MarkerPoint GetMarker(const char* name, u32 index) const;
		SpinePoint	GetSpine(u32 index) const;

		SamplePoint GetLastSample() const;

		void		ClearSamples();
		void		ClearMarkers();
		void		ClearSpines();

		// navigation to other branches
		Branch		GetChild() const;
		Branch		GetPeer() const;
		Branch		GetPeerBack() const;
		Branch		GetParent() const;

		// modify attached structure
		Branch		NewChildLast(bool autoAddFirstSample);
		Branch		NewChildFirst(bool autoAddFirstSample);
		Branch		NewPeer(bool autoAddFirstSample);
		Branch		Split(u32 index);

		Branch		AscendToUnaryBranchPoint();

		bool operator!=(const Branch& rhs) const { return m_branIt != rhs.m_branIt; }
		bool operator<(const Branch& rhs) const { return m_branIt < rhs.m_branIt; } 
		bool operator<=(const Branch& rhs) const { return m_branIt <= rhs.m_branIt; } 
		bool operator==(const Branch& rhs) const { return !(*this != rhs); }
		operator bool() { return *this != NullBranch(); }

#if !defined(_OSX) && !defined(_LINUX)
	private:
#endif
		Branch(Neuron3D::DendriteTreeIterator it);

#if !defined(_OSX) && !defined(_LINUX)
	private:
#endif


		Neuron3D::DendriteTreeIterator	m_branIt;

		friend class MorphologyBuilder;
	};


	class Soma
	{
	public:
						Soma();

		void			AddSample(SamplePoint sp);
		void			AddMarker(const char* name, MarkerPoint mp);

		void			SetColour(u8 r, u8 g, u8 b);

		u32				NumSamples() const;
		u32				NumMarkers() const;

		SamplePoint		GetSample(u32 index) const;
		MarkerPoint		GetMarker(const char* name, u32 index) const;
		SamplePoint		GetLastSample() const;

		bool operator!=(const Soma& rhs) const { return m_somaIt != rhs.m_somaIt; }

	private:
						Soma(Neuron3D::SomaIterator it);

		Neuron3D::SomaIterator m_somaIt;

		friend class MorphologyBuilder;
	};

	class Dendrite
	{
	public:
									Dendrite();
		MorphologyBuilder::Branch	GetRoot();

		bool operator!=(const Dendrite& rhs) const { return m_dendIt != rhs.m_dendIt; }
		bool operator==(const Dendrite& rhs) const { return m_dendIt == rhs.m_dendIt; }

		void			SetColour(u8 r, u8 g, u8 b);
		void			SetApical();

	private:
									Dendrite(Neuron3D::DendriteIterator it);

		Neuron3D::DendriteIterator m_dendIt;

		friend class MorphologyBuilder;
	};

	class Axon
	{
	public:
									Axon();
		MorphologyBuilder::Branch	GetRoot();

		bool operator!=(const Axon& rhs) const { return m_axonIt != rhs.m_axonIt; }

		void			SetColour(u8 r, u8 g, u8 b);

	private:
									Axon(Neuron3D::AxonIterator it);

		Neuron3D::AxonIterator m_axonIt;

		friend class MorphologyBuilder;
	};


					MorphologyBuilder(Neuron3D& neuron/*, MorphologyLimitations limitations*/);

	// create primary structures
	Soma			NewSomaOpenContour();
	Soma			NewSomaClosedContour();
	Soma			NewSomaSegment();
	Soma			NewSomaSinglePoint(SamplePoint sp);

	Axon			NewAxon();
	Axon			NewAxonWithSample(SamplePoint sp);

	Dendrite		NewDendrite();
	Dendrite		NewDendriteWithSample(SamplePoint sp);

	u32				NumSomas() const;
	u32				NumAxons() const;
	u32				NumDendrites() const;

	// destruction
	void			DeleteAll();
	void			DeleteSoma(Soma s);
	void			DeleteAxon(Axon a);
	void			DeleteDendrite(Dendrite d);
	void			DeleteSubTree(Branch b);

	// current state and iteration
	Soma			SomaBegin();
	Soma			SomaEnd();
	Soma			NextSoma();
	Soma			CurrentSoma();

	Axon			AxonBegin();
	Axon			AxonEnd();
	Axon			NextAxon();
	//Axon			PrevAxon();
	Axon			CurrentAxon();

	Dendrite		DendriteBegin();
	Dendrite		DendriteEnd(); 
	Dendrite		NextDendrite();
	//Dendrite		PrevDendrite();
	Dendrite		CurrentDendrite();

	Soma			SetCurrentSoma(Soma s);
	Dendrite		SetCurrentDendrite(Dendrite d);
	Axon			SetCurrentAxon(Axon d);

	// modify current neuron (global state)
	void			AddMarker(const char* name, MarkerPoint mp);
	void			AddMarker(const String& name, MarkerPoint mp);
	void			AddSpine(SpinePoint sp);

	// modify current soma
	void			CsAddSample(SamplePoint sp);
	void			CsAddMarker(const char* name, MarkerPoint mp);
	void			CsAddMarker(const String& name, MarkerPoint mp);
	void			CsSetColour(u8 r, u8 g, u8 b);

	// current branch state
	static Branch	NullBranch();
	Branch			CurrentBranch() const;
	Branch			SetCurrentBranch(Branch b);
	Branch			CbToParent();
	Branch			CbToChild();
	Branch			CbToPeer();
	Branch			CbToPeerBack();

	// create new branches, change current branch
	Branch			CbNewChildLast(bool autoAddParentSample);
	Branch			CbNewChildFirst(bool autoAddParentSample);
	Branch			CbNewPeer(bool autoAddParentSample);
	Branch			CbSplit(u32 index);

	// modify current branch (axon or dendrite)
	void			CbAddSample(SamplePoint sp);
	void			CbAddMarker(const char* name, MarkerPoint mp);
	void			CbAddMarker(const String& name, MarkerPoint mp);
	void			CbAddSpine(SpinePoint sp);

	void			CbSetTerminalType(TerminalType type);

	void			CbClearSamples();

	void			ClearTags();
	void			CbSetTag(HString name, u32 tag);
	u32				CbGetTag(HString name) const;
	Branch			CbAscendToUnaryBranchPoint();
	Branch			CbAscendToBranchWithTag(HString name, u32 tag);

	u32				GetTagForBranch(Branch bran, HString name) const;
	void			SetTagForBranch(Branch bran, HString name, u32 tag);

	template<class unaryfunc>
	Branch			CbAscendToBranchWithTagCondition(HString tagname, unaryfunc f)
	{
		while( CbToParent() )
		{
			u32 tag = CbGetTag(tagname);
			bool result = f(tag);
			if(result)
				break;
		}

		return CurrentBranch();
	}


	void		CdSetColour(u8 r, u8 g, u8 b);
	void		CdSetApical();


	bool		CdMoveToPosition(u32 position)
	{
		Dendrite dendToMove = CurrentDendrite();
		const u32 numDendrites = NumDendrites();
		if(position >= numDendrites)
			return false;

		u32 dendriteCount = 0;
		Dendrite d = DendriteBegin();
		SetCurrentDendrite(d);
		while(dendriteCount < position)
		{
			NextDendrite();
			++dendriteCount;
		}

		if(CurrentDendrite().m_dendIt != dendToMove.m_dendIt)
			m_neuron.MoveDendrite(CurrentDendrite().m_dendIt, dendToMove.m_dendIt);

		SetCurrentDendrite(dendToMove);

		assert(numDendrites == m_neuron.CountDendrites());

		return true;
	}

	template<class binaryfunc>
	Branch			CdFindBranchWithTagCondition(HString name, binaryfunc f)
	{
		/*
		for(BranchTagMap::const_iterator it = m_tagmaps[tagname].begin(); it != m_tagmaps[tagname].end();++it)
		{
			if( f( (*it).second, Branch( (*it).first ) ) )
				return Branch( (*it).first );
		}
		*/

		for(u32 n=0;n<m_branchTagInfo.size();++n)
		{
			if(	m_branchTagInfo[n].name == name )
			{
				Branch bran(m_branchTagInfo[n].it);
				if( f(m_branchTagInfo[n].tag, bran) )
				{
					return bran;
				}
			}
		}


		return NullBranch();
	}

	Branch			CdFindBranchWithTag(HString name, u32 tag)
	{
		/*
		for(BranchTagMap::const_iterator it = m_tagmaps[tagname].begin(); it != m_tagmaps[tagname].end();++it)
		{
			if( f( (*it).second, Branch( (*it).first ) ) )
				return Branch( (*it).first );
		}*/

		for(u32 n=0;n<m_branchTagInfo.size();++n)
		{
			if(	m_branchTagInfo[n].name == name && 
				m_branchTagInfo[n].tag == tag)
			{
				return Branch(m_branchTagInfo[n].it);
			}
		}

		return NullBranch();
	}

	struct CheckIfTagInRangeFunctor : public std::binary_function<u32, MorphologyBuilder::Branch, bool>
	{
		CheckIfTagInRangeFunctor(u32 node) : searchNode(node) { }

		bool operator()(u32 rootNode, MorphologyBuilder::Branch branch)
		{
			return	searchNode >= rootNode							&& 
					searchNode < rootNode + branch.NumSamples() - 1;
		}

		u32 searchNode;
	};



	// move trees
	void			ChangeDendriteToAxon(Dendrite d);
	void			ChangeAxonToDendrite(Axon a);

	void			ApplyChanges();
	bool			HasFailed() const;

	String			ConstructionError2String(ConstructionError error) const;
	String			GetError() const { return ConstructionError2String(m_error); }

private:
	Neuron3D&											m_rOrigNeuron;
	Neuron3D											m_neuron;

	Branch												m_currentBranch;
	Dendrite											m_currentDendrite;
	Axon												m_currentAxon;
	Soma												m_currentSoma;

	ConstructionError									m_error;


	//typedef std::map<Neuron3D::DendriteTreeIterator, u32>	BranchTagMap;
	//typedef std::map<HString,  BranchTagMap>				NameMap;
	//NameMap													m_tagmaps;

	struct branchTagInfo
	{
		HString name;
		Neuron3D::DendriteTreeIterator it;
		u32 tag;
	};

	std::vector<branchTagInfo> m_branchTagInfo;
};

#endif // MORPHOLOGYBUILDER_HPP_
