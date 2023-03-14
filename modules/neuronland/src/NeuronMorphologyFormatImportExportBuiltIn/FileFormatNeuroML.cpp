//
//
//

#include "ImportExportCommonIncludes.h"

#include <map>

#include "TinyXML/tinyxml.h"


/**

	Import/Export NeuroML (NeuroML Level 1, MorphML) data files.

	Can import/export any version from 1.0 to 2.0.

	Structure supported ->
		Soma		: YES (P,S)
		Dendrite	: YES
		Axon		: YES
		Marker		: YES (G)
		Spine		: YES (G,B)

	Pending:
	* Soma - no support for closed contours.
	* Some soma data will be discarded if there is a mixture of e.g. contours and segment based data

	Disabled:
	* Some code to write tree data as paths, but not strictly correct, as paths are for extracellular features.

**/


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

struct SegmentData 
{
	int			id;
	int			parent;
	int			cable;
	SamplePoint proximal;
	SamplePoint distal;
	//String		name;
	bool		attached;

	// filled in when connecting
	MorphologyBuilder::Branch	branch;
	u32							sampleOffset;


	SegmentData()
	{
		attached = false;
		branch = MorphologyBuilder::NullBranch();
		sampleOffset = 0;
	}
};

enum MorphMLStructureType 
{
	kStructureSoma,
	kStructureAxon,
	kStructureDend
};

struct CableData
{
	int id;
	MorphMLStructureType type;
	String name;
};

typedef std::map<int, SegmentData> SegmentDataList;
typedef std::map<int, CableData> CableDataList;

static const HString kBranchID("id");


static int FindUnattachedChildSegment(SegmentDataList& segments, int seg)
{
	for(SegmentDataList::iterator it = segments.begin();it != segments.end();++it)
	{
		if( (!it->second.attached) && 
			(it->second.parent == seg) )
		{
			return it->second.id;
		}
	}

	return -1;
}

static bool ConnectSomaSegments(MorphologyBuilder& builder, SegmentDataList& segments, CableDataList& cables, int startSegment)
{
	builder.NewSomaSegment();
	builder.CsAddSample( segments[startSegment].proximal );
	builder.CsAddSample( segments[startSegment].distal );
	segments[startSegment].attached = true;

	int currentSegId = startSegment;

	bool found = false;

	do
	{
		found = false;

		for(SegmentDataList::iterator it = segments.begin();it != segments.end();++it)
		{
			if(it->second.parent == currentSegId && cables[it->second.cable].type == kStructureSoma)
			{
				if(it->second.attached)
				{
					LogImportFailure("Soma segment already attached!");
					return false;
				}

				it->second.attached = true;

				builder.CsAddSample(it->second.distal);
				found = true;
				currentSegId = it->second.id;
				break;
			}
		}
	}
	while(found);

	return true;
}

static bool ConnectTreeSegments(MorphologyBuilder& builder, SegmentDataList& segments, CableDataList& cables, int startSegment, MorphMLStructureType type)
{
	segments[startSegment].attached = true;
	builder.CbAddSample( segments[startSegment].distal );

	segments[startSegment].branch = builder.CurrentBranch();
	segments[startSegment].sampleOffset = builder.CurrentBranch().NumSamples() - 1;

	while(1)
	{
		int unattachedChildSegment = FindUnattachedChildSegment(segments, startSegment);

		if(unattachedChildSegment == -1)
			break;

		MorphMLStructureType cabletype = cables[segments[unattachedChildSegment].cable].type;

		if(cabletype != type)
		{
			LogImportFailure("Unexpected segment type: " + dec(cabletype) );
			return false;
		}

		builder.CbNewChildLast(true);
		ConnectTreeSegments(builder, segments, cables, unattachedChildSegment, type);
		builder.CbToParent();
	}

	return true;
}

static bool ConnectSegments(MorphologyBuilder& builder, SegmentDataList& segments, CableDataList& cables)
{
	// find soma/axon/dendrite segments with no parent, or axon/dendrite segments connected to soma, and build structure from these key starting points.
	for(SegmentDataList::const_iterator it = segments.begin();it != segments.end();++it)
	{
		const int segmentId = (*it).second.id;
		const int cableId	= (*it).second.cable;
		const int parentId	= (*it).second.parent;

		if(cables.find(cableId) == cables.end())
		{
			LogImportFailure("Can't find expected cable: " + dec(cableId) );
			return false;
		}

		const MorphMLStructureType currentType = cables[cableId].type;

		if(parentId == -1)
		{
			// must be soma or dendrite root segment
			switch(currentType)
			{
			case kStructureSoma:
				if(!ConnectSomaSegments(builder, segments, cables, segmentId))
					return false;
				break;
			case kStructureAxon:
				builder.NewAxonWithSample( (*it).second.proximal );
				if(!ConnectTreeSegments(builder, segments, cables, segmentId, currentType))
					return false;
				break;
			case kStructureDend:
				builder.NewDendriteWithSample( (*it).second.proximal );
				if(!ConnectTreeSegments(builder, segments, cables, segmentId, currentType))
					return false;
				break;
			}
		}
		else
		{
			const int parentCableId = segments[parentId].cable;
			if(cables.find(parentCableId) == cables.end())
			{
				LogImportFailure("Can't find expected parent cable: " + dec(parentCableId) );
				return false;
			}

			const MorphMLStructureType parentType = cables[parentCableId].type;

			if(parentType != currentType)
			{
				if(parentType != kStructureSoma)
				{
					LogImportFailure("Unexpected parent structure type: " + dec(parentType));
				}

				switch(currentType)
				{
				case kStructureAxon:
					builder.NewAxonWithSample( (*it).second.proximal );
					if(!ConnectTreeSegments(builder, segments, cables, segmentId, currentType))
						return false;
					break;
				case kStructureDend:
					builder.NewDendriteWithSample( (*it).second.proximal );
					if(!ConnectTreeSegments(builder, segments, cables, segmentId, currentType))
						return false;
					break;
				}
			}
		}
	}

	// check nothing left over..
	for(SegmentDataList::const_iterator it = segments.begin();it != segments.end();++it)
	{
		if(it->second.attached == false)
		{
			LogImportWarning("Segment could not be incorporated into neuron");
		}
	}

	return true;
}

