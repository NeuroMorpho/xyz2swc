//
// FileFormatNeurolucida.cpp
//

#include "ImportExportCommonIncludes.h"

#include <math.h>
#include <algorithm>
#include <stdlib.h>

#include "Core/Parser.h"

#include "NeurolucidaMarkers.h"
#include "NeurolucidaColours.h"
#include "NeurolucidaTerminals.h"

/** 

	Neurolucida ASC file format

**/


enum StructureTypeASC
{
	kStructureGlobal,
	kStructureContour,
	kStructureTree
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import 
//

static bool g_bAllContoursAsSoma = false;
static bool g_useSomaHint = false;
static String g_SomaHint;
static bool IsCellBodyName(String name)
{
	return	g_bAllContoursAsSoma ||
			(name == "Cell Body" || name == "CellBody" || StringContains(name, "Soma"))  ||
			(g_useSomaHint && StringContains(name, g_SomaHint));
}


class NeurolucidaParser : public SimpleParser
{
private:
	vector<String> keyword_ignore;
	vector<String> keyword_marker;
	vector<String> m_sections;

	MorphologyBuilder builder;

	int level;

	Neuron3D& m_nrn;


	enum ImportError
	{
		kError
	};

	void Init()
	{
		SetIgnoreAfter(';');

		SetToken(')');
		SetToken('(');
		SetToken('<');
		SetToken('>');

		SetToken('\"');
		SetToken('|');
	}

	void SkipBlock()
	{
		int count=1;
		String item;

		while(count && rIn.remaining() > 0)
		{
			SeekNextItem();
			item = GetNextItem();
			if( item == "(") count++;
			if( item == ")") count--;
		}
	}

public:

	NeurolucidaParser(Input& rIn, Neuron3D &nrn) : SimpleParser(rIn), builder(nrn), m_nrn(nrn)
	{
		Init();
		level = 0;
	}

	// Sections
	bool ExtractSectionData()
	{
		for(;;)
		{
			String item;
			SeekNextItem();
			item = GetNextItem();

			if( item == ")")
			{
				break;
			}
			else
			{
				// new section
				m_sections.push_back(item);

				// carefully bypass section name
				SeekNextItem();
				item = GetQuotedItem();

				// skip the next two items
				SeekStartOfNextLine();
				//SeekNextItem(); SeekEndOfItem();
				//SeekNextItem(); SeekEndOfItem();
			}
		}

		return true;
	}

	bool IsIgnorableBlock_TopLevel(const String& item)
	{
		return	item == "Description" ||
				item == "ImageCoords" ||
				item == "Thumbnail" || 
				item == "Sections" ||
				StringBegins(item, "SSM");
	}

	bool IsIgnorableBlock_Contour(const String& item)
	{
		return	item == "Color" ||
				item == "Style" ||
				item == "Resolution" ||
				item == "GUID" ||		//meyer
				item == "MBFObjectType"||//meyer
				item == "Set";
	}

	bool IsIgnorableBlock_Tree(const String& item)
	{
		return	item == "Color" ||
				item == "Style" || 
				item == "Set";
	}

	bool IsIgnorableBlock_MarkerSet(const String& item)
	{
		return	item == "Color" ||
				item == "Varicosity";
	}

	bool IsTreeName(const String& item)
	{
		return	item == "Dendrite" || 
				item == "Apical" || 
				item == "Axon";
	}

	bool IsValidFloat(const String& item)
	{
		const char* startptr = item.c_str();
		char* endptr;
		u32 length = item.length();
		double result = strtod(startptr, &endptr);

		if(result == 0.0f)
		{
			if(endptr < startptr + length)
				return false;
		}

		return true;
	}

	bool ExtractSample(SamplePoint& data)
	{
		// opening bracket '(' should already be passed
		SeekNextItem();
		data.x = GetNextItemAsFloat();
		SeekNextItem();
		data.y = GetNextItemAsFloat();
		SeekNextItem();
		data.z = GetNextItemAsFloat();
		SeekNextItem();
		data.d = GetNextItemAsFloat();

		SeekNextItem();
		String next = GetNextItem();

		if( next != ")")
		{
			// could be section info...so go to next..
			SeekNextItem();
			String nextitem = GetNextItem();
			if( nextitem != ")")
			{
				LogImportFailure("Malformed sample point!");
				return false;
			}
		}

		return true;
	}

