//
//
//

#include "ImportExportCommonIncludes.h"

#include "TinyXML/tinyxml.h"

#include "Core/OutputMem.h"
#include "Core/InputMem.h"


extern bool ImportSWC(Input& rIn, Neuron3D &nrn, const HintOptions& options);

/**

	PSICS format

	Structure supported ->
		Soma		: YES
		Dendrite	: YES
		Axon		: YES
		Marker		: NO
		Spine		: NO

	Pending:

	@note  we convert into SWC format, then use the SWC importer. Hopefully this will serve ok, as the formats are very similar..
	@note  Haven't seen many examples, probably some cases unaccounted for.

**/

static const char* kTagPoint = "Point";

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//


static u32 ExtractPSICSCellAsSWC(const TiXmlElement* cell, Neuron3D &nrn, Output* swcOut, const String& nameSoma, const String& nameAxon, const String& nameDend)
{
	const TiXmlElement* pointElem = cell->FirstChildElement(kTagPoint);

	u32 idCount = 1;

	struct PointInfo 
	{
		int index;
		int structure;
	};
	std::map<String, PointInfo > idMap;
	while(pointElem)
	{
		// extract point data
		const char* attrID			= pointElem->Attribute("id");
		const char* attrX			= pointElem->Attribute("x");
		const char* attrY			= pointElem->Attribute("y");
		const char* attrZ			= pointElem->Attribute("z");
		const char* attrR			= pointElem->Attribute("r");
		const char* attrBeyond		= pointElem->Attribute("beyond");
		const char* attrParent		= pointElem->Attribute("parent");
		const char* attrLabel		= pointElem->Attribute("label");
		const char* attrPartOf		= pointElem->Attribute("partof");
		const char* attrMinor		= pointElem->Attribute("minor");
		const char* attrOnSurface	= pointElem->Attribute("onSurface");

		// required
		assert(attrID);
		//assert(attrR);
		assert( (attrX && attrY && attrZ) || attrBeyond );
		assert( attrParent || !attrBeyond );
		//


		SamplePoint sample;
		sample.x = (float)atof(attrX);
		sample.y = (float)atof(attrY);
		sample.z = (float)atof(attrZ);
		sample.d = 2.0f * (float)atof(attrR);

		// use label to determine structure type (if parent is root or soma..)
		u32 structureId = 3;

		if(!attrParent)
		{
			structureId = 1;
		}

		if(attrLabel)
		{
			String labelStr(attrLabel);
			if(StringContains(labelStr, nameSoma))
				structureId = 1;
			else if(StringContains(labelStr, nameAxon))
				structureId = 2;
			else if(StringContains(labelStr, nameDend))
				structureId = 3;
		}

		// parent 
		int parentId = -1;
		if(attrParent)
		{
			const String parentStr(attrParent);
			std::map<String, PointInfo>::const_iterator it = idMap.find(parentStr);
			if(it == idMap.end())
			{
				LogImportFailure("Parent Id does not exist!");
				return false;
			}

			parentId = it->second.index;
		}

		String swcLine =	dec(idCount)		+ " " + 
							dec(structureId)	+ " " + 
							dec(sample.x,2)		+ " " + 
							dec(sample.y,2)		+ " " + 
							dec(sample.z,2)		+ " " + 
							dec(sample.d,2)		+ " " + 
							dec(parentId);

		String idStr(attrID);
		assert(idMap.find(idStr) == idMap.end());

		PointInfo info = {idCount, structureId };
		idMap[idStr] = info;

		swcOut->writeLine(swcLine);

		++idCount;
		pointElem = pointElem->NextSiblingElement(kTagPoint);
	}

	return swcOut->GetBytesWritten();
}

bool ImportPSICS(Input& rIn, Neuron3D &nrn, const HintOptions& options)	
{
	// lets put the entire file in memory so we can directly Parse() it
	size_t size = rIn.remaining();
	char* buffer = new char[size+1];
	rIn.read(buffer, size);
	buffer[size] = 0;

	TiXmlDocument document;
	document.Parse(buffer);

	delete [] buffer;


	const TiXmlElement* cellmorph = document.FirstChildElement("CellMorphology");

	if(cellmorph)
	{
		const String nameSoma = options.hasHintSoma ? options.hintSomaSubString : "soma";
		const String nameAxon = options.hasHintAxon ? options.hintAxonSubString : "axon";
		const String nameDend = options.hasHintDend ? options.hintDendSubString : "dend";


		void* swcMem = malloc(size);

		OutputMem	swcOut(swcMem, size);
		u32 swcSize = ExtractPSICSCellAsSWC(cellmorph, nrn, &swcOut, nameSoma, nameAxon, nameDend);

		assert(swcSize <= size);

		InputMem	swcIn(swcMem,swcSize);
		HintOptions options;
		options.hintStyle = kStyleSwc_Std;
		bool result = ImportSWC(swcIn, nrn, options);

		free(swcMem);

		return result;
	}

	return false;
}