template<typename T>
static void ExtractPointAttributes(const TiXmlElement* sampleElem, T& sample)
{
	const char* attrX = sampleElem->Attribute("x");
	const char* attrY = sampleElem->Attribute("y");
	const char* attrZ = sampleElem->Attribute("z");
	const char* attrD = sampleElem->Attribute("diameter");

	assert(attrX);
	assert(attrY);
	assert(attrZ);
	assert(attrD);

	sample.x = (float)atof(attrX);
	sample.y = (float)atof(attrY);
	sample.z = (float)atof(attrZ);
	sample.d = (float)atof(attrD);
}

static void ExtractSomaPolygon(MorphologyBuilder& builder, const TiXmlElement* polygon)
{
	const TiXmlElement* point = polygon->FirstChildElement("point");

	if(point)
		builder.NewSomaClosedContour();

	while(point)
	{
		SamplePoint sp;
		ExtractPointAttributes(point, sp);

		builder.CsAddSample(sp);

		point = point->NextSiblingElement();
	}
}

static bool ExtractSoma(MorphologyBuilder& builder, const TiXmlElement* cellElem)
{
	// test both names (
	const TiXmlElement* cellbodyElem = cellElem->FirstChildElement("cellBody");
	if(!cellbodyElem)
		cellbodyElem = cellElem->FirstChildElement("cell_body");
	//

	if(cellbodyElem)
	{
		const TiXmlElement* polyhedronElem = cellbodyElem->FirstChildElement("polyhedron");
		const TiXmlElement* polygonElem = cellbodyElem->FirstChildElement("polygon");
		const TiXmlElement* sphereElem = cellbodyElem->FirstChildElement("sphere");

		if(polyhedronElem)
		{
			const TiXmlElement* polygonsElem = polyhedronElem->FirstChildElement("polygons");
			assert(polygonsElem);

			const TiXmlElement* polygonElem = polygonsElem->FirstChildElement("polygon");
			while(polygonElem)
			{
				ExtractSomaPolygon(builder, polygonElem);
				polygonElem = polygonElem->NextSiblingElement("polygon");
			}
		}
		else if(polygonElem)
		{
			ExtractSomaPolygon(builder, polygonElem->ToElement());
		}
		else if(sphereElem)
		{
			const TiXmlElement* centerElem = sphereElem->FirstChildElement("center");
			assert(centerElem);

			SamplePoint sp;
			ExtractPointAttributes(centerElem, sp);
			builder.NewSomaSinglePoint(sp);
		}
	}

	return true;
}

static bool ExtractSegments(const TiXmlNode* cellNode, SegmentDataList& segments)
{
	const TiXmlElement* segmentsElem = cellNode->FirstChildElement("segments");
	if(!segmentsElem)
		return false;

	while(segmentsElem)
	{
		// v. 1.7.3 and later should only have 1 segments tag.

		const TiXmlElement* segmentElem = segmentsElem->FirstChildElement("segment");
		while(segmentElem)
		{
			// segment properties and connectivity
			const char* attrId		= segmentElem->Attribute("id");
			const char* attrName	= segmentElem->Attribute("name");
			const char* attrParent	= segmentElem->Attribute("parent");
			const char* attrCable	= segmentElem->Attribute("cable");

			assert(attrId);
			//assert(attrName);

			SegmentData seg;

			// warning: assuming integer!! - could possible not be!?? 
			seg.id		= atoi(attrId);
//			seg.name	= attrName ? String(attrName) : String("");
			seg.parent	= attrParent ? atoi(attrParent) : -1;
			seg.cable	= attrCable ? atoi(attrCable) : -1;
			seg.attached = false;

			// proximal ?
			const TiXmlElement* proximalElem = segmentElem->FirstChildElement("proximal");
			if(proximalElem)
				ExtractPointAttributes(proximalElem, seg.proximal);

			// distal ?
			const TiXmlElement* distalElem = segmentElem->FirstChildElement("distal");
			if(distalElem)
				ExtractPointAttributes(distalElem, seg.distal);

			segments[seg.id] = seg;

			segmentElem = segmentElem->NextSiblingElement("segment");
		}

		segmentsElem = segmentsElem->NextSiblingElement("segments");
	}

	return true;
}


