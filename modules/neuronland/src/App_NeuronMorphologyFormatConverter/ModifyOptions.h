#ifndef _NLMC_MODIFYOPTIONS_HPP_
#define _NLMC_MODIFYOPTIONS_HPP_


/** 

	Specifies how to modify a neuron
	
*/

enum SomaReplacement
{
	kSomaOriginal,
	kSomaForceSphere,
	kSomaForceCylinder
};

struct ModifyOptions
{
	bool				omitAllSomas;
	bool				omitAllAxons;
	bool				omitAllDends;

	bool				omitGlobalMarkers;
	bool				omitSomaMarkers;
	bool				omitDendMarkers;
	bool				omitAxonMarkers;

	bool				omitGlobalSpines;
	bool				omitDendSpines;
	bool				omitAxonSpines;

	bool				allAxonsAsDends;
	bool				allDendsAsAxons;
	bool				reverseAllTrees;

	bool				allMarkersToSpines;
	bool				someMarkersToSpines;
	bool				spineToMarker;
	bool				mapGlobalSpinesToBranches;
	bool				mapGlobalMarkersToBranches;

	bool				treeSpinesToBranches;

	bool				forceSpinesToBeGlobal;
	bool				forceMarkersToBeGlobal;

	SomaReplacement		somaReplacementType;
	float				somaDiameter;
	float				somaLength;

	bool 				rotateNeuron;
	bool 				translateNeuron;
	bool 				scaleNeuron;
	bool				moveOriginToSoma;

	std::vector<u32>	somasOmitted;
	std::vector<u32>	treesOmitted;
	std::vector<u32>	treesToFlip;
	std::vector<u32>	treesToReverse;
	v4f 				rotation;
	v4f 				translation;
	v4f 				scale;

	float				spineMapLimit;
	float				markerMapLimit;
	String				spineMarkerName;
	std::vector<String> markerNamesAsSpines;
	std::map<String, String> markerRenameMap;

	ModifyOptions()
	{
		omitAllSomas		= false;
		omitAllAxons		= false;
		omitAllDends		= false;

		omitGlobalMarkers	= false;
		omitSomaMarkers		= false;
		omitDendMarkers		= false;
		omitAxonMarkers		= false;

		omitGlobalSpines	= false;
		omitDendSpines		= false;
		omitAxonSpines		= false;

		allAxonsAsDends		= false;
		allDendsAsAxons		= false;
		reverseAllTrees		= false;

		allMarkersToSpines	= false;
		someMarkersToSpines	= false;
		spineToMarker		= false;
		mapGlobalSpinesToBranches = false;
		mapGlobalMarkersToBranches = false;

		treeSpinesToBranches = false;

		forceSpinesToBeGlobal = false;
		forceMarkersToBeGlobal = false;

		rotateNeuron		= false;
		translateNeuron 	= false;
		scaleNeuron			= false;
		moveOriginToSoma	= false;

		somaReplacementType = kSomaOriginal;
	}
};



#endif // _NLMC_MODIFYOPTIONS_HPP_
