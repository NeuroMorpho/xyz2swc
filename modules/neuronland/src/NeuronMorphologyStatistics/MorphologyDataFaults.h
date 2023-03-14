#ifndef MORPHOLOGYDATAFAULTS_HPP__
#define MORPHOLOGYDATAFAULTS_HPP__

#include <ostream>
#include "Core/StringFormatting.h"

enum NeuronFaultType
{
	// tree
	kTreeTrunkTooDistant,					// tree id

	// branch - connectivity
	kTreeBranchPointUnary,					// tree id, branch id					- soln: merge branches (auto)
	kTreeBranchPointTrinary,
	kTreeBranchPointNary,					// tree id, branch id					- soln: not necessarily a concern
	kTreeBranchDisjointToParent,			// tree id, branch id					- soln: set connection points to the same value ? (or leave)
	kTreeBranchFallsWithinParentRadius, 	//XXX tree id, branch id

	// branch - number of samples
	kTreeBranchSamplesZero,					// tree id, branch id					- soln: delete ?
	kTreeBranchSamplesOnlyOne,				// tree id, branch id					- soln: delete ?

	// branch - sample changes
	kTreeBranchDiameterLargeJumpFromParent, //tree id, branch id
	kTreeBranchTerminalDiameterTooBig,		//tree id, branch id, sample id		- soln: assign value
	kTreeBranchSamplesTooClose,				//tree id, branch id, sample id

	// branch - individual samples
	kTreeBranchSampleTooDistant,			// tree id, branch id, sample id		- soln: delete sample, ignore sample
	kTreeBranchSampleDiameterTooBig,		// tree id, branch id, sample id		- soln: assign value
	kTreeBranchSampleDiameterTiny,			// tree id, branch id, sample id		- soln: assign value
	kTreeBranchSampleDiameterZero,			// tree id, branch id, sample id		- soln: assign value

	// branch - markers
	kTreeBranchMarkerTooDistant,			// tree id, branch id, marker id		- soln: attach to nearest branch; make global

	// soma
	kSomaSampleTooDistant,					// soma id, sample id
	kSomaMarkerTooDistant					// soma id, marker id
};

struct NeuronFaultData
{
	NeuronFaultType type;
	u32	objectId;
	u32 elementId;
	u32 index;
	SamplePoint refSample;
};

struct MorphologyDataFaults
{
	std::vector<NeuronFaultData> m_faults;

	void Add(const NeuronFaultData& fault)
	{
		m_faults.push_back(fault);
	}

	void Clear()
	{
		m_faults.clear();
	}

	String Sample2String(const SamplePoint& s) const
	{
		return dec(s.x) + ", " + dec(s.y) + ", " + dec(s.z) + ", " + dec(s.d);
	}

	String FaultToString(const NeuronFaultData& fault) const
	{
		switch(fault.type)
		{
		case kTreeTrunkTooDistant:
			return "\tTree root point too distant ------ Tree #" + dec(fault.objectId+1, 2);

		case kTreeBranchPointUnary:
			return "\tUnary branch point --------------- Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchPointTrinary:
			return "\tTrinary branch point ------------- Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchPointNary:
			return "\tN-ary branch point with N > 3 ---- Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchDisjointToParent:
			return "\tBranch child is disjoint --------- Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchFallsWithinParentRadius:
			return "\tBranch lies inside parent -------- Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";

		case kTreeBranchSamplesZero:
			return "\tBranch with 0 samples ------------ Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchSamplesOnlyOne:
			return "\tBranch with 1 sample ------------- Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";

		case kTreeBranchDiameterLargeJumpFromParent:
			return "\tBranch with 1 sample ------------- Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchTerminalDiameterTooBig:
			return "\tTerminal branch diameter too big - Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchSamplesTooClose:
			return "\tSamples very close together ------ Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";

		case kTreeBranchSampleTooDistant:
			return "\tUnexpectedly distant sample ------ Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchSampleDiameterTooBig:
			return "\tUnexpectedly large diameter ------ Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchSampleDiameterTiny:
			return "\tBranch Sample with zero diameter - Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchSampleDiameterZero:
			return "\tBranch Sample with tiny diameter - Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";


		case kTreeBranchMarkerTooDistant:
			return "\tUnexpectedly distant marker ------ Tree #" + dec(fault.objectId, 2) + " Branch #" + dec(fault.elementId+1) + " Marker #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";

		case kSomaSampleTooDistant:
			return "\tSoma Sample unexpectedly distant - Tree #" + dec(fault.objectId, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kSomaMarkerTooDistant:
			return "\tSoma Marker unexpectedly distant - Tree #" + dec(fault.objectId, 2) + " Branch #" + dec(fault.elementId+1) + " Marker #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		default:
			return String("");
		}
	}


	String FaultToStringVisual(const NeuronFaultData& fault) const
	{
		switch(fault.type)
		{
		case kTreeTrunkTooDistant:
			return "Tree root point too distant.Tree #" + dec(fault.objectId+1, 2) ;

		case kTreeBranchPointUnary:
			return "Unary branch point: Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchPointTrinary:
			return "Trinary branch point: Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchPointNary:
			return "N-ary branch point with N > 3:Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchDisjointToParent:
			return "Branch child is disjoint: Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchFallsWithinParentRadius:
			return "Branch lies inside parent: Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";

		case kTreeBranchSamplesZero:
			return "Branch with 0 samples: Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchSamplesOnlyOne:
			return "\tBranch with 1 sample: Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";

		case kTreeBranchDiameterLargeJumpFromParent:
			return "\tBranch with 1 sample: Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchTerminalDiameterTooBig:
			return "\tTerminal branch diameter too big: Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchSamplesTooClose:
			return "\tSamples very close together: Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";

		case kTreeBranchSampleTooDistant:
			return "\tUnexpectedly distant sample: Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchSampleDiameterTooBig:
			return "\tUnexpectedly large diameter: Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchSampleDiameterTiny:
			return "\tBranch Sample with zero diameter:Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kTreeBranchSampleDiameterZero:
			return "\tBranch Sample with tiny diameter: Tree #" + dec(fault.objectId+1, 2) + " Branch #" + dec(fault.elementId+1) + " Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";


		case kTreeBranchMarkerTooDistant:
			return "\tUnexpectedly distant marker: Tree #" + dec(fault.objectId, 2) + " Branch #" + dec(fault.elementId+1) + "Marker #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";

		case kSomaSampleTooDistant:
			return "\tSoma Sample unexpectedly distant: Tree #" + dec(fault.objectId, 2) + " Branch #" + dec(fault.elementId+1) + "Sample #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		case kSomaMarkerTooDistant:
			return "\tSoma Marker unexpectedly distant: Tree #" + dec(fault.objectId, 2) + " Branch #" + dec(fault.elementId+1) + "Marker #" + dec(fault.index) + " (ref. sample: " + Sample2String(fault.refSample) + ")";
		default:
			return String("");
		}
	}

	void Print(std::ostream& s) const
	{
		s << "Warnings: " << m_faults.size() << std::endl;

		for(u32 n=0;n<m_faults.size();++n)
			s << "\tFault #" << PrependSpaces(dec(n+1), 3) << ": " << FaultToString(m_faults[n]) << std::endl;
	}

	void Print(Output* pOut) const
	{
		pOut->writeLine( "Warnings: " + dec( (u32) m_faults.size()) );

		for(u32 n=0;n<m_faults.size();++n)
			pOut->writeLine("\tFault #" + PrependSpaces(dec(n+1), 3) + ": " + FaultToString(m_faults[n]) );
	}
};

#endif //MORPHOLOGYDATAFAULTS_HPP__