static bool ExtractCables(const TiXmlNode* parent, CableDataList& cables, const String& cableGroupName, const String& dendriteHint, const String& axonHint, const String& somaHint)
{
	// cables
	const TiXmlElement* cableElem = parent->FirstChildElement("cable");
	while(cableElem)
	{
		const char* attrId		= cableElem->Attribute("id");
		const char* attrName	= cableElem->Attribute("name");

		assert(attrId);
		//assert(attrName);

		CableData cab;

		cab.id		= atoi(attrId);
		cab.name	= attrName ? String(attrName) : String("");
		cab.type	= kStructureDend;

		// meta:groups

		const TiXmlElement* child = cableElem->FirstChildElement("meta:group");
		while(child)
		{
			String text( child->GetText() );
			if( StringContains(text, dendriteHint) )
			{
				cab.type = kStructureDend;
				break;
			}
			else if(StringContains(text, axonHint) )
			{
				cab.type = kStructureAxon;
				break;
			}
			else if(StringContains(text, somaHint) )
			{
				cab.type = kStructureSoma;
				break;
			}

			child = child->NextSiblingElement("meta:group");
		}

		cables[cab.id] = cab;

		cableElem = cableElem->NextSiblingElement("cable");
	}

	return true;
};

static bool ExtractCablesAndCableGroups(const TiXmlNode* parent, CableDataList& cables, const String& dendriteHint, const String& axonHint, const String& somaHint)
{
	const TiXmlNode* cablesNode = parent->FirstChild("cables");

	if(cablesNode)
	{
		// cablegroup (from v. 1.4)
		const TiXmlElement* cableGroupElem = cablesNode->FirstChildElement("cablegroup");

		if(cableGroupElem)
		{
			String cableGroupName = cableGroupElem->Attribute("name");
			while(cableGroupElem)
			{
				// get name of group...
				ExtractCables(cableGroupElem, cables, cableGroupName, dendriteHint, axonHint, somaHint);

				cableGroupElem = cableGroupElem->NextSiblingElement("cablegroup");
			}
		}

		ExtractCables(cablesNode, cables, "", dendriteHint, axonHint, somaHint);
	}

	// should only be 1 cables element 

	return true;
}

SpineType String2Shape(const String& s)
{
	if(s == "mushroom")
		return kSpineMushroom;
	else if(s == "stubby")
		return kSpineStubby;
	else if(s == "thin")
		return kSpineThin;
	else
		return kSpineNoDetail;
}

static bool ExtractSpines(MorphologyBuilder& builder, const TiXmlElement* cellNode, SegmentDataList& segments)
{
	const TiXmlElement* spinesElem = cellNode->FirstChildElement("spines");

	if(spinesElem)
	{
		const TiXmlElement* spineElem = spinesElem->FirstChildElement("spine");

		while(spineElem)
		{
			const TiXmlElement* proxElem = spineElem->FirstChildElement("proximal");
			const TiXmlElement* distElem = spineElem->FirstChildElement("distal");

			SpinePoint sp;

			if(distElem)
				ExtractPointAttributes(distElem, sp);
			else if(proxElem)
				ExtractPointAttributes(proxElem, sp);

			const char* parentStr = spineElem->Attribute("parent");
			const char* shapeStr = spineElem->Attribute("shape");

			if(shapeStr)
				sp.type = String2Shape( shapeStr );
			else 
				sp.type = kSpineNoDetail;

			if(parentStr)
			{
				// spine corresponds to specific branch segment - so find that segment !!
				int parentSegId = atoi(parentStr);

				sp.associatedSegment = segments[parentSegId].sampleOffset;

				if(!segments[parentSegId].branch)
				{
					LogImportFailure("Cannot find branch to which spine maps!!");
					return false;
				}

				segments[parentSegId].branch.AddSpine(sp);
			}
			else
				builder.AddSpine(sp);


			spineElem = spineElem->NextSiblingElement("spine");
		}

		// todo: need to sort spines here ?

	}

	return true;
}

static bool ExtractFreePoints(MorphologyBuilder& builder, const TiXmlElement* cellNode)
{
	const TiXmlElement* pointsElem = cellNode->FirstChildElement("free_points");
	if(!pointsElem)
		pointsElem = cellNode->FirstChildElement("freePoints");

	while(pointsElem)
	{
		const char* name = pointsElem->Attribute("name");
		String nameStr = name ? name : "unnamed";

		const TiXmlElement* pointElem = pointsElem->FirstChildElement("point");

		MarkerPoint mp;

		while(pointElem)
		{
			ExtractPointAttributes(pointElem, mp);
			builder.AddMarker(nameStr, mp);

			pointElem = pointElem->NextSiblingElement("point");
		}

		
		//
		const TiXmlElement* potentialPointsElem = pointsElem->NextSiblingElement("free_points");
		if(!potentialPointsElem)
			potentialPointsElem = pointsElem->NextSiblingElement("freePoints");

		pointsElem = potentialPointsElem;
	}

	return true;
}

