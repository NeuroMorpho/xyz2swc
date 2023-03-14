//
//
//

#include "ImportExportCommonIncludes.h"

#include <map>

#include "TinyXML/tinyxml.h"

#include "NeurolucidaMarkers.h"
#include "NeurolucidaColours.h"
#include "NeurolucidaTerminals.h"


/**


**/


const char* kTagPoint		= "point";
const char* kTagMarker		= "marker";
const char* kTagSpine		= "spine";
const char* kTagContour 	= "contour";
const char* kTagTree		= "tree";
const char* kTagBranch		= "branch"; // 4.0 ?

const char* kAttrType		= "type";
const char* kAttrColor		= "color";
const char* kAttrName		= "name";
const char* kAttrVaricosity	= "varicosity";
const char* kAttrLeaf		= "leaf";
const char* kAttrStyle		= "style";
const char* kAttrClosed		= "closed";
const char* kAttrShape		= "shape"; // 4.0 ?


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//


enum StructureType 
{
	kStructureGlobal,
	kStructureContour,
	kStructureTree
};

template<typename T>
void ExtractSamplePointAttributes(const TiXmlElement* sampleElem, T& sample)
{
	const char* attrX = sampleElem->Attribute("x");
	const char* attrY = sampleElem->Attribute("y");
	const char* attrZ = sampleElem->Attribute("z");
	const char* attrD = sampleElem->Attribute("d");

	assert(attrX);
	assert(attrY);
	assert(attrZ);
//	assert(attrD);

	sample.x = (float)atof(attrX);
	sample.y = (float)atof(attrY);
	sample.z = (float)atof(attrZ);

	if(attrD)
		sample.d = (float)atof(attrD);
	else 
		sample.d = 0.0f;
}


static void ExtractSamplePoints(MorphologyBuilder& builder, const TiXmlElement* parentNode, StructureType structType)
{
	SamplePoint pt;
	const TiXmlElement* pointElem = parentNode->FirstChildElement(kTagPoint);
	while(pointElem)
	{
		ExtractSamplePointAttributes(pointElem, pt);

		switch(structType)
		{
		case kStructureContour:
			builder.CsAddSample(pt);
			break;
		case kStructureTree:
			builder.CbAddSample(pt);
			break;
		}

		pointElem = pointElem->NextSiblingElement(kTagPoint);
	}
}

static void ExtractMarkerPoints(MorphologyBuilder& builder, const TiXmlElement* parentNode, StructureType structType)
{
	MarkerPoint pt;
	const TiXmlElement* markerElem = parentNode->FirstChildElement(kTagMarker);
	while(markerElem)
	{
		const char* attrType		= markerElem->Attribute(kAttrType);
		const char* attrColor		= markerElem->Attribute(kAttrColor);
		const char* attrName		= markerElem->Attribute(kAttrName);
		const char* attrVaricosity	= markerElem->Attribute(kAttrVaricosity);

		const TiXmlElement* pointElem = markerElem->FirstChildElement(kTagPoint);
		ExtractSamplePointAttributes(pointElem, pt);

		pt.shape = NeurolucidaShapeName2MarkerShape(attrType);

		switch(structType)
		{
		case kStructureGlobal:
			builder.AddMarker(attrName, pt);
			break;
		case kStructureContour:
			builder.CsAddMarker(attrName, pt);
			break;
		case kStructureTree:
			builder.CbAddMarker(attrName, pt);
			break;
		}

		markerElem = markerElem->NextSiblingElement(kTagMarker);
	}
}

static void ExtractSpinePoints(MorphologyBuilder& builder, const TiXmlElement* parentNode, StructureType structType)
{
	SpinePoint pt;
	const TiXmlElement* spineElem = parentNode->FirstChildElement(kTagSpine);
	while(spineElem)
	{
		const TiXmlNode* pointNode = spineElem->FirstChildElement(kTagPoint);
		ExtractSamplePointAttributes(pointNode->ToElement(), pt);

		pt.type = kSpineNoDetail;

		//pt.associatedSegment = 

		switch(structType)
		{
		case kStructureTree:
			builder.CbAddSpine(pt);
			break;
		default:
			break;
		}

		spineElem = spineElem->NextSiblingElement(kTagSpine);
	}
}