bool ImportRawPSICS(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportPSICS(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

static void WritePSICSPoint(TiXmlElement* xmlparent, const String& id, const String& parentId, const String& label, const SamplePoint& sample)
{
	TiXmlElement* p = new TiXmlElement(kTagPoint);
	p->SetAttribute("id", id.c_str());
	p->SetAttribute("x", dec(sample.x, 2).c_str());
	p->SetAttribute("y", dec(sample.y, 2).c_str());
	p->SetAttribute("z", dec(sample.z, 2).c_str());
	p->SetAttribute("r", dec(0.5f*sample.d, 2).c_str() );
	if(parentId != "")
		p->SetAttribute("parent", parentId.c_str());
	p->SetAttribute("label", label.c_str());
	xmlparent->LinkEndChild(p);
}

struct PointData
{
	u32 id;
	int parentId;
	u32 distanceToRoot;
	String label;
	SamplePoint sample;

	bool operator<(const PointData& rhs)
	{
		/*switch(style)
		{
			case 
		}*/
		return true;
	}

};

typedef std::vector<PointData> PointList;

struct PointSorter
{
	FormatStyle style;

	PointSorter(FormatStyle s) : style(s) {} 

};

static void AddSomaPointsToList(Neuron3D::SomaConstIterator it, PointList& pointList, int parentId, u32 distance, const String& name, u32 index, u32 segId)
{
	// points
	for(u32 n=0;n<(*it).m_samples.size();++n)
	{
		PointData point;
		point.id = pointList.size() + 1;
		point.parentId = (n == 0 ? parentId : pointList.size());
		point.distanceToRoot = distance + n;
		point.label = "Seg" + dec(segId) + name + dec(index);
		point.sample = (*it).m_samples[n];

		pointList.push_back(point);
	}
}

static u32 AddTreePointsToList(Neuron3D::DendriteTreeConstIterator it, PointList& pointList, u32 parentId, u32 distance, const String& name, u32 index, u32 segId)
{
	// points

	u32 start = it.parent() ? 1 : 0;

	const u32 numSamples = (*it).m_samples.size();

	for(u32 n=start;n<numSamples;++n)
	{
		PointData point;
		point.id = pointList.size() + 1;
		point.parentId = (n==start ? parentId : pointList.size());
		point.distanceToRoot = distance + n;
		point.label = "Seg" + dec(segId) + name + dec(index);
		point.sample = (*it).m_samples[n];

		pointList.push_back(point);
	}

	++segId;

	//
	if(it.child())
		segId = AddTreePointsToList(it.child(), pointList, pointList.size(), distance + (numSamples-start), name, index, segId);
	if(it.peer())
		segId = AddTreePointsToList(it.peer(), pointList, parentId, distance, name, index, segId);

	return segId;
}

static void WritePSICS(TiXmlElement* cell, const Neuron3D &nrn, const StyleOptions& options)
{
	/**
		2 stages

		1 - Add all points to a big list (containing id, parent id, and other info required for sort..)  [Note - ids are enumerated in a standard child/peer order]
		2 - Sort the entire list to generate the output order. (Standard order is that seen in some PSICS example files. There are several potential variations however)

	*/

	// configure
	const FormatStyle style = options.formatStyle;

	const String nameSoma = options.customiseSomaName ? options.somaName : "soma";
	const String nameAxon = options.customiseAxonName ? options.axonName : "axon";
	const String nameDend = options.customiseDendName ? options.dendName : "dend";

	if(options.identifyNL || options.addCommentText)
	{
		String text;

		if(options.identifyNL)
			text = String("PSICS morphology file. generated by " + options.appName + " (version " + options.appVersion + "). ");
		if(options.addCommentText)
			text += "\n" + options.commentText;

		TiXmlElement* about = new TiXmlElement("CellMorphology");
		cell->LinkEndChild(about);
	}

	//

	u32 pointcount = 0;

	PointList pointList;

	// soma
	u32 somaIndex = 1;
	if(nrn.CountSomas() > 0)
		AddSomaPointsToList(nrn.SomaBegin(), pointList, -1, 0, nameSoma, somaIndex, 1);

	u32 treeRoot = pointList.size();

	// dendrites
	u32 dendIndex = 1;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d, ++dendIndex)
		AddTreePointsToList((*d).root(), pointList, (treeRoot == 0 ? -1 : treeRoot), treeRoot, nameDend, dendIndex, 1);

	// axons
	u32 axonIndex = 1;
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a, ++axonIndex)
		AddTreePointsToList((*a).root(), pointList, (treeRoot == 0 ? -1 : treeRoot), treeRoot, nameAxon, axonIndex, 1);


	//PointSorter sorter(style);
	//std::sort(pointList.begin(),pointList.end(), sorter);

	for(u32 n=0;n<pointList.size();++n)
		WritePSICSPoint(cell, dec(pointList[n].id), pointList[n].parentId == -1 ? String("") : dec(pointList[n].parentId), pointList[n].label, pointList[n].sample);
}

bool ExportPSICS(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	// xml 
	TiXmlDocument doc;
	//TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "ISO-8859-1", "");
	//doc.LinkEndChild( decl );

	// cell
	TiXmlElement* cell = new TiXmlElement("CellMorphology");
	cell->SetAttribute("id", options.fileName.c_str());
	doc.LinkEndChild(cell);

	// connectivity and sample data
	WritePSICS(cell, nrn, options);

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

ValidationResult ValidatePSICS(Input& rIn, const ValidationOptions& options)
{
	// lets put the entire file in memory so we can directly Parse() it
	size_t size = rIn.remaining();
	char* buffer = new char[size+1];
	rIn.read(buffer, size);
	buffer[size] = 0;

	TiXmlDocument document;
	document.Parse(buffer);

	delete [] buffer;

	if(!document.Error())
	{
		const TiXmlNode* cellNode = document.FirstChild("CellMorphology");

		if(cellNode)
			return kValidationTrue;
	}

	return kValidationFalse;
}