bool ImportNeuroML(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	String somaGroupHint = options.hasHintSoma ? options.hintSomaSubString : "soma";
	String dendriteGroupHint = options.hasHintDend ? options.hintDendSubString : "dendrite";
	String axonGroupHint = options.hasHintAxon ? options.hintAxonSubString : "axon";

	// lets put the entire file in memory so we can directly Parse() it
	size_t size = rIn.remaining();
	char* buffer = new char[size+1];
	rIn.read(buffer, size);
	buffer[size] = 0;

	TiXmlDocument document;
	document.Parse(buffer);

	delete [] buffer;


	// navigate the DOM
	const TiXmlElement* root = document.RootElement();
	const TiXmlNode* cellsNode = root->FirstChild("cells");


	// max 1 "cells"
	if(cellsNode)
	{
		const TiXmlNode* cellNode = cellsNode->FirstChild("cell");
		const TiXmlElement* cellElem = cellNode->ToElement();

		SegmentDataList segments;
		CableDataList	cables;

		// only extracts the first "cell"
		if(cellNode)
		{
			if(!ExtractSegments(cellNode, segments))
				return false;

			if(!ExtractCablesAndCableGroups(cellNode, cables, dendriteGroupHint, axonGroupHint, somaGroupHint) )
				return false;
		}
		else
			return false;

		MorphologyBuilder builder(nrn);

		if(!ConnectSegments(builder, segments, cables))
			return false;

		if(!ExtractSoma(builder, cellElem))
			return false;

		if(!ExtractFreePoints(builder, cellElem))
			return false;

		if(!ExtractSpines(builder, cellElem, segments))
			return false;


		builder.ApplyChanges();
		nrn.MergeAllUnaryBranchPoints(); // important


		// rescale all positions and lengths (only necessary if not in micrometers)
		const char* unitsStr = 0;
		unitsStr = root->Attribute("length_units"); // v. 1.8.0 and later
		if(!unitsStr)
			unitsStr = root->Attribute("lengthUnits");

		if(unitsStr)
		{
			String units(unitsStr);
			if(units == "millimeter" || units == "millimetre") // "..er" from v. 1.8.1
				nrn.Scale( v4f(1e3, 1e3, 1e3, 1e3) );
			else if(units == "meter" || units == "metre")
				nrn.Scale( v4f(1e6, 1e6, 1e6, 1e6) );
		}
		//

		return true;
	}
	else
		return false;
}