static void ExtractSampleAndSpinePoints(MorphologyBuilder& builder, const TiXmlElement* parentNode, StructureType structType)
{
	SamplePoint pt;
	const TiXmlElement* elemGen = parentNode->FirstChildElement();
	const TiXmlElement* elemPoint = parentNode->FirstChildElement(kTagPoint);
	const TiXmlElement* elemSpine = parentNode->FirstChildElement(kTagSpine);

	while(elemPoint || elemSpine)
	{
		assert(elemGen);

		if(elemGen == elemPoint)
		{
			ExtractSamplePointAttributes(elemPoint, pt);

			switch(structType)
			{
			case kStructureContour:
				builder.CsAddSample(pt);
				break;
			case kStructureTree:
				builder.CbAddSample(pt);
				break;
			}

			elemPoint = elemPoint->NextSiblingElement(kTagPoint);
		}
		else if(elemGen == elemSpine)
		{
			const TiXmlElement* pointElem = elemSpine->FirstChildElement(kTagPoint);

			SpinePoint pt;
			ExtractSamplePointAttributes(pointElem, pt);
			pt.type = kSpineNoDetail;
			pt.associatedSegment = builder.CurrentBranch().NumSamples() - 1;

			switch(structType)
			{
			case kStructureTree:
				builder.CbAddSpine(pt);
				break;
			default:
				break;
			}

			elemSpine = elemSpine->NextSiblingElement(kTagSpine);
		}

		elemGen = elemGen->NextSiblingElement();

	}
}


static void ExtractNeurolucidaXMLBranch(MorphologyBuilder& builder, const TiXmlElement* treeElem, bool isAxon, bool root, int version)
{
	const char* attrLeaf		= treeElem->Attribute(kAttrLeaf);
	const char* attrColor		= treeElem->Attribute(kAttrColor);
	const char* attrType		= treeElem->Attribute(kAttrType);
	const char* attrStyle		= treeElem->Attribute(kAttrStyle);

	const bool isLeaf = (attrLeaf != 0);

	//
	if(root)
	{
		const bool isAxon = ( String(attrType) == String("Axon") );

		if(isAxon)
			builder.NewAxon();
		else
		{
			builder.NewDendrite();

			const bool isApical = ( String(attrType) == String("Apical") );

			if(isApical)
				builder.CdSetApical();
		}
	}
	else
		builder.CbNewChildLast(true);

	if(isLeaf)
	{
		NeurolucidaTerminalType ttype = NeurolucidaTerminalName2TerminalType(attrLeaf);
		builder.CbSetTerminalType( (TerminalType) ttype);
	}

	// add samples, markers and spines

	// samples, then spines - doesn't seem to be an associated sample attribute...
	// spines are probably interleaved with points, to indicate position..
	//ExtractSamplePoints(builder, treeElem, kStructureTree);
	//ExtractSpinePoints(builder, treeElem, kStructureTree);

	// extract samples and spines at the same time
	ExtractSampleAndSpinePoints(builder, treeElem, kStructureTree);

	ExtractMarkerPoints(builder, treeElem, kStructureTree);


	// do children
	const TiXmlElement* subTreeElem = treeElem->FirstChildElement(version == 4 ? kTagBranch : kTagTree);

	while(subTreeElem)
	{
		ExtractNeurolucidaXMLBranch(builder, subTreeElem, isAxon, false, version);

		subTreeElem = subTreeElem->NextSiblingElement(version == 4 ? kTagBranch : kTagTree);
	}


	if(!root)
		builder.CbToParent();
}

static bool g_bAllContoursAsSoma = false;
static bool g_useSomaHint = false;
static String g_SomaHint;
static bool IsCellBodyName(String name)
{
	return	g_bAllContoursAsSoma || 
			(name == "Cell Body" || name == "CellBody" || StringContains(name, "Soma")) ||
			(g_useSomaHint && StringContains(name, g_SomaHint));
}