	bool ExtractSpine(SpinePoint& data, u32 currentSegment)
	{
		// opening square bracket '<' should already be passed

		SeekNextItem();
		String next = GetNextItem();

		if(next == "(")
		{
			SamplePoint pt;
			if(ExtractSample(pt))
			{
				SeekNextItem();
				next = GetNextItem();
				if(next == ">")
				{
					data.x = pt.x;
					data.y = pt.y;
					data.z = pt.z;
					data.d = pt.d;
					data.type = kSpineNoDetail;
					data.associatedSegment = currentSegment;
					return true;
				}
			}
		}

		LogImportFailure("Malformed spine point");
		return false;
	}

	bool ExtractMarkerData(StructureTypeASC structure, MorphologyBuilder::Branch branch = MorphologyBuilder::NullBranch())
	{
		String item = GetNextItem();
		MarkerShape shape = NeurolucidaShapeName2MarkerShape(item);

		String markerName;

		while(1)
		{
			SeekNextItem();
			String item2 = GetNextItem();

			if(item2 == "(")
			{
				SeekNextItem();
				item2 = GetNextItem();

				if( IsIgnorableBlock_MarkerSet(item2) )
				{
					SkipBlock();
				}
				else if(item2 == "Name")
				{
					SeekNextItem();
					markerName = GetQuotedItem();
					assert(markerName.length() != 0);
					SkipBlock();
				}
				else if( IsValidFloat(item2) )
				{
					MarkerPoint pt;
					pt.shape = shape;

					RollbackCurrentItem(item2);
					if(!ExtractSample(pt))
						return false;

					switch(structure)
					{
					case kStructureGlobal:
						builder.AddMarker(markerName, pt);
						break;
					case kStructureContour:
						builder.CsAddMarker(markerName, pt);
						break;
					case kStructureTree:
						branch.AddMarker(markerName, pt);
						break;
					}
				}
				else
				{
					LogImportWarning("Unexpected marker item: " + item2 + ". Trying to continue.");
					assert(0);
					SkipBlock();
					SkipBlock();
					break;
				}
			}
			else
			{
				if(item2 != ")")
				{
					LogImportFailure("Malformed marker data.");
					return false;
				}

				break;
			}
		}

		return true;
	}

	bool ExtractTree(MorphologyBuilder::Branch parentBranch)
	{
		MorphologyBuilder::Branch latestBranch = MorphologyBuilder::NullBranch();
		u32 latestSample = 0;

		bool createBranch = true;
		u32 branchesCreated = 0;
		String item;

		while(1)
		{
			SeekNextItem();
			item = GetNextItem();

			if(item == "(")
			{
				SeekNextItem();
				String nextitem = GetNextItem();

				if(IsIgnorableBlock_Tree(nextitem)) // skip any ignorable block
				{
					SkipBlock();
				}
				else if(IsValidFloat(nextitem))
				{
					RollbackCurrentItem(nextitem);

					if(createBranch)
					{
						branchesCreated++;

						// if dendrite root, has already created
						if(parentBranch)
							latestBranch = parentBranch.NewChildLast(true);
						else
							latestBranch = builder.CurrentBranch();

						createBranch = false;
					}

					SamplePoint pt;

					if(!ExtractSample(pt))
						return false;

					latestBranch.AddSample(pt);

					++latestSample;
				}
				else if(nextitem == "(")
				{
					if(!branchesCreated)
					{
						LogImportFailure("Attempt to increase branching order before creating branch at current level!");
						return false;
					}

					RollbackCurrentItem(nextitem);

					level++;
					if(!ExtractTree(latestBranch))
						return false;
					level--;
				}
				else if(nextitem == ")") // new branch was started...but didn't contain anything...let's forget we ever got here..
				{
					assert(0);
					LogImportWarning("New branch was started, but didn't contain any data!!");
				}
				else if(nextitem == "TreeOrder")
				{
					SkipBlock();
				}
				else
				{
					if( !IsNeurolucidaMarkerShapeName(nextitem) )
					{
						assert(0);
						LogImportWarning("Assuming previously unknown marker name: " + nextitem);
					}

					RollbackCurrentItem(nextitem);
					if(!ExtractMarkerData(kStructureTree, latestBranch))
					{
						LogImportFailure( String("Couldn't extract tree marker data: ") + nextitem );
						return false;
					}
				}
			}
			else if(item == "<")
			{
				const u32 sampleToConnect = latestBranch.GetParent() ? latestSample : (latestSample-1);
				// spine data
				SpinePoint pt;
				if(!ExtractSpine(pt, sampleToConnect))
					return false;

				latestBranch.AddSpine(pt);
			}
			else if( item == ")") // assume this is the end of data at this level...
			{
				if(!branchesCreated)
				{
					assert(0);
					LogImportWarning("End of this level of branching...but haven't encountered data??");
				}
				break;
			}
			else if( item == "|") // a peer branch...just skip this symbol...data should be next and will be picked up on next cycle
			{
				if(!branchesCreated)
				{
					LogImportFailure("Found '|' before first branch at current level was created!");
					return false;
				}
				createBranch = true;
				latestSample = 0;
			}
			else // should be a terminal type
			{
				if(! IsNeurolucidaTerminalTypeName(item) )
					LogImportWarning("Unexpected item: '" + item + "'. Assuming unknown terminal type.");
				else
				{
					const NeurolucidaTerminalType ttype = NeurolucidaTerminalName2TerminalType(item);

					// TODO: proper conversion between Neurolucida and general terminal types..
					builder.CbSetTerminalType( (const TerminalType)ttype);
					//
				}
			}
		}

		return true;
	}