bool ImportRawNeuroML(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportNeuroML(*pIn, nrn, options);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

struct SegCabIds
{
	u32 segmentId;
	u32 cableId;
};

static bool NeuronHasSuitableSomaData(const Neuron3D &nrn)
{
	return nrn.CountSomas() > 0;
}

static bool NeuronHasSuitableSpineData(const Neuron3D &nrn)
{
	return nrn.HasSpines();
}

static bool NeuronHasSuitableFreePointData(const Neuron3D &nrn)
{
	return nrn.HasMarkers();
}

static void WriteCable(TiXmlElement* cables, int cableId, String /*cableName*/, const String& groupname)
{
	TiXmlElement* cable = new TiXmlElement("cable");
	cable->SetAttribute("id", cableId);
	//cable->SetAttribute("name", cableName);
	cables->LinkEndChild(cable);

	TiXmlElement* group1 = new TiXmlElement("meta:group");
	cable->LinkEndChild(group1);
	TiXmlText* text1 = new TiXmlText("all");
	group1->LinkEndChild(text1);

	TiXmlElement* group2 = new TiXmlElement("meta:group");
	cable->LinkEndChild(group2);

	TiXmlText* text2 = new TiXmlText( groupname.c_str() );
	group2->LinkEndChild(text2);
}

template<typename T>
static void AddSamplePointToSegment(TiXmlElement* segment, String name, const T& sample, bool useDiam = true)
{
	TiXmlElement* elem = new TiXmlElement(name.c_str());
	elem->SetAttribute("x", dec(sample.x, 2).c_str());
	elem->SetAttribute("y", dec(sample.y, 2).c_str());
	elem->SetAttribute("z", dec(sample.z, 2).c_str());
	elem->SetAttribute("diameter", dec(sample.d, 2).c_str());
	segment->LinkEndChild(elem);
}

template<typename T>
static void WritePoints(TiXmlElement* parent, T& points)
{
	const u32 numPoints = points.size();
	for(u32 n=0;n<numPoints;++n)
		AddSamplePointToSegment(parent, String("point"), points[n]);
}

static void WriteMarkerPoints(TiXmlElement* parent, Branch3D::Markers markers)
{
	for(Branch3D::Markers::const_iterator it=markers.begin();it!=markers.end();++it)
	{
		String markerName = it->first;
		for(u32 n=0;n<it->second.size();++n)
			WritePoints(parent, it->second);
	}
}

static void WriteSegment(TiXmlElement* segments, int segmentId, String /*segmentName*/, int parentId, int cableId, const SamplePoint* proximal, const SamplePoint* distal)
{
	TiXmlElement* segment = new TiXmlElement("segment");

	segment->SetAttribute("id", segmentId);
	//segment->SetAttribute("name", segmentName);
	if(parentId >= 0) segment->SetAttribute("parent", parentId);
	segment->SetAttribute("cable", cableId);

	if(proximal)
		AddSamplePointToSegment(segment, String("proximal"), *proximal);

	if(distal)
		AddSamplePointToSegment(segment, String("distal"), *distal);

	segments->LinkEndChild(segment);
}

static void WriteSpine(TiXmlElement* spines, const SpinePoint& sp, int segmentOffset)
{
	TiXmlElement* spine = new TiXmlElement("spine");

	if(segmentOffset != -1)
	{
		// compensate for possible 0 associated segment for root branch...
		const u32 segment = (sp.associatedSegment == 0 ? 1 : sp.associatedSegment);
		//

		spine->SetAttribute("parent", dec(segmentOffset + segment - 1).c_str() );
	}

	switch(sp.type)
	{
	case kSpineMushroom:
		spine->SetAttribute("shape", "mushroom");
		break;
	case kSpineStubby:
		spine->SetAttribute("shape", "stubby");
		break;
	case kSpineThin:
		spine->SetAttribute("shape", "thin");
		break;
	default:
		// just don't bother setting this attribute
		break;
	}

	AddSamplePointToSegment(spine, String("proximal"), sp);

	spines->LinkEndChild(spine);
}

static void WriteSpines(TiXmlElement* spines, const std::vector<SpinePoint>& vs, int segmentOffset)
{
	for(u32 n=0;n<vs.size();++n)
		WriteSpine(spines, vs[n], segmentOffset);
}

static SegCabIds WriteBranchSegmentsCablesSpinesPoints(TiXmlElement* segments, TiXmlElement* cables, TiXmlElement* spines, TiXmlElement* points, Neuron3D::DendriteTreeConstIterator it, SegCabIds ids, int parentId, const String& groupname)
{
	const int firstSegmentId = ids.segmentId;

	int localParentId = parentId;
	for(u32 n=1;n<(*it).m_samples.size();++n)
	{
		WriteSegment(segments, ids.segmentId, String(""/*segmentName*/), localParentId, ids.cableId, (n==1 ? &((*it).m_samples[n-1]) : 0),  &((*it).m_samples[n]) );
		localParentId = ids.segmentId;
		++(ids.segmentId);
	}

	if(spines)
		WriteSpines(spines, (*it).m_spines, firstSegmentId);

	if(points)
		WriteMarkerPoints(points, (*it).m_markers);

	WriteCable(cables, ids.cableId, String(""/*cableName*/), groupname);
	++(ids.cableId);

	if(it.child())
		ids = WriteBranchSegmentsCablesSpinesPoints(segments, cables, spines, points, it.child(), ids, localParentId, groupname);
	if(it.peer())
		ids = WriteBranchSegmentsCablesSpinesPoints(segments, cables, spines, points, it.peer(), ids, parentId, groupname);

	return ids;
}

static void WriteSegmentsCablesSpinesPoints(TiXmlElement* segments, TiXmlElement* cables, TiXmlElement* spines, TiXmlElement* points, const Neuron3D &nrn, const String& somaGroupName, const String& dendGroupName, const String& axonGroupName)
{
	SegCabIds ids = {0, 0};

	int parentId = -1;

	// global data
	if(spines)
		WriteSpines(spines, nrn.m_globalData.m_spines, -1);

	if(points)
		WriteMarkerPoints(points, nrn.m_globalData.m_markers);

	// soma segments ?
#if 0
	for(Neuron3D::SomaConstIterator s = nrn.SomaBegin(); s != nrn.SomaEnd(); ++s)
	{
		if(s->somaType == Soma3D::kSegment)
		{
			const u32 numSamples = nrn.SomaBegin()->m_samples.size();
			if(numSamples > 1)
			{
				for(u32 n=1;n<numSamples;++n)
				{
					WriteSegment(segments, ids.segmentId, String(""/*segmentName*/), parentId, ids.cableId, (n==1 ? &(nrn.SomaBegin()->m_samples[n-1]) : 0), &(nrn.SomaBegin()->m_samples[n]));
					parentId = ids.segmentId;
					++(ids.segmentId);
				}
			}
			else
			{
				WriteSegment(segments, ids.segmentId, String(""/*segmentName*/), parentId, ids.cableId, &(nrn.SomaBegin()->m_samples[0]), &(nrn.SomaBegin()->m_samples[0]));
			}

			WriteCable(cables, ids.cableId, String(""), somaGroupName);
			++(ids.cableId);

			// we only allow 1 segment-based soma
			break;
		}

		if(points)
		{
			WriteMarkerPoints(points, s->m_markers);
		}
	}
#endif


	// dendrites
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		ids = WriteBranchSegmentsCablesSpinesPoints(segments, cables, spines, points, (*d).root(), ids, parentId, dendGroupName);
	}

	// axons
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		ids = WriteBranchSegmentsCablesSpinesPoints(segments, cables, spines, points, (*a).root(), ids, parentId, axonGroupName);
	}
}


/** 
	NeuroML V2 experimental support
*/

#if 0
static SegCabIds WriteBranchSegmentGroupsV2(TiXmlElement* segments, Neuron3D::DendriteTreeConstIterator it, SegCabIds ids, int parentId, const String& groupname)
{
	const int firstSegmentId = ids.segmentId;

	int localParentId = parentId;
	for(u32 n=1;n<(*it).m_samples.size();++n)
	{
		WriteSegmentV2(segments, ids.segmentId, String(""/*segmentName*/), localParentId, ids.cableId, (n==1 ? &((*it).m_samples[n-1]) : 0),  &((*it).m_samples[n]) );
		localParentId = ids.segmentId;
		++(ids.segmentId);
	}

	if(it.child())
		ids = WriteBranchSegmentsV2(segments, it.child(), ids, localParentId, groupname);
	if(it.peer())
		ids = WriteBranchSegmentsV2(segments, it.peer(), ids, parentId, groupname);

	return ids;
}

