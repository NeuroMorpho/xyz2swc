//
// FileFormatNeurolucidaDAT.cpp
//

#include "ImportExportCommonIncludes.h"

#include <float.h>
#include <math.h>
#include <algorithm>

#include "NeurolucidaMarkers.h"
#include "NeurolucidaColours.h"
#include "NeurolucidaTerminals.h"



/** 

	NeurolucidaDAT file format. 
	
	Initially reverse engineered from three DAT/ASC file pairs provided by Joseph Pierce.
	Further improvement after analyzing the Yuste archive.
	Plenty of DAT data now available in NeuroMorpho.org, so this format is very well tested now.

	Notes:

	- Format is little-endian.
	On big-endian systems, need to swap multibyte values when reading and writing data.

	- Several element types have been observed in ASC files, but not yet in DAT files,
	so their values are unknown. This isn't a problem, since these specific elements are
	not of interest at the moment anyway, and can be skipped. However, it would be nice
	to be complete at some point.

**/

const size_t headerSize = 70;

const u8 datHeader[headerSize] = 
{ 
	0x04,  'V',  '3',  ' ',  'D',  'A',  'T',  ' ',  'f',  'i',  'l',  'e', 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x2B, 0xAD
};

const u32 datFooter					= 0xAABBCCDD;
const u16 datFooter1				= 0xAABB;
const u16 datFooter2				= 0xCCDD;


// element tags
const u16 kElementString			= 0x0001;	// id (u16), size (u32), chars

const u16 kElementSample			= 0x0101;	// id (u16), size (u32), x, y, z, d (floats), section (u16)
const u16 kElementUNKNOWN			= 0x0102;	//#id (u16), size (u32), ?????
const u16 kElementSampleList		= 0x0103;	// id (u16), size (u32), num samples (u16)
const u16 kElementProperty			= 0x0104;	// id (u16), size (u32), string, property-dependent
const u16 kElementPropertyGroup		= 0x0105;	// id (u16), size (u32), num properties (u16)

const u16 kElementContour			= 0x0201;	// id (u16), size (u32), ....
const u16 kElementTree				= 0x0202;	// id (u16), size (u32), ....
const u16 kElementSubTree			= 0x0203;	// id (u16), size (u32), terminal type (u16), num children (u16)
const u16 kElementMarkerSet			= 0x0204;	//*id (u16), size (u32), shape (string), colour (4 x u8)
const u16 kElementMarkerSection		= 0x0205;	// id (u16), size (u32), num marker sets (u16)
const u16 kElementSpine				= 0x0206;	// id (u16), size (u32), colour (4 x u8)
const u16 kElementSpineSection		= 0x0207;	// id (u16), size (u32), num spines (u16)
const u16 kElementTextTag			= 0x0208;	// id (u16), size (u32), 
const u16 kElementSubTree2			= 0x0209;	// id (u16), size (u32), terminal type (u16), num children (u16)

const u16 kElementThumbnail			= 0x0401;	// 
const u16 kElementDescription		= 0x0402;	// 
const u16 kElementImageData			= 0x0403;	// id (u16), size (u32), ....

// # - haven't seen this tag in any real files, no idea what it might be (if anything)!
// * - note marker set sizes are incorrect, and this affects the size of any section into which they are embedded (contours or trees)


// enumerate object types
enum CurveType
{
	kCurveOpen		= 0x0000,
	kCurveClosed	= 0x0001
};

enum TreeType 
{
	kTreeAxon		= 0x0000,
	kTreeDendrite	= 0x0001,
	kTreeApical		= 0x0002,
	kTreeBasal		= 0x0003
};

enum TerminalTypeVal
{
	kTerminalNormal			= 0x0000,
	kTerminalHigh			= 0x0001,
	kTerminalLow			= 0x0002,
	kTerminalMidpoint		= 0x0003, // guess - could be 0x0005
	kTerminalIncomplete		= 0x0004,
	kTerminalOrigin			= 0x0005, // guess - could be 0x0003
	kTerminalGenerated		= 0x0006,
	kTerminalBranchPoint	= 0x0007
};

// useful sizes
const u32	kSizeElementHeader	= 6; // u16 + u32
const u32	kSizeCurveType		= 2; // u16
const u32	kSizeTreeType		= 2; // u16
const u32	kSizeTerminalType	= 2; // u16
const u32	kSizeColour			= 4; // 4 x u8
const u32	kSizeSample			= 0x18; // header, 4 x float, u16

static const HString kTagChildrenLeft("childrenleft");

bool IsBigEndianRT()
{
	unsigned int value(1);
	const char* byte = (const char*) (&value);
	return byte[0] == 0;
}