static bool ExtractNeurolucidaXMLCell(MorphologyBuilder& builder, const TiXmlElement* mbfElement, Neuron3D& nrn, int version)
{
	// extract global markers
	ExtractMarkerPoints(builder, mbfElement, kStructureGlobal);

	// extract contours
	const TiXmlElement* contourElem = mbfElement->FirstChildElement(kTagContour);
	while(contourElem)
	{
		const char* attrName		= contourElem->Attribute(kAttrName);
		const char* attrColor		= contourElem->Attribute(kAttrColor);
		const char* attrClosed		= contourElem->Attribute(kAttrClosed);
		const char* attrStyle		= contourElem->Attribute(kAttrStyle);

		const String& attrNameStr(attrName);

		// 
		if( IsCellBodyName(attrNameStr) )
		{
			const bool closed = attrClosed && attrClosed == "true";

			if(closed)
				builder.NewSomaClosedContour();
			else
				builder.NewSomaOpenContour();

			ExtractSamplePoints(builder, contourElem, kStructureContour);
			ExtractMarkerPoints(builder, contourElem, kStructureContour);
		}

		contourElem = contourElem->NextSiblingElement(kTagContour);
	}


	// extract trees
	const TiXmlElement* treeElem = mbfElement->FirstChildElement(kTagTree);
	while(treeElem)
	{
		ExtractNeurolucidaXMLBranch(builder, treeElem, true, true, version);

		treeElem = treeElem->NextSiblingElement(kTagTree);
	}

	if(builder.HasFailed())
	{
		LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
		return false;
	}

	builder.ApplyChanges();
	nrn.MergeAllUnaryBranchPoints();


	return true;
}

bool ImportNeurolucidaXML(Input& rIn, Neuron3D &nrn, const HintOptions& options)	
{
	// configure
	g_useSomaHint = options.hasHintSoma;
	if(g_useSomaHint)
		g_SomaHint = options.hintSomaSubString;

	g_bAllContoursAsSoma = options.allContoursAsSoma;
	//

	// lets put the entire file in memory so we can directly Parse() it
	size_t size = rIn.remaining();
	char* buffer = new char[size+1];
	rIn.read(buffer, size);
	buffer[size] = 0;

	TiXmlDocument document;
	document.Parse(buffer);

	delete [] buffer;

	const TiXmlNode* mbf = document.FirstChild("mbf");

	if(mbf)
	{
		int version = ( strcmp(mbf->ToElement()->Attribute("version"), "4.0") == 0 ? 4 : 3);
		MorphologyBuilder builder(nrn);
		return ExtractNeurolucidaXMLCell(builder, mbf->ToElement(), nrn, version);
	}

	return false;
}