static void WriteSegmentGroupsV2(TiXmlElement* segments, const Neuron3D &nrn, const String& somaGroupName, const String& dendGroupName, const String& axonGroupName)
{
	SegCabIds ids = {0, 0};

	int parentId = -1;

	// dendrites
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		ids = WriteBranchSegmentGroupsV2(segments, (*d).root(), ids, parentId, dendGroupName);
	}

	// axons
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		ids = WriteBranchSegmentGroupsV2(segments, (*a).root(), ids, parentId, axonGroupName);
	}
}


static void WriteSegmentV2(TiXmlElement* segments, int segmentId, String /*segmentName*/, int parentId, int cableId, const SamplePoint* proximal, const SamplePoint* distal)
{
	TiXmlElement* segment = new TiXmlElement("segment");

	segment->SetAttribute("id", segmentId);
	//segment->SetAttribute("name", segmentName);

	if(parentId >= 0)
	{
		TiXmlElement* parent = new TiXmlElement("parent");
		parent->SetAttribute("segment", parentId);
		segment->LinkEndChild(parent);
	}

	if(proximal)
		AddSamplePointToSegment(segment, String("proximal"), *proximal);

	if(distal)
		AddSamplePointToSegment(segment, String("distal"), *distal);

	segments->LinkEndChild(segment);
}


static SegCabIds WriteBranchSegmentsV2(TiXmlElement* segments, Neuron3D::DendriteTreeConstIterator it, SegCabIds ids, int parentId, const String& groupname)
{
	const int firstSegmentId = ids.segmentId;

	int localParentId = parentId;
	for(u32 n=1;n<(*it).m_samples.size();++n)
	{
		WriteSegmentV2(segments, ids.segmentId, String(""/*segmentName*/), localParentId, ids.cableId, (n==1 ? &((*it).m_samples[n-1]) : 0),  &((*it).m_samples[n]) );
		localParentId = ids.segmentId;
		++(ids.segmentId);
	}

	if(it.child())
		ids = WriteBranchSegmentsV2(segments, it.child(), ids, localParentId, groupname);
	if(it.peer())
		ids = WriteBranchSegmentsV2(segments, it.peer(), ids, parentId, groupname);

	return ids;
}

static void WriteSegmentsAndGroupsV2(TiXmlElement* segments, const Neuron3D &nrn, const String& somaGroupName, const String& dendGroupName, const String& axonGroupName)
{
	SegCabIds ids = {0, 0};

	std::vector<int> somaSegments;
	std::vector<int> dendSegments;
	std::vector<int> axonSegments;

	int parentId = -1;

	// First write segments, making note of the 
	const u32 somaStartId = ids.segmentId;
#if 0
	for(Neuron3D::SomaConstIterator s = nrn.SomaBegin(); s != nrn.SomaEnd(); ++s)
	{
		if(s->somaType == Soma3D::kSegment)
		{
			const u32 numSamples = nrn.SomaBegin()->m_samples.size();
			if(numSamples > 1)
			{
				for(u32 n=1;n<numSamples;++n)
				{
					WriteSegment(segments, ids.segmentId, String(""/*segmentName*/), parentId, ids.cableId, (n==1 ? &(nrn.SomaBegin()->m_samples[n-1]) : 0), &(nrn.SomaBegin()->m_samples[n]));
					parentId = ids.segmentId;
					++(ids.segmentId);
				}
			}
			else
			{
				WriteSegment(segments, ids.segmentId, String(""/*segmentName*/), parentId, ids.cableId, &(nrn.SomaBegin()->m_samples[0]), &(nrn.SomaBegin()->m_samples[0]));
			}

			// we only allow 1 segment-based soma
			break;
		}
	}
#endif
	const u32 somaEndId = ids.segmentId;


	// dendrites
	const u32 dendriteStartId = ids.segmentId;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		ids = WriteBranchSegmentsV2(segments, (*d).root(), ids, parentId, dendGroupName, dendSegments, spineSegments);
	}
	const u32 dendriteEndId = ids.segmentId;

	// axons
	const u32 axonStartId = ids.segmentId;
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		ids = WriteBranchSegmentsV2(segments, (*a).root(), ids, parentId, axonGroupName, axonSegments, spineSegments);
	}
	const u32 axonEndId = ids.segmentId;


	WriteSegmentGroup("soma_group", dendSegments);
	WriteSegmentGroup("dendrite_group", dendSegments);
	WriteSegmentGroup("axon_group", axonSegments);

	WriteSegmentGroup("dendrite_N", subTree...);
}
#endif