// endian swap helper
template<typename T>
void eswap(T& t)
{
	if(IsBigEndianRT())
	{
		char* ptr = (char*) &t;
		u32 size = sizeof(T) - 1;

		for(u32 n=0;n<size/2;++n)
			std::swap(ptr[n], ptr[size - n]);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import 
//

bool ReadHeader(Input& rIn)
{
	char header[headerSize];
	rIn.read(header, headerSize);

	return (strncmp(header, (const char*)datHeader, headerSize) == 0);
}

void SkipData(Input& rIn, u32 size)
{
	rIn.seek(size);
}

template<typename T>
T ReadValue(Input& rIn)
{
	T value;

	rIn.read(value);

	eswap(value);

	return value;
}

String ReadString(Input& rIn, u32 size)
{
	String ret;

	while(size)
	{
		ret += rIn.read();
		--size;
	}

	return ret;
}

bool ReadElementHeader(Input& rIn, u16& id, u32& size)
{
	const u16 theId = ReadValue<u16>(rIn);
	id = theId;

	if( (!IsBigEndianRT()) && theId == datFooter2)
	{
		return !rIn.didFail();
	}
	else if(IsBigEndianRT() && theId == datFooter1) 
	{
		return !rIn.didFail();
	}

	const u32 theSize = ReadValue<u32>(rIn);

	size = theSize;

	return !rIn.didFail();
}

bool ReadSamplePoint(Input& rIn, SamplePoint& sample)
{
	u16 section;

	sample.x = ReadValue<float>(rIn);
	sample.y = ReadValue<float>(rIn);
	sample.z = ReadValue<float>(rIn);
	sample.d = ReadValue<float>(rIn);

	section = ReadValue<u16>(rIn);

	return true;
}

bool ReadMarkerPoint(Input& rIn, MarkerPoint& sample)
{
	u16 section;
	u16 id;
	u32 size;

	ReadElementHeader(rIn, id, size);

	if(id != kElementSample || size != 0x00000018)
		return false;

	sample.x = ReadValue<float>(rIn);
	sample.y = ReadValue<float>(rIn);
	sample.z = ReadValue<float>(rIn);
	sample.d = ReadValue<float>(rIn);

	section = ReadValue<u16>(rIn);

	return true;
}

bool ReadSpinePoint(Input& rIn, SpinePoint& sample)
{
	u16 section;
	u16 id;
	u32 size;

	ReadElementHeader(rIn, id, size);

	if(id != kElementSample || size != 0x00000018)
		return false;

	sample.x = ReadValue<float>(rIn);
	sample.y = ReadValue<float>(rIn);
	sample.z = ReadValue<float>(rIn);
	sample.d = ReadValue<float>(rIn);

	section = ReadValue<u16>(rIn);

	return true;
}

// temp
static bool g_bAllContoursAsSoma = false;
static bool g_useSomaHint = false;
static String g_SomaHint;
static bool IsCellBodyName(String name)
{
	return	g_bAllContoursAsSoma || 
			(name == "Cell Body" || name == "CellBody" || StringContains(name, "Soma")) ||
			(g_useSomaHint && StringContains(name, g_SomaHint));
}
//

bool ImportNeurolucidaDAT(Input& rIn, Neuron3D& nrn, const HintOptions& options)
{
	// configure temp
	g_useSomaHint = options.hasHintSoma;
	if(g_useSomaHint)
		g_SomaHint = options.hintSomaSubString;

	g_bAllContoursAsSoma = options.allContoursAsSoma;
	//

	if( ReadHeader(rIn) )
	{
		MorphologyBuilder builder(nrn);

		u16		subElementsRemaining = 0;
		u16		markerSetsRemaining = 0;

		bool	doingCellBody	= false;
		bool	doingTree		= false;
		bool	doingMarkers	= false;
		bool	doingSpines		= false;
		bool	doingGlobalMarkers = false;

		bool	skippingContour = false;

		bool startingNewTree = false;
		TreeType newTreeType;
		SamplePoint newTreeRootSample;

		bool	unexpectedStructure = false;

		String currentShapeName;
		String currentMarkerName;

		bool forcedTermination = false;

		while(rIn.remaining() && !forcedTermination)
		{
			u16	elementID;
			u32	elementSize;

			ReadElementHeader(rIn, elementID, elementSize);
			assert(elementSize >= kSizeElementHeader);

			switch(elementID)
			{
			case kElementPropertyGroup:
				subElementsRemaining = ReadValue<u16>(rIn);
				break;

			case kElementProperty:
				assert(subElementsRemaining > 0);
				SkipData(rIn, elementSize - kSizeElementHeader);
				--subElementsRemaining;
				break;

			case kElementImageData:
				SkipData(rIn, elementSize - kSizeElementHeader);
				break;

			case kElementContour:
				{
					skippingContour = false;

					u16 nameElementId;
					u32 nameElementSize;

					ReadElementHeader(rIn, nameElementId, nameElementSize);
					String objectName = ReadString(rIn, nameElementSize - kSizeElementHeader);


					const u32 curveType = ReadValue<u16>(rIn);

					// not interested in colour information (at least not for now)
					SkipData(rIn, kSizeColour + sizeof(u16));

					bool hasCellBodyProperty = false;


					// extract marker properties...
					u16 groupId;
					u32 groupSize;
					ReadElementHeader(rIn, groupId, groupSize);
					assert(groupId == kElementPropertyGroup);
					u16 numSub = ReadValue<u16>(rIn);

					while(numSub > 0)
					{
						u16 propertyId;
						u32 propertySize;

						ReadElementHeader(rIn, propertyId, propertySize);
						assert(propertyId == kElementProperty);

						u16 propertyNameId;
						u32 propertyNameSize;
						ReadElementHeader(rIn, propertyNameId, propertyNameSize);
						assert(propertyNameId == kElementString);

						String propertyName = ReadString(rIn, propertyNameSize - kSizeElementHeader);

						if(propertyName == "CellBody")
						{
							hasCellBodyProperty = true;
						}

						// skip remaining data..
						SkipData(rIn, propertySize - kSizeElementHeader - propertyNameSize);

						--numSub;
					}


					if(	hasCellBodyProperty || IsCellBodyName(objectName) )
					{
						doingCellBody = true;
						switch(curveType)
						{
						default:
						case kCurveClosed:
							builder.NewSomaClosedContour();
							break;
						case kCurveOpen:
							builder.NewSomaOpenContour();
							break;
						}
					}
					else
					{
						skippingContour = true;
					}
				}
				break;

			case kElementSampleList:
				{
					// || doingGlobalMarkers ?
					assert(doingCellBody || doingTree || skippingContour || doingGlobalMarkers);
					assert( !(doingCellBody && doingTree) );

					const u32 numSamples = ReadValue<u16>(rIn);

					for(u32 i=0;i<numSamples;++i)
					{

						if(doingMarkers)
						{
							MarkerPoint sample;
							sample.shape = NeurolucidaShapeName2MarkerShape(currentShapeName);
							const bool ok = ReadMarkerPoint(rIn, sample);
							assert(ok);

							if(doingCellBody)
								builder.CsAddMarker(currentMarkerName, sample);
							else if(doingTree)
								builder.CbAddMarker(currentMarkerName, sample);
							else
								builder.AddMarker(currentMarkerName, sample);
						}
						else
						{
							u16 id;
							u32 size;

							ReadElementHeader(rIn, id, size);

							if(id != kElementSample || size != 0x00000018)
								return false;

							SamplePoint sample;
							const bool ok = ReadSamplePoint(rIn, sample);
							assert(ok);

							if(doingCellBody)
								builder.CsAddSample(sample);
							else if(doingTree)
								builder.CbAddSample(sample);
						}
					}


					doingMarkers	= false;
					doingGlobalMarkers = false;
					doingSpines		= false;
					doingCellBody	= false;
				}
				break;

			case kElementSample:
				{
					assert(doingTree);
					assert(elementSize == 0x00000018);

					assert(startingNewTree);
					assert(	newTreeType == kTreeApical ||
							newTreeType == kTreeBasal ||
							newTreeType == kTreeDendrite ||
							newTreeType == kTreeAxon);

					const bool ok = ReadSamplePoint(rIn, newTreeRootSample);
					assert(ok);
				}

				break;

			// Trees
			case kElementTree:
				{
					const u32 treeType = ReadValue<u16>(rIn);

					newTreeType = (TreeType)treeType;
					startingNewTree = true;

					doingTree = true;

					SkipData(rIn, kSizeColour + sizeof(u16));
				}
				break;

			case kElementSubTree:
			case kElementSubTree2:
				{
					assert(doingTree);

					u16 terminalType = ReadValue<u16>(rIn);
					u16 numChildren = ReadValue<u16>(rIn);

					if(startingNewTree)
					{
						startingNewTree = false;
						switch(newTreeType)
						{
						case kTreeApical:
							builder.NewDendriteWithSample(newTreeRootSample);
							builder.CdSetApical();
							break;
						default:
						case kTreeDendrite:
						case kTreeBasal:
							builder.NewDendriteWithSample(newTreeRootSample);
							break;

						case kTreeAxon:
							builder.NewAxonWithSample(newTreeRootSample);
							break;
						}
					}
					else
					{

						MorphologyBuilder::Branch parent = builder.CurrentBranch().GetParent();
						if(parent != MorphologyBuilder::NullBranch())
						{
							// update number of current branch's parent's children.
							u32 parentChildrenRemaining = builder.GetTagForBranch(parent, kTagChildrenLeft);
							--parentChildrenRemaining;
							builder.SetTagForBranch(parent, kTagChildrenLeft, parentChildrenRemaining);

							// ascend tree until we find a parent which is still waiting for children
							while( builder.CbGetTag(kTagChildrenLeft) == 0)
							{
								builder.CbToParent();
							}
						}
						//

						builder.CbNewChildFirst(true);
					}

					builder.CbSetTag(kTagChildrenLeft, numChildren);

					// TODO: better terminal type conversion..
					if(terminalType != kTerminalBranchPoint)
						builder.CbSetTerminalType( (TerminalType) (NeurolucidaTerminalType) terminalType );
					//
				}

				break;

			// Markers
			case kElementMarkerSection:
				{
					assert(markerSetsRemaining == 0);

					markerSetsRemaining = ReadValue<u16>(rIn);

					if(markerSetsRemaining > 0)
					{
					}
				}
				break;

			case kElementMarkerSet:
				{
					// global markers ! ?
					if(markerSetsRemaining == 0)
					{
						//assert(!doingCellBody);
						//assert(!skippingContour);
						assert(!doingTree);
						assert(!doingSpines);

						doingGlobalMarkers = true;
						markerSetsRemaining = 1;
					}

					//assert(doingMarkers);
					assert(markerSetsRemaining > 0);
					--markerSetsRemaining;

					// marker size info is unreliable..

					// extract marker shape name..
					u16 stringId;
					u32 stringSize;
					ReadElementHeader(rIn, stringId, stringSize);
					assert(stringId == kElementString);
					currentShapeName = ReadString(rIn, stringSize - kSizeElementHeader);

					// skip colour
					SkipData(rIn, 4);

					// extract marker properties...
					u16 groupId;
					u32 groupSize;
					ReadElementHeader(rIn, groupId, groupSize);
					assert(groupId == kElementPropertyGroup);
					u16 numSub = ReadValue<u16>(rIn);

					while(numSub > 0)
					{
						// read properties
						u16 propertyId;
						u32 propertySize;

						ReadElementHeader(rIn, propertyId, propertySize);
						assert(propertyId == kElementProperty);

						u16 propertyNameId;
						u32 propertyNameSize;
						ReadElementHeader(rIn, propertyNameId, propertyNameSize);
						assert(propertyNameId == kElementString);

						String propertyName = ReadString(rIn, propertyNameSize - kSizeElementHeader);

						if(propertyName == "Name")
						{
							u16 val1 = ReadValue<u16>(rIn); // ?
							u16 val2 = ReadValue<u16>(rIn); // ?
							u16 nameValueId;
							u32 nameValueSize;
							ReadElementHeader(rIn, nameValueId, nameValueSize);
							assert(nameValueId == kElementString);
							currentMarkerName = ReadString(rIn, nameValueSize - kSizeElementHeader);
						}
						else
						{
							// skip remaining data..
							SkipData(rIn, propertySize - kSizeElementHeader - propertyNameSize);
						}

						--numSub;
					}

					doingMarkers = true;
				}
				break;

			// Spines
			case kElementSpineSection:
				{
					u32 numSpines = ReadValue<u16>(rIn);

					u16 spineId;
					u32 spineSize;

					SpinePoint spine;
					for(u32 n=0;n<numSpines;++n)
					{
						ReadElementHeader(rIn, spineId, spineSize);

						assert(spineId == kElementSpine);
						//assert(spineSize == 0x2e);
						u32 colour = ReadValue<u32>(rIn);
						u16 numSpinesMaybe = ReadValue<u16>(rIn);
						assert(numSpinesMaybe == 1);

						// group
						u16 groupId;
						u32 groupSize;
						ReadElementHeader(rIn, groupId, groupSize);
						assert(groupId == kElementPropertyGroup);

						//assert(groupSize == 0x8);
						//u16 zero = ReadValue<u16>(rIn);
						//assert(zero == 0);

						//todo: read spine type
						SkipData(rIn, groupSize - kSizeElementHeader);

						//
						u32 associatedSample = ReadValue<u16>(rIn);
						assert(associatedSample <= builder.CurrentBranch().NumSamples());

						bool ok = ReadSpinePoint(rIn, spine);
						assert(ok);

						if(associatedSample == 0 && builder.CurrentBranch().GetParent())
						{
							LogImportWarning("Associated Sample is 0 (non-root branch). Adding spine to parent branch, associating it with last sample.");

							MorphologyBuilder::Branch parent = builder.CurrentBranch().GetParent();

							spine.associatedSegment = parent.NumSamples() - 1;
							parent.AddSpine(spine);
						}
						else
						{
							spine.associatedSegment = associatedSample;

							// To help with testing (comparing with NeurolucidaASC, NeurolucidaXML, and NeuroML), 
							// spines are added in associatedSample order, DAT does not guarantee the right order!
							builder.CbAddSpine(spine);
						}
					}
				}
				break;

			case kElementSpine:
				// 0x0206, size, colour, type, empty group, unknown, normal sample data
				assert(0);
				break;

			// End of file...
			case datFooter1:
				{
					// never gets here, as final tag seems to overlap last 2 bytes of previous structure..
					u16 test = ReadValue<u16>(rIn);
					assert(test == datFooter2);
				}
				break;

			case datFooter2:
				{
					u16 test = ReadValue<u16>(rIn);
					assert(test == datFooter1);

					// do this just in case there is corrupted structure at the end of th NRX file..
					forcedTermination = true;
				}
				break;

			case kElementTextTag:
				SkipData(rIn, elementSize - kSizeElementHeader);
				break;

			case kElementThumbnail:
			case kElementDescription:
				// something to do here ?
				SkipData(rIn, elementSize - kSizeElementHeader);
				break;

			default:
				{
					LogImportWarning("Trying to safely skip a tag we don't recognize: " + dec(elementID) );
					assert(0);
					SkipData(rIn, elementSize - kSizeElementHeader);
					WriteToLog( String("File format warning - Skipping unexpected element tag: 0x")  + hex(elementID) );
				}
			}

			if(unexpectedStructure)
			{
				LogImportFailure( "Unexpected structure" );
				return false;
			}

			if(rIn.didFail())
			{
				LogImportFailure("Input failure");
				return false;
			}

			if(builder.HasFailed())
			{
				LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
				return false;
			}
		}

		builder.ApplyChanges();

		nrn.MergeAllUnaryBranchPoints();
		nrn.EnhanceSingleSampleTrees();

		return true;
	}

	return false;
}


bool ImportRawNeurolucidaDAT(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportNeurolucidaDAT(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//


static const char kCellBodyName[] = "Cell Body";

enum MarkerLocation
{
	kMarkerGlobal,
	kMarkerContour,
	kMarkerTree
};

struct ColourValue
{
	u8 r;
	u8 g;
	u8 b;
	u8 a;

	ColourValue() {}
	ColourValue(u8 _r, u8 _g, u8 _b) : r(_r), g(_g), b(_b) {}
};

struct ColourData
{
	ColourValue soma;
	ColourValue axon;
	ColourValue dend;
	ColourValue marker;
	ColourValue spine;
};

bool IsRGBFormat(const String& s)
{
	return StringBegins(s, "RGB");
}

ColourValue String2ColourValue(const String& s)
{
	u8 r,g,b;

	const NeurolucidaColour colourId = NeurolucidaColourString2Id(s);

	if(colourId != kColour_Unknown)
	{
		GetNeurolucidaColourRGB(colourId, r, g, b);
		return ColourValue(r,g,b);
	}
	else if( IsRGBFormat(s) )
	{
		Strings values = StringSplit( StringBefore( StringAfter(s, '('), ')'), ':');

		assert(values.size() == 3);

		r = std::stof( values[0] );
		g = std::stof( values[1] );
		b = std::stof( values[2] );

		return ColourValue(r,g,b);
	}
	else
	{
		return ColourValue(0xff, 0x00, 0x00);
	}
}

template<typename T>
void WriteValue(Output& rOut, T val)
{
	eswap(val);
	rOut.write(val);
}

void WriteElementHeader(Output& rOut, u16 id, u32 size)
{
	eswap(id);
	eswap(size);

	rOut.write(id);
	rOut.write(size);
}

u32 CalculateNameElementSize(const char* name)
{
	return kSizeElementHeader + strlen(name);
}

template<typename T>
u32 CalculateSampleListSize(const std::vector<T>& s, bool ignoreFirst)
{
	u32 size = s.size();

	if(ignoreFirst)
		--size;

	return 
		kSizeElementHeader + sizeof(u16) +
		(size * kSizeSample);
}

u32 CalculateNamePropertySize(const String& name)
{
	return	kSizeElementHeader + 
			CalculateNameElementSize("Name") +
			sizeof(u16) +
			sizeof(u16) +
			CalculateNameElementSize(name.c_str());
}

u32 CalculateGroupMarkerPropertiesSize(const String& name, const Branch3D::MarkerSet& markers)
{
	return	kSizeElementHeader + sizeof(u16) +
			CalculateNamePropertySize(name);
			// + any additional properties - varicosity, etc.. ??
}

u32 CalculateMarkerSetSize(const String& name, const Branch3D::MarkerSet& markers, bool corrected)
{
	if(corrected)
	{
		const u32 shapeId = markers[0].shape;
		String shape = NeurolucidaMarkerShape2ShapeName(shapeId);

		return	kSizeElementHeader + 
				CalculateNameElementSize(shape.c_str()) + 
				sizeof(u32) + // colour
				CalculateGroupMarkerPropertiesSize(name, markers) + 
				CalculateSampleListSize(markers, false);
	}
	else
	{
		// this isn't perfect..but not yet sure how the values are incorrectly calculated..
		// works for all available test data, except case where multiple marker sets in a contour.. :(
		return	0x2b + 
				CalculateGroupMarkerPropertiesSize(name, markers);
	}
}

u32 CalculateMarkerInfoSize(const Branch3D::Markers& markers, MarkerLocation location, bool corrected)
{
	u32 size = 0;

	if(location != kMarkerGlobal)
		size += kSizeElementHeader + sizeof(u16);

	for(Branch3D::Markers::const_iterator it=markers.begin();it!=markers.end();++it)
	{
		if( it->second.size() > 0 )
		{
			size += CalculateMarkerSetSize(it->first, it->second, corrected);
		}
	}

	return size;
}

u32 CalculateSpineSize()
{
	return	kSizeElementHeader + 
			kSizeColour + 
			sizeof(u16) + // numSpines
			kSizeElementHeader + sizeof(u16) + // empty group
			sizeof(u16) + // associated sample
			kSizeSample;
}

u32 CalculateSpineInfoSize(const std::vector<SpinePoint>& s)
{
	return kSizeElementHeader + sizeof(u16) + (s.size() * CalculateSpineSize());
}

u32 CalculateContourSize(const Neuron3D::SomaElement& s, bool corrected)
{
	return	// First the curve info
			kSizeElementHeader + 
			CalculateNameElementSize(kCellBodyName) + 
			kSizeCurveType + 
			kSizeColour + 
			sizeof(u16) +
			0x1E + // group with (CellBody)
			// Sample list
			CalculateSampleListSize(s.m_samples, false) + 
			CalculateMarkerInfoSize(s.m_markers, kMarkerContour, corrected);
}

void WriteHeader(Output& rOut)
{
	rOut.write(datHeader, headerSize);
}

void WriteEmptyPropertyGroup(Output& rOut)
{
	WriteElementHeader(rOut, kElementPropertyGroup, 8);
	WriteValue<u16>(rOut, 0);
}

void WriteString(Output& rOut, const char* name)
{
	WriteElementHeader(rOut, kElementString, CalculateNameElementSize(name) );
	rOut.write(name, strlen(name));
}

void WriteColour(Output& rOut, ColourValue col)
{
	WriteValue<u8>(rOut, col.r);
	WriteValue<u8>(rOut, col.g);
	WriteValue<u8>(rOut, col.b);
	WriteValue<u8>(rOut, 0);
}

template<typename T>
void WriteSample(Output& rOut, const T& s, u16 section)
{
	WriteElementHeader(rOut, kElementSample, kSizeSample);

	WriteValue<float>(rOut, s.x);
	WriteValue<float>(rOut, s.y);
	WriteValue<float>(rOut, s.z);
	WriteValue<float>(rOut, s.d);

	WriteValue<u16>(rOut, section);
}

template<typename T>
void WriteSampleList(Output& rOut, const std::vector<T>& s, bool ignoreFirst)
{
	u32 size = s.size();

	if(ignoreFirst)
		--size;

	WriteElementHeader(rOut, kElementSampleList, CalculateSampleListSize(s, ignoreFirst) );
	WriteValue<u16>(rOut, size);

	typename std::vector<T>::const_iterator it = s.begin();

	if(ignoreFirst)
		++it;

	for(; it != s.end(); ++it)
	{
		WriteSample(rOut, *it, 0); 
	}
}

String GetShapeNameFromId(u32 id)
{
	if( IsNeurolucidaMarkerShapeKnown(id) )
		return NeurolucidaMarkerShape2ShapeName(id);
	else
		return String("FilledCircle");
}

/** 
	
	Global marker data (i.e. marker data which appears before any contour data) starts directly with 0x0204 blocks

	Contour marker data starts with 0x0205, but seems to be zero size, then followed by 0x0204 

	Tree marker data starts with 0x0205, with an odd incorrect size that seems dependent mainly on the marker name string, then followed by 0x0204 blocks

	For contours, when there are multiple sets of markers, first is not the expected incorrect size, while for trees it is..

	We have enough information to import and export marker data, but **NEED MORE DATA TO BE ABLE TO INVESTIGATE FURTHER**.

**/

void WriteMarkerInfo(Output& rOut, const Branch3D::Markers markers, MarkerLocation location, bool corrected, ColourValue colour)
{
	if(location != kMarkerGlobal)
	{
		if(location == kMarkerContour)
		{
			WriteElementHeader(rOut, kElementMarkerSection, 8);
			WriteValue<u16>(rOut, 0);
		}
		else
		{
			u32 numMarkerSets = markers.size();
			u32 sectionSize = CalculateMarkerInfoSize(markers, location, corrected);

			WriteElementHeader(rOut, kElementMarkerSection, sectionSize);
			WriteValue<u16>(rOut, numMarkerSets);
		}
	}


	for(Branch3D::Markers::const_iterator it=markers.begin();it!=markers.end();++it)
	{
		if( it->second.size() > 0 )
		{
			u32 groupSize = CalculateMarkerSetSize(it->first, it->second, corrected);

			String shapeName = GetShapeNameFromId(it->second[0].shape);

			// Shape tag
			WriteElementHeader(rOut, kElementMarkerSet, groupSize);
			WriteString(rOut, shapeName.c_str());
			WriteColour(rOut, colour);

			const u16 numProperties = 0x0001;

			WriteElementHeader(rOut, kElementPropertyGroup, groupSize);
			WriteValue<u16>(rOut, numProperties);

			// Marker name
			u32 nameSize1 = CalculateNameElementSize("Name");
			WriteElementHeader(rOut, kElementProperty, nameSize1);
			WriteString(rOut, "Name");

			WriteValue<u16>(rOut, 0x0001);
			WriteValue<u16>(rOut, 0x0001);

			WriteString(rOut, it->first.c_str());

			// No other properties for now?

			WriteSampleList(rOut, it->second, false);
		}
	}
}

static void WriteSpine(Output& rOut, const SpinePoint& spine, u32 associatedSample, ColourValue colour)
{
	WriteElementHeader(rOut, kElementSpine, CalculateSpineSize() );
	WriteColour(rOut, colour);
	WriteValue<u16>(rOut, 0x0001); // 1 spine
	WriteEmptyPropertyGroup(rOut);
	WriteValue<u16>(rOut, associatedSample);
	WriteSample(rOut, spine, 0);
}

static void WriteSpineInfo(Output& rOut, const std::vector<SpinePoint>& spines, const std::vector<SamplePoint>& samples, ColourValue colour)
{
	u16 numSpines = (u16)  spines.size();
	u32 sectionSize = CalculateSpineInfoSize(spines);

	WriteElementHeader(rOut, kElementSpineSection, sectionSize);
	WriteValue<u16>(rOut, numSpines);

	for(u16 n=0;n<numSpines;++n)
	{
		const u32 associatedSample = spines[n].associatedSegment;
		WriteSpine(rOut, spines[n], associatedSample, colour);
	}
}

static void WriteContour(Output& rOut, const Neuron3D::SomaElement& s, CurveType curveType, bool corrected, const ColourData& colours)
{
	WriteElementHeader(rOut, kElementContour, CalculateContourSize(s, corrected) );
	WriteString(rOut, kCellBodyName);

	WriteValue<u16>(rOut, curveType);
	WriteColour(rOut, colours.soma);
	WriteValue<u16>(rOut, 0);

	// Properties
	WriteElementHeader(rOut, kElementPropertyGroup, 0x1E);
	// Write CellBody property
	WriteValue<u16>(rOut, 0x0001);
	WriteElementHeader(rOut, kElementProperty, 0x16);
	WriteString(rOut, "CellBody");
	WriteValue<u16>(rOut, 0x0000);
	//

	WriteSampleList(rOut, s.m_samples, false);
	WriteMarkerInfo(rOut, s.m_markers, kMarkerContour, corrected, colours.marker);
}

static u32 CalculateSubTreeSize(Neuron3D::DendriteTreeConstIterator it, u32 currentSize, bool rootBranch, bool corrected)
{
	currentSize += 
		kSizeElementHeader + 
		kSizeTerminalType + sizeof(u16) +
		CalculateSampleListSize( (*it).m_samples, true ) + 
		CalculateMarkerInfoSize( (*it).m_markers, kMarkerTree, corrected ) + 
		CalculateSpineInfoSize( (*it).m_spines );

	if(it.child())
		currentSize = CalculateSubTreeSize(it.child(), currentSize, false, corrected);
	if(it.peer() && !rootBranch)
		currentSize = CalculateSubTreeSize(it.peer(), currentSize, false, corrected);

	return currentSize;
}

static u32 CalculateTreeSize(const Neuron3D::DendriteTree& d, bool corrected)
{
	return 
		kSizeElementHeader + 
		kSizeTreeType + 
		kSizeColour + 
		sizeof(u16) +
		// empty group!?
		kSizeElementHeader + sizeof(u16) +
		//
		kSizeSample + 
		CalculateSubTreeSize(d.root(), 0, true, corrected);
}


static void WriteEmptyImageData(Output& rOut, u32 width, u32 height)
{
	const u32 numPixels		= width*height;
	const u32 sizeImage		= numPixels*4;
	const u32 sizeHeader	= kSizeElementHeader + 2*sizeof(u16);
	const u32 sizeBlock		= sizeImage + sizeHeader;

	WriteElementHeader(rOut, kElementImageData, sizeBlock); // size of 64x64x4 image + 10 bytes header
	WriteValue<u16>(rOut, width);
	WriteValue<u16>(rOut, height);

	for(u32 n=0;n<sizeImage;++n)
		WriteValue<u8>(rOut, 0);
}

static void WriteSubTree(Output& rOut, Neuron3D::DendriteTreeConstIterator it, bool corrected, const ColourData& colours)
{
	u16 numChildren = Neuron3D::DendriteTree::countchildren(it);
	TerminalTypeVal type = (numChildren > 0) ? kTerminalBranchPoint : (TerminalTypeVal) (NeurolucidaTerminalType) (*it).GetTerminalType() ;

	WriteElementHeader(rOut, kElementSubTree, CalculateSubTreeSize(it, 0, true, corrected));
	WriteValue<u16>(rOut, type);
	WriteValue<u16>(rOut, numChildren);

	WriteSampleList( rOut, (*it).m_samples, true);
	WriteMarkerInfo( rOut, (*it).m_markers, kMarkerTree, corrected, colours.marker);
	WriteSpineInfo(rOut, (*it).m_spines, (*it).m_samples, colours.spine);

	if(it.child())
		WriteSubTree(rOut, it.child(), corrected, colours);
	if(it.peer())
		WriteSubTree(rOut, it.peer(), corrected, colours);
}

static void WriteTree(Output& rOut, const Neuron3D::DendriteTree& d, TreeType type, bool corrected, const ColourData& colours)
{
	WriteElementHeader(rOut, kElementTree, CalculateTreeSize(d, corrected) );
	WriteValue<u16>(rOut, type);
	WriteColour(rOut, (type == kTreeAxon) ? colours.axon : colours.dend); // arbitrary colours
	WriteValue<u16>(rOut, 0);
	WriteEmptyPropertyGroup(rOut);
	WriteSample(rOut, d.root()->m_samples[0], 0);

	WriteSubTree(rOut, d.root(), corrected, colours);
}


bool ExportNeurolucidaDAT(Output& rOut, const Neuron3D &nrnOrig, const StyleOptions& options)
{
	// configure
	const bool corrected = (options.formatStyle == kStyleNLDat_Corrected);

	ColourData colours;
	colours.soma	= String2ColourValue(options.somaColour);
	colours.axon	= String2ColourValue(options.axonColour);
	colours.dend	= String2ColourValue(options.dendColour);
	colours.marker	= String2ColourValue(options.markerColour);
	colours.spine	= String2ColourValue(options.spineColour);

	// DAT format has children saved out in reverse order to ASC...
	// Brute force method - copy entire tree, then reverse order..
	Neuron3D nrn(nrnOrig);

	nrn.ReverseChildOrderAllTrees();
	//


	// headers
	WriteHeader(rOut);

	// Do we want to write any section info : Yes, looks like empty section info required.
	WriteEmptyPropertyGroup(rOut); // empty section

	// Don't know if this is essential, but write it anyway
	WriteEmptyImageData(rOut, 0x40, 0x40); // 64x64

	// global markers
	WriteMarkerInfo(rOut, nrn.m_globalData.m_markers, kMarkerGlobal, corrected, colours.marker);

	// somas
	for(Neuron3D::SomaConstIterator s = nrn.SomaBegin(); s!=nrn.SomaEnd(); ++s)
		WriteContour(rOut, *s, (s->somaType == Soma3D::kContourClosed ? kCurveClosed : kCurveOpen), corrected, colours);

	// axons
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a!=nrn.AxonEnd(); ++a)
		WriteTree(rOut, *a, kTreeAxon, corrected, colours);

	// dendrites
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d!=nrn.DendriteEnd(); ++d)
	{
		if( (*d).GetTreeType() == Neuron3D::DendriteTree::Apical)
			WriteTree(rOut, *d, kTreeApical, corrected, colours);
		else
			WriteTree(rOut, *d, kTreeDendrite, corrected, colours);
	}

	// footer
	WriteValue<u32>(rOut, datFooter);

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateNeurolucidaDAT(Input& rIn, const ValidationOptions& options)
{
	return ReadHeader(rIn) ? kValidationTrue : kValidationFalse;
}