bool ImportRawNeurolucidaXML(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportNeurolucidaXML(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

String rgbAxon		= "#000000";
String rgbDend		= "#000000";
String rgbSoma		= "#000000";
String rgbMarker	= "#000000";

static void SetColour(const String& colIn, String& colOut)
{
	u8 r,g,b;

	NeurolucidaColour nlCol = NeurolucidaColourString2Id(colIn);
	if(nlCol != kColour_Unknown)
		GetNeurolucidaColourRGB(nlCol, r, g, b);
	else
	{
		// RGB components ?
	}

	colOut = String("#" + hex(r,2) + hex(g,2) + hex(b,2) );
}

static void SetColourAxon(const String& col)	{ SetColour(col, rgbAxon); }
static void SetColourDend(const String& col)	{ SetColour(col, rgbDend); }
static void SetColourSoma(const String& col)	{ SetColour(col, rgbSoma); }
static void SetColourMarker(const String& col)	{ SetColour(col, rgbMarker); }

static String GetColourAxon() { return rgbAxon; }
static String GetColourDend() { return rgbDend; }
static String GetColourSoma() { return rgbSoma; }
static String GetColourMarker() { return rgbMarker; }


template<typename T>
static void WriteNeurolucidaXMLPoint(TiXmlElement* parent, const T& sample)
{
	TiXmlElement* p = new TiXmlElement("point");
	p->SetAttribute("x", dec(sample.x, 2).c_str() );
	p->SetAttribute("y", dec(sample.y, 2).c_str());
	p->SetAttribute("z", dec(sample.z, 2).c_str());
	p->SetAttribute("d", dec(sample.d, 2).c_str());
	//p->SetAttribute("sid", dec(sample.sectionId, 2) );
	parent->LinkEndChild(p);
}

static void WriteNeurolucidaXMLMarkers(TiXmlElement* parent, const Branch3D::Markers& markers)
{
	for(Branch3D::Markers::const_iterator mit = markers.begin();mit != markers.end();++mit)
	{
		String markerName = mit->first;
		String markerShape = NeurolucidaMarkerShape2ShapeName(mit->second[0].shape);

		for(Branch3D::MarkerSet::const_iterator it = mit->second.begin();it!=mit->second.end();++it)
		{
			TiXmlElement* m = new TiXmlElement(kTagMarker);
			m->SetAttribute(kAttrType, markerShape.c_str());
			m->SetAttribute(kAttrColor, GetColourMarker().c_str());
			m->SetAttribute(kAttrName, markerName.c_str());
			m->SetAttribute(kAttrVaricosity, "false");
			WriteNeurolucidaXMLPoint(m, *it);
			parent->LinkEndChild(m);
		}
	}
}

static void WriteNeurolucidaXMLSpine(TiXmlElement* parent, const SpinePoint& sample)
{
	TiXmlElement* s = new TiXmlElement(kTagSpine);
	WriteNeurolucidaXMLPoint(s, sample);
	parent->LinkEndChild(s);
}

static void WriteNeurolucidaXMLBranch(TiXmlElement* parent, Neuron3D::DendriteTreeConstIterator it, bool isAxon, bool isApical)
{
	const bool isRoot = !it.parent();
	const bool isTerminal = !it.child();
	TiXmlElement* tree = new TiXmlElement(kTagTree);

	// same order as Neurolucida
	if(isRoot)
	{
		tree->SetAttribute(kAttrColor, isAxon ? GetColourAxon().c_str() : GetColourDend().c_str());
		tree->SetAttribute(kAttrStyle, "solid");
		tree->SetAttribute(kAttrType, isAxon ? "Axon" : (isApical ? "Apical" : "Dendrite") );
	}

	if(isTerminal)
	{
		NeurolucidaTerminalType ttype = (NeurolucidaTerminalType) (*it).GetTerminalType();
		String terminalTypeName = GetNeurolucidaTerminalTypeName(ttype);

		// TODO: Possible that not all terminal types are valid for XML: "Generated", "Incomplete" may not work !??
		tree->SetAttribute(kAttrLeaf, terminalTypeName.c_str());
		//
	}

	parent->LinkEndChild(tree);

	// points
	const u32 startPoint = it.parent() ? 1 : 0;
	for(u32 n=startPoint;n<(*it).m_samples.size();++n)
	{
		WriteNeurolucidaXMLPoint(tree, (*it).m_samples[n]);

		for(u32 s=0;s<(*it).m_spines.size();++s)
		{
			if(n == (*it).m_spines[s].associatedSegment)
				WriteNeurolucidaXMLSpine(tree, (*it).m_spines[s]);
		}
	}

	// markers
	WriteNeurolucidaXMLMarkers(tree, (*it).m_markers);

	// spines (now written interleaved with points..)
	//for(u32 n=0;n<(*it).m_spines.size();++n)
	//	WriteNeurolucidaXMLSpine(tree, (*it).m_spines[n]);

	//
	if(it.child())
		WriteNeurolucidaXMLBranch(tree, it.child(), isAxon, isApical);
	if(it.peer())
		WriteNeurolucidaXMLBranch(parent, it.peer(), isAxon, isApical);
}

static void WriteNeurolucidaXML(TiXmlElement* mbf, const Neuron3D &nrn)
{
	// global markers
	WriteNeurolucidaXMLMarkers(mbf, nrn.m_globalData.m_markers);

	// somas
	for(Neuron3D::SomaConstIterator s = nrn.SomaBegin(); s != nrn.SomaEnd(); ++s)
	{
		const Soma3D& soma = (*s);

		// contour
		TiXmlElement* contour = new TiXmlElement("contour");
		contour->SetAttribute(kAttrName, "Soma");
		contour->SetAttribute(kAttrColor, GetColourSoma().c_str());
		contour->SetAttribute(kAttrClosed, (soma.somaType == Soma3D::kContourClosed) ? "true" : "false");
		contour->SetAttribute(kAttrStyle, "solid");
		mbf->LinkEndChild(contour);

		// points
		for(u32 n=0;n<soma.m_samples.size();++n)
			WriteNeurolucidaXMLPoint(contour, soma.m_samples[n]);

		// markers
		WriteNeurolucidaXMLMarkers(contour, soma.m_markers);
	}

	// dendrites
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		const bool isApical = ((*d).GetTreeType() == Neuron3D::DendriteTree::Apical);
		WriteNeurolucidaXMLBranch(mbf, (*d).root(), false, isApical);
	}

	// axons
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		WriteNeurolucidaXMLBranch(mbf, (*a).root(), true, false);
	}
}