static void WriteCellBody(TiXmlElement* cellBody, const Neuron3D &nrn)
{
	assert( NeuronHasSuitableSomaData(nrn) );

	u32 numSphere = 0;
	u32 numPolygon = 0;
	u32 numSegment = 0;

	// lets see if we can make a sensible decision about how to output the soma
	for(Neuron3D::SomaConstIterator s = nrn.SomaBegin(); s != nrn.SomaEnd(); ++s)
	{
		const Soma3D& soma = (*s);

		if(soma.m_samples.size() == 1)
			++numSphere;
		else
			++numPolygon;
	}

	// so what shall we do ?
	if(numPolygon > 1)
	{
		// polyhedron
		TiXmlElement* polyhedron = new TiXmlElement("polyhedron");
		cellBody->LinkEndChild(polyhedron);

		TiXmlElement* polygons = new TiXmlElement("polygons");
		polyhedron->LinkEndChild(polygons);

		for(Neuron3D::SomaConstIterator s = nrn.SomaBegin(); s != nrn.SomaEnd(); ++s)
		{
			const Soma3D& soma = (*s);

			if(soma.m_samples.size() > 1)
			{
				TiXmlElement* polygon = new TiXmlElement("polygon");
				polygons->LinkEndChild(polygon);

				WritePoints(polygon, soma.m_samples);
			}
		}
	}
	else if(numPolygon == 1)
	{
		// polygon
		for(Neuron3D::SomaConstIterator s = nrn.SomaBegin(); s != nrn.SomaEnd(); ++s)
		{
			const Soma3D& soma = (*s);

			if(soma.m_samples.size() > 1)
			{
				TiXmlElement* polygon = new TiXmlElement("polygon");
				cellBody->LinkEndChild(polygon);
				WritePoints(polygon, soma.m_samples);
				break;
			}
		}
	}
	else if(numSphere == 1)
	{
		for(Neuron3D::SomaConstIterator s = nrn.SomaBegin(); s != nrn.SomaEnd(); ++s)
		{
			const Soma3D& soma = (*s);

			if(soma.m_samples.size() == 1)
			{
				TiXmlElement* sphere = new TiXmlElement("sphere");
				cellBody->LinkEndChild(sphere);

				AddSamplePointToSegment(sphere, String("center"), soma.m_samples[0]);
				break;
			}
		}
	}

	// anything else will just be represented directly as segments/cables, or nothing at all...
}

const char* versionNumbers[kVersion_Num] = 
{
	"1.0", "1.1", "1.2", "1.3", "1.4", "1.5", "1.6", "1.7", "1.7.1", "1.7.2", "1.7.3", "1.8.0", "1.8.1", "1.8.2", "2.0"
};


bool ExportNeuroML(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	const NeuroMLVersion version	= options.versionId;
	const bool identifyNL			= options.identifyNL;

	String somaGroupName = options.customiseSomaName ? options.somaName : "soma_group";
	String dendGroupName = options.customiseDendName ? options.dendName : "dendrite_group";
	String axonGroupName = options.customiseAxonName ? options.axonName : "axon_group";


	const bool canUseCableGroup			= (version >= kVersion_14);
	const bool usePre173CellOrder		= (version < kVersion_173);
	const bool useLengthUnits			= (version < kVersion_180);
	const bool useMicron				= (version < kVersion_181);
	const bool useCellBody				= (version < kVersion_200);
	const bool useFreePoints			= (version < kVersion_200);
	const bool useMorphMLNamespace		= (version < kVersion_200);
	const bool exactlyOneSegments		= (version >= kVersion_173);
	const bool neuroMLV2				= (version >= kVersion_200);


	String versionNumber = ConvertNeuroMLVersion2String(version);

	// xml 
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "");
	doc.LinkEndChild( decl );

	// morphml

	const String rootName = true ? "morphml" : "neuroml";

	TiXmlElement* morphML = new TiXmlElement(rootName.c_str());

	if(useMorphMLNamespace)
	{
		morphML->SetAttribute("xmlns", "http://morphml.org/morphml/schema");
		morphML->SetAttribute("xmlns:meta", "http://morphml.org/metadata/schema");
		morphML->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
		morphML->SetAttribute("xsi:schemaLocation", "http://morphml.org/morphml/schema MorphML.xsd");
	}
	else
	{
		// v2 alpha
		morphML->SetAttribute("xmlns", "http://neuroml.org/schema/neuroml2");
		morphML->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
		morphML->SetAttribute("xsi:schemaLocation", "http://www.neuroml.org/schema/neuoml2  ../Schemas/NeuroML2/NeuroML_v2alpha.xsd");
		//morphML->SetAttribute("id", options.cellName);
	}

	morphML->SetAttribute( useLengthUnits ? "lengthUnits" : "length_units", useMicron ? "micron" : "micrometer");
	doc.LinkEndChild(morphML);

	// which software
	TiXmlElement* notes = new TiXmlElement("meta:notes");

	String text("Level 1 NeuroML file (MorphML " + versionNumber + ")");
	if(identifyNL)
		text += String(" - generated by ") + options.appName + " (version " + options.appVersion + "). ";
	if(options.addCommentText)
		text += "\n" + options.commentText;

	TiXmlText* notesText = new TiXmlText(text.c_str());
	notes->LinkEndChild(notesText);
	morphML->LinkEndChild(notes);

	// high level tags
#if 0
	if(neuroMLV2)
	{
		TiXmlElement* cell = new TiXmlElement("cell");
		cell->SetAttribute("id", options.cellName);
		morphML->LinkEndChild(cell);

		TiXmlElement* morphology = new TiXmlElement("morphology");
		morphology->SetAttribute("id", (const char*)String((String(options.cellName) + String("_Morpology"))) );
		cell->LinkEndChild(morphology);

		// connectivity and sample data, and also spine and marker data at the same time. *** No longer export local marker data ***
		WriteSegmentsAndGroupsV2(morphology, nrn, somaGroupName, dendGroupName, axonGroupName);
	}
	else