	bool Parse()
	{
		String item;

		while(rIn.remaining() > 0)
		{
			// expect an open bracket first !
			SeekNextItem();

			if(rIn.remaining() == 0) break;

			item = GetNextItem();

			if(item != "(")
			{
				LogImportFailure("Badly formatted ASC file!");
				return false;
			}

			// get top level block 
			SeekNextItem();
			item = GetNextItem();


			// top-level blocks we don't care about
			if(	IsIgnorableBlock_TopLevel(item) )
			{
				SkipBlock();
			}

			// contours
			else if( StringBegins(item, "\""))
			{
				RollbackCurrentItem(item);
				item = GetQuotedItem();

				bool readContour	= IsCellBodyName(item);
				bool closed			= false;
				bool createContour	= true;

				while(1)
				{
					SeekNextItem();
					String nextitem = GetNextItem();
					if(nextitem != "(")
					{
						assert( nextitem == ")");
						break;
					}

					SeekNextItem();
					nextitem = GetNextItem();

					if(IsIgnorableBlock_Contour(nextitem))
					{
						SkipBlock();
					}
					else if(nextitem == "CellBody")
					{
						readContour = true;
						SkipBlock();
					}
					else if(nextitem == "Closed")
					{
						closed = true;
						SkipBlock();
					}
					else if( IsValidFloat(nextitem) )
					{
						if(!readContour)
						{
							SkipBlock(); // skip sample
							SkipBlock(); // skip contour
							break;
						}

						if(createContour)
						{
							if(closed)
								builder.NewSomaClosedContour();
							else
								builder.NewSomaOpenContour();

							createContour = false;
						}

						RollbackCurrentItem(nextitem);

						SamplePoint pt;
						if(!ExtractSample(pt))
						{
							LogImportFailure( String("Couldn't extract contour sample data: ") + item );
							return false;
						}

						builder.CsAddSample(pt);
					}
					else
					{
						if( !IsNeurolucidaMarkerShapeName(nextitem) )
						{
							assert(0);
							LogImportWarning("Assuming previously unknown marker name: " + nextitem);
						}

						RollbackCurrentItem(nextitem);
						if(!ExtractMarkerData(kStructureContour))
						{
							LogImportFailure("Couldn't extract contour marker data: " + nextitem );
							return false;
						}
					}
				}
			}

			// should be a tree !! (possibly Font keyword..)
			else if(item == "(")
			{
				RollbackCurrentItem(item);

				while(1)
				{
					SeekNextItem();
					String nextitem = GetNextItem();
					assert(nextitem == "(");
					SeekNextItem();
					nextitem = GetNextItem();

					if( IsIgnorableBlock_Tree(nextitem) )
					{
						SkipBlock();
					}
					else if( nextitem == "Font" )
					{
						// NOT A TREE - lets get out of here!
						SkipBlock();
						SkipBlock();
						break;
					}
					else if( IsTreeName(nextitem) )
					{
						SkipBlock();

						if(nextitem == "Axon")
							builder.NewAxon();
						else
						{
							builder.NewDendrite();

							if(nextitem == "Apical")
								builder.CdSetApical();
						}

						level++;
						if(!ExtractTree(MorphologyBuilder::NullBranch()))
							return false;
						level--;

						break;
					}
					// catch text block which aren't identified with Font keyword
					else if( IsValidFloat(nextitem) )
					{
						SkipBlock();
						SkipBlock();
						break;
					}
					else
					{
						report("Unsupported keyword: Warning!! ignoring block - " + nextitem );
						assert(0);
						SkipBlock();
						SkipBlock();
						break;
					}
				}
			}

			// global markers
			else
			{
				if( !IsNeurolucidaMarkerShapeName(item) )
				{
					assert(0);
					LogImportWarning("Assuming previously unknown marker name: " + item);
				}

				RollbackCurrentItem(item);
				if(!ExtractMarkerData(kStructureGlobal))
				{
					LogImportFailure( String("Couldn't extract global marker data: ") + item );
					return false;
				}
			}

			if(builder.HasFailed())
			{
				LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
				return false;
			}
		}

		builder.ApplyChanges();

		m_nrn.MergeAllUnaryBranchPoints();
		m_nrn.EnhanceSingleSampleTrees();

		return true;
	}

};


bool ImportNeurolucidaASC(Input& rIn, Neuron3D& nrn, const HintOptions& options)
{
	// configure
	g_useSomaHint = options.hasHintSoma;
	if(g_useSomaHint)
		g_SomaHint = options.hintSomaSubString;

	g_bAllContoursAsSoma = options.allContoursAsSoma;
	//

	nrn.Clear();
	NeurolucidaParser p(rIn, nrn);
	return p.Parse();
}


bool ImportRawNeurolucidaASC(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportNeurolucidaASC(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//


static String GetLevelSpace(u32 size)
{
	return PrependSpaces("", size);
}

struct NeurolucidaASCExporter
{
	const StyleOptions& options;

	bool showComments;
	bool showClassicTags;
	bool compactWhitespace;
	bool treePathPerSample;
	bool nmpmlHeader;

	String axonColourStr;
	String dendColourStr;
	String somaColourStr;
	String spineColourStr;
	String markerColourStr;

	String Colour2String(String colour)
	{
		if(StringContains(colour, "RGB"))
		{
			Strings rgb = StringSplit( StringBefore(StringAfter(colour, '('), ')'), ':');

			u8 r = std::stoi( rgb[0] );
			u8 g = std::stoi( rgb[1] );
			u8 b = std::stoi( rgb[2] );

			return "RGB (" + dec(r), + ", " + dec(g) + ", " + dec(b) + ")";
		}
		else
		{
			if( NeurolucidaColourString2Id(colour) != kColour_Unknown)
			{
				return colour;
			}
			else
			{
				return "White";
			}
		}
	}

	NeurolucidaASCExporter(const StyleOptions& opt) : options(opt) 
	{
		switch(options.formatStyle)
		{
		default:
		case kStyleNLAsc_Modern:
			showComments		= true;
			showClassicTags		= false;
			compactWhitespace	= false;
			treePathPerSample	= false;
			nmpmlHeader			= false;
			break;

		case kStyleNLAsc_Verbose:
			showComments		= true;
			showClassicTags		= false;
			compactWhitespace	= false;
			treePathPerSample	= true;
			nmpmlHeader			= false;
			break;

		case kStyleNLAsc_ClassicTags:
			showComments		= true;
			showClassicTags		= true;
			compactWhitespace	= false;
			treePathPerSample	= true;
			nmpmlHeader			= false;
			break;

		case kStyleNLAsc_NoComments:
			showComments		= false;
			showClassicTags		= false;
			compactWhitespace	= false;
			treePathPerSample	= false;
			nmpmlHeader			= false;
			break;

		case kStyleNLAsc_Compact:
			showComments		= false;
			showClassicTags		= false;
			compactWhitespace	= true;
			treePathPerSample	= false;
			nmpmlHeader			= false;
			break;

		case kStyleNLAsc_NMPML:
			showComments		= false;
			showClassicTags		= false;
			compactWhitespace	= true;
			treePathPerSample	= false;
			nmpmlHeader			= true;
			break;
		}

		axonColourStr	= Colour2String(options.axonColour);
		dendColourStr	= Colour2String(options.dendColour);
		somaColourStr	= Colour2String(options.somaColour);
		spineColourStr	= Colour2String(options.spineColour);
		markerColourStr	= Colour2String(options.markerColour);
	}

	template<typename T>
	String FormatSample(const T& sample)
	{
		if(compactWhitespace)
		{
			return	"( " + dec(sample.x,2) + 
					 " " + dec(sample.y,2) + 
					 " " + dec(sample.z,2) + 
					 " " + dec(sample.d,2) + ")";
		}
		else
		{
			return	"(" +	PrependSpaces(dec(sample.x,2), 8) +
							PrependSpaces(dec(sample.y,2), 9, 1) +
							PrependSpaces(dec(sample.z,2), 9, 1) +
							PrependSpaces(dec(sample.d,2), 9, 1) + ")";
		}
	}

	String FormatSpine(const SpinePoint& sp)
	{
		return	"<" + FormatSample(sp) + ">";
	}

	String GetAxonColour()	{ return axonColourStr; }
	String GetDendColour()	{ return dendColourStr; }
	String GetSomaColour()	{ return somaColourStr; }
	String GetSpineColour() { return spineColourStr; }
	String GetMarkerColour(){ return markerColourStr; }

	String GetSomaName()	{ return options.customiseSomaName ? options.somaName : "CellBody"; }

	String GenerateClassicTag(u32 major, u32 minor)
	{
		if(showClassicTags)
			return String("  ;  [" + dec(major) + "," + dec(minor) + "]");
		else
			return String();
	}

	String AddComment(const String& str)
	{
		if(showComments)
			return String("  ; ") + str;
		else
			return "";
	}

	void WriteMarkers(Output& rOut, const Branch3D::Markers& markers, String space)
	{
		for(Branch3D::Markers::const_iterator it=markers.begin();it!=markers.end();++it)
		{
			u32 markerIndex = (*it).second[0].shape;

			if(markerIndex == kMarker_Invalid)
				markerIndex = kMarker_Dot;

			String markerType = NeurolucidaMarkerShape2ShapeName(markerIndex);
			String markerName = (*it).first;

			rOut.writeLine("");
			rOut.writeLine(space + "(" + markerType + GenerateClassicTag(3,markerIndex) );
			rOut.writeLine(space + "  (Color " + GetMarkerColour() + ")");
			rOut.writeLine(space + "  (Name \"" + markerName + "\")");

			for(u32 i=0; i<it->second.size();++i)
				rOut.writeLine(space + "  " + FormatSample(it->second[i]) + AddComment(dec(i+1)) );

			rOut.writeLine(space + ")" + AddComment(" End of markers") );
		}
	}

	void OutputNeurolucidaBranch(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int level, const String &route)
	{
		// samples first
		int count;

		// offset from start of line
		String levelspace = GetLevelSpace( (level+1) * 2 );

		vector<SamplePoint>::const_iterator s = (*it).m_samples.begin();

		if(level == 0)
		{
			count = 0;
		}
		else
		{
			count = 1;
			++s;
		}

		u32 numSpinesWritten = 0;

		for(; s!=(*it).m_samples.end(); ++s, ++count)
		{
			String outputStr = levelspace + FormatSample(*s);

			if(showComments)
			{
				if( s == (*it).m_samples.begin() )
				{
					outputStr += "  ; Root";
				}
				else
				{
					// seems to be 2 styles of hierarchy comments
					// 1 - each sample has branching route info and then sample number .
					// 2 - first sample has sample number and then route info. Remaining samples have only sample number.

					if(treePathPerSample)
					{
						outputStr += "  ; " + route + ", " + dec(count);
					}
					else
					{
						if(count == 1)
							outputStr += "  ; " + dec(count) + ", " + route;
						else
							outputStr += "  ; " + dec(count);
					}
				}
			}

			rOut.writeLine(outputStr);

			// output any spines associated with this sample..
			for(u32 n=0;n<(*it).m_spines.size();++n)
			{
				const SpinePoint& sp = (*it).m_spines[n];
				// twice the level space..
				if(sp.associatedSegment == count)
				{
					if(compactWhitespace)
						rOut.writeLine(levelspace + "  " + FormatSpine(sp) + AddComment("Spine") );
					else
						rOut.writeLine(levelspace + levelspace + FormatSpine(sp) + AddComment("Spine") );

					++numSpinesWritten;
				}
			}
		}

		if(numSpinesWritten != (*it).m_spines.size())
			LogImportWarning("Didn't write all spines for this branch!!");


		// then the markers
		WriteMarkers(rOut, (*it).m_markers, levelspace);


		if(it.child())
		{
			rOut.writeLine(levelspace + "(");

			String newroute(route);
			newroute += "-1";

			OutputNeurolucidaBranch( rOut, it.child(), level+1, newroute);
		}
		else
		{
			NeurolucidaTerminalType ttype = (NeurolucidaTerminalType)  (*it).GetTerminalType() ;
			String ttypeName = GetNeurolucidaTerminalTypeName(ttype);
			rOut.writeLine(levelspace + " " + ttypeName);
		}

		if(it.peer())
		{
			String levelspacePrev = GetLevelSpace( level * 2 );

			rOut.writeLine(levelspacePrev + "|");

			// rather clumsy way of updating route 
			const String lastNumberStr = StringAfterLast(route, "-");
			const int lastNumber = std::stoi( lastNumberStr );

			String newroute(route);
			StringReplaceLast(newroute, lastNumberStr, dec(lastNumber+1) );
			//

			OutputNeurolucidaBranch( rOut, it.peer(), level, newroute);
		}
		else
		{
			if(level > 0)
			{
				String levelspacePrev = GetLevelSpace( level * 2 );
				rOut.writeLine(levelspacePrev + ")" + AddComment(" End of split") );
			}
		}
	}


	bool Export(Output& rOut, const Neuron3D &nrn)
	{
		// headers
		if(nmpmlHeader)
			rOut.writeLine("; written by nmpml translation system\n");
		else
		{
			rOut.writeLine(";\tV3 text file written for MicroBrightField products.");
		}

		//
		if(options.identifyNL)
			rOut.writeLine(";\tWritten by " + options.appName + " (version " + options.appVersion + ")." );


		//
		rOut.writeLine("(Sections)");


		// global markers
		WriteMarkers(rOut, nrn.m_globalData.m_markers, String("") );

		// soma contours
		u32 contourCount = 0;
		for(Neuron3D::SomaConstIterator sit = nrn.SomaBegin(); sit!= nrn.SomaEnd();++sit)
		{
			++contourCount;

			const bool closed = (*sit).somaType == Soma3D::kContourClosed;

			// then cell body contour
			rOut.writeLine("");
			rOut.writeLine("(\"" + GetSomaName() + "\"");
			rOut.writeLine("  (Color " + GetSomaColour() + ")");
			rOut.writeLine("  (CellBody)");
			if(closed)
				rOut.writeLine("  (Closed)");

			int numSamples = (*sit).m_samples.size();
			int sampleCount = 1;
			for(vector<SamplePoint>::const_iterator s = (*sit).m_samples.begin(); s!=(*sit).m_samples.end(); ++s, ++sampleCount)
			{
				if(closed && sampleCount == numSamples)
					rOut.writeLine("  " + FormatSample(*s) + AddComment((contourCount < 10 ? " " : "") + dec(contourCount) + ", " + dec(sampleCount) + " Closure Point" ) );
				else
					rOut.writeLine("  " + FormatSample(*s) + AddComment((contourCount < 10 ? " " : "") + dec(contourCount) + ", " + dec(sampleCount) ) );
			}


			// soma markers ?


			//
			rOut.writeLine(")" + AddComment(" End of contour") );
		}

		// axons
		int count = 0;
		for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a!=nrn.AxonEnd(); ++a, ++count)
		{
			rOut.writeLine("");
			rOut.writeLine("( (Color " + GetAxonColour() + ")" + GenerateClassicTag(10,21) );
			rOut.writeLine("  (Axon)");
			OutputNeurolucidaBranch(rOut, (*a).root(), 0, String("R"));
			rOut.writeLine(")" + AddComment(" End of tree") );
		}

		//dendrites
		count = 0;
		for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d!=nrn.DendriteEnd(); ++d, ++count)
		{
			const bool apical = ((*d).GetTreeType() == Neuron3D::DendriteTree::Apical);
			rOut.writeLine("");
			rOut.writeLine("( (Color " + GetDendColour() + ")" + GenerateClassicTag(10,1) );
			rOut.writeLine(apical ? "  (Apical)" : "  (Dendrite)");
			OutputNeurolucidaBranch( rOut, (*d).root(), 0, String("R"));
			rOut.writeLine(")" + AddComment(" End of tree") );
		}

		return true;
	}
};


bool ExportNeurolucidaASC(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	NeurolucidaASCExporter exporter(options);
	return exporter.Export(rOut, nrn);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateNeurolucidaASC(Input& rIn, const ValidationOptions& options)
{
	// TODO: don't depend on comments for validation... try to import something...

	String firstLine = rIn.readLine();
	if(StringBegins(firstLine, ";"))
	{
		if( StringContains(firstLine, "V3 text file written for MicroBrightField products") ||
			StringContains(firstLine, "written by nmpml translation system") ||
			StringContains(firstLine, "written by XModeL translation system") )
		{
			return kValidationTrue;
		}
	}
	// for rose files..
	else if( StringBegins(firstLine, "( (Color "))
	{
		return kValidationTrue;
	}

	return kValidationFalse;
}