bool ExportNeurolucidaXML(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	//
	SetColourAxon(options.axonColour);
	SetColourDend(options.dendColour);
	SetColourSoma(options.somaColour);
	SetColourMarker(options.markerColour);

	// xml 
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "ISO-8859-1", "");
	doc.LinkEndChild( decl );

	// mbf
	TiXmlElement* mbf = new TiXmlElement("mbf");
	mbf->SetAttribute("version", options.formatStyle == kStyleNLXML_4 ? "4.0" : "3.0"/*options.versionId*/);
	mbf->SetAttribute("xmlns", "http://www.mbfbioscience.com/2007/neurolucida");
	mbf->SetAttribute("xmlns:nl", "http://www.mbfbioscience.com/2007/neurolucida");
	mbf->SetAttribute("appname", options.appName.c_str());
	mbf->SetAttribute("appversion", options.appVersion.c_str());
	doc.LinkEndChild(mbf);

	// connectivity and sample data
	WriteNeurolucidaXML(mbf, nrn);

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

ValidationResult ValidateNeurolucidaXML(Input& rIn, const ValidationOptions& options)
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
		const TiXmlElement* root = document.FirstChildElement("mbf");

		if(root)
			return kValidationTrue;
	}

	return kValidationFalse;
}




#if 0

Overview

This document describes the XML output format for Neurolucida tracing models. These models have been saved as .ASC (ASCII) or .DAT (binary data) files in previous versions. The XML export feature was introduced in version 7.51 of Neurolucida and also in the same version of Stereo Investigator. An XSD (Xml Schema Document) is in the works for those that want to perform validation of the XML. Most people will find this document much easier to understand than the XSD.
The format is easier to understand with an example. Please refer to the sample XML that accompanies this document. 
The MBF Data Set: <mbf> tag:

The entire xml document is contained in the <mbf> tag. Unless you are interested in using the XSD file for validation, you probably aren’t interested in this tag.

version attribute: refers to the version of the XML file (3.0) which matches the version of the conventional .ASC and .DAT files used to store tracing and 3D reconstruction data.

xmlns attribute: this is the name space for entities used in this file.

xmlns:nl attribute: the nl namespace (nl is an abbreviation of Neurolucida).

appname: The name of the application that generated this XML file. In this case, it’s Neurolucida. But other MBF products generate XML data files in the same format too.

appversion: The version number of Neurolucida used to generate this XML file. Please note that while Neurolucida has generated XML output since version 6.5, we have only formally begun supporting the XML format as of version 7.51.
Images: <image> tag

		Neurolucida allows you to load multiple images, and trace from these images. The images can be either 2D (a single image plane) or 3D (multiple image planes from a single file or from multiple files). In this example, we have a single 2D image.

		<channels> entity: This is only relevant if you are loading channel data from separate images and merging them into a single view. For example, three separate images, each one for  a different channel.

		<scale> entity: The X and Y scaling in microns per pixel.

		<zspacing> entity: This is the spacing between z planes in microns.

Points: <point> tag
		Points are used inside several other XML tags to denote lines and contours. A point is composed of an XYZ coordinate and an optional diameter.

		X attribute: coordinate in real world units (microns)

		Y attribute: coordinate in real world units (microns)

		Z attribute: coordinate in real world units (microns)

		D attribute: the diameter at this point, in real world units (optional)