#endif
	{
		TiXmlElement* cells = new TiXmlElement("cells");
		morphML->LinkEndChild(cells);
		TiXmlElement* cell = new TiXmlElement("cell");
		cells->LinkEndChild(cell);

		TiXmlElement* segments = new TiXmlElement("segments");
		cell->LinkEndChild(segments);

		TiXmlElement* cables = new TiXmlElement("cables");
		cell->LinkEndChild(cables);

		TiXmlElement* cellBody = NeuronHasSuitableSomaData(nrn) ? new TiXmlElement(useCellBody ? "cellBody" : "cell_body") : 0;
		if(cellBody)
		{
			if(usePre173CellOrder)
				// potential memory leak..
				cellBody = cell->InsertBeforeChild(segments, *cellBody)->ToElement();
			else
				cell->LinkEndChild(cellBody);
		}

		TiXmlElement* spines = NeuronHasSuitableSpineData(nrn) ? new TiXmlElement("spines") : 0;
		if(spines)
		{
			if(usePre173CellOrder)
				// potential memory leak..
				spines = cell->InsertBeforeChild(cables, *spines)->ToElement();
			else
				cell->LinkEndChild(spines);
		}

		// global markers
		const bool hasGlobalMarkers = (nrn.CountGlobalMarkers() > 0);
		if(hasGlobalMarkers)
		{
			TiXmlElement* pre173Position = segments;

			for(Branch3D::Markers::const_iterator it=nrn.m_globalData.m_markers.begin();it!=nrn.m_globalData.m_markers.end();++it)
			{
				TiXmlElement* freePoints = new TiXmlElement(useFreePoints ? "freePoints" : "free_points");

				freePoints->SetAttribute("name", it->first.c_str());

				WritePoints(freePoints, it->second);

				if(usePre173CellOrder)
				{
					// warning - mem leak
					pre173Position = cell->InsertAfterChild(pre173Position, *freePoints)->ToElement();
				}
				else
					cell->LinkEndChild(freePoints);
			}
		}

		// connectivity and sample data, and also spine and marker data at the same time. *** No longer export local marker data ***
		WriteSegmentsCablesSpinesPoints(segments, cables, spines, /*freePoints*/0, nrn, somaGroupName, dendGroupName, axonGroupName);

		//
		if(cellBody)
			WriteCellBody(cellBody, nrn);
	}

	// write xml file
	TiXmlPrinter printer;
	printer.SetIndent("\t");
	printer.SetLineBreak("\n");
	doc.Accept( &printer );

	rOut.write(printer.CStr(), printer.Size());

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateNeuroML(Input& rIn, const ValidationOptions& options)
{
	// lets put the entire file in memory so we can directly Parse() it
	size_t size = rIn.remaining();
	char* buffer = new char[size+1];
	rIn.read(buffer, size);
	buffer[size] = 0;

	// TODO: we would like to validate against xsd, but no support in tinyxml...

	TiXmlDocument document;
	document.Parse(buffer);

	delete [] buffer;

	if(!document.Error())
	{
		const TiXmlNode* morphml = document.FirstChild("morphml");
		if(morphml)
			return kValidationTrue;

		const TiXmlNode* neuroml = document.FirstChild("neuroml");
		if(neuroml)
			return kValidationTrue;
	}

	return kValidationFalse;
}



#if 0

// write axons/dendrites as set of paths.... not really supported by morphml...

static void WritePath(TiXmlElement* parent, const Branch3D& branch, int currentId, int parentId)
{
	TiXmlElement* path = new TiXmlElement("path");

	path->SetAttribute("name", );
	if(parentId != -1)
		path->SetAttribute("parent", dec(parentId) );

	parent->LinkEndChild(path);
}

static int WriteBranchPathSpinesPoints(TiXmlElement* cell, TiXmlElement* spines, TiXmlElement* points, Neuron3D::DendriteTreeConstIterator it, int id, int parentId, MorphMLStructureType structureType)
{
	int currentId = id;

	WritePath(cell, *it, currentId, parentId);
	++id;

	if(spines)
		WriteSpines(spines, (*it).m_spines, -1);

	if(points)
		WritePoints(points, (*it).m_markers);

	if(it.child())
		id = WriteBranchPathSpinesPoints(cell, spines, points, it.child(), id, currentId, structureType);
	if(it.peer())
		id = WriteBranchPathSpinesPoints(cell, spines, points, it.peer(), id, parentId, structureType);

	return id;
}

static void WritePathsSpinesPoints(TiXmlElement* cell, TiXmlElement* spines, TiXmlElement* points, const Neuron3D &nrn)
{
	int id			= 0;
	int parentId	= -1;

	// global data
	if(spines)
		WriteSpines(spines, nrn.m_globalData.m_spines, -1);

	if(points)
		WritePoints(points, nrn.m_globalData.m_markers);

	// dendrites
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		id = WriteBranchPathSpinesPoints(cell, spines, points, (*d).root(), id, parentId, kStructureDend);
	}

	// axons
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		id = WriteBranchPathSpinesPoints(cell, spines, points, (*a).root(), id, parentId, kStructureAxon);
	}
}
#endif