Contours : <contour> tag
		   A contour is a named list of <points>. If the contour is closed, then the last point is connected to the first point.

		   name attribute: The name of the contour. Please note that a collection of contours, all with the same name a considered a contour set. In 3D, a set of same-named contours can be displayed in the solids module with a surrounding skin,

		   closed attribute: False if this is an open contour, otherwise True.

		   style attribute: Currently, this is always set to solid in Neurolucida.

		   <resolution> entity: This is the size of the pixel, in microns, used when tracing the contour.  <point> entities: One point for each point in the polygon that makes up the contour.

Markers: <marker> tag

		 A marker is a single point in the tracing model marked with a symbol. There are a total of 50 marker symbols available in Neurolucida, and each maker can be placed an unlimited number of times. Markers can also be associated with other objects such as contours and trees. In our example, marker with type circle1 is contained inside the contour named MyContour. This means that the marker is associated with this same contour.

		 type attribute: A name that describes the symbol used for the maker (star, filled circle, etc.)

		 color attribute: A hexadecimal RGB value for the color of the contour. In our example, #008000 means Red = 0x00, Blue = 0x80 and Green = 0x00. This is the same convention used in HTML for web colors.

		 name attribute: The name used for the marker. This defaults to the maker number (e.g. “Marker 19”), but the user can give markers custom names in Neurolucida.

		 varicosity attribute: Set to true if the marker is being used to mark a varicosity.

 Branching Structures: <tree> tag

		 Trees represent branching structures (axons, dendritic trees) in Neurolucida. They are normally nested, with sub-trees branching off of the main branch. A tree begins with a root node. A node is the place where a tree splits into multiple branches or sub-trees.  A branch is written as collection of points followed by the ending for the branch.

		 Nodes are not explicitly listed in the XML structure, but rather are implied when a <tree> contains another <tree>. A node can be a root node, a bifurcation or a terminal node.

		 Leaf attribute:  When present, this attribute indicates that this sub-tree terminates with an ending rather than a node. The string value of the attribute indicates the type of ending, enumerated as High, Low, Origin, Normal, or Middle

		 Color attribute: A hexadecimal RGB value for the color of the contour. In our example, #008000 means Red = 0x00, Blue = 0x80 and Green = 0x00. This is the same convention used in HTML for web colors.

		 Type attribute: The type of tree, enumerated as: Axon, Dendrite or Apical


		 Style attribute: The line type used to display the tree. The value is normally set to Solid.

Spines: <spine> tag

		Spines are marked as point objects that are attached to a branch of a <tree>. The spine contains a single <point> object, indicating the center position of the spine head and the diameter of the head.

Sets of Objects

		Objects are implicitly placed into a set when the objects in a named set all contain the same contain property entities of name set In the example below the contours are both part of the set named Cell1.

		<contour name="Left Hemisphere" color="#008000" closed="true" style="solid">

		<property name="Set">Cell1</property>

		<point x="73.50" y="-48.00" z="0.00" d="0.25"></point>

		<point x="71.50" y="-46.75" z="0.00" d="0.25"></point>

		<point x="66.75" y="-45.25" z="0.00" d="0.25"></point>

		. . . points omitted for brevity . . .

		<point x="82.00" y="-59.25" z="0.00" d="0.25"></point>

		<point x="78.75" y="-53.50" z="0.00" d="0.25"></point>

		<point x="76.00" y="-50.25" z="0.00" d="0.25"></point>

		</contour>

		<contour name="Left Hemisphere" color="#008000" closed="true" style="solid">

		<property name="Set">Cell1</property>

		<point x="63.00" y="-54.75" z="6.00" d="0.25"></point>

		<point x="49.75" y="-54.25" z="6.00" d="0.25"></point>

		<point x="45.25" y="-55.50" z="6.00" d="0.25"></point>

		. . . points omitted for brevity . . .

		<point x="64.00" y="-61.25" z="6.00" d="0.25"></point>

		<point x="63.50" y="-59.50" z="6.00" d="0.25"></point>

		<point x="63.50" y="-59.00" z="6.00" d="0.25"></point>

		</contour>
#endif

