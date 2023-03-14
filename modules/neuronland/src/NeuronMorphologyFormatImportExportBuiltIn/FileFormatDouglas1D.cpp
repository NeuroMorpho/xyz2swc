//
//
//

#include "ImportExportCommonIncludes.h"

#include <algorithm>
#include <math.h>

#include "Core/Parser.h"


/** 

	Douglas1D format

	@status  Imcomplete due to lack of examples..... unfinished implementation !!

**/

static char gs_Douglas1DHeader[] = "branchID br.ord. seglth segdiam term";
static char	gs_Douglas1DFooter[] = "end 1 1 1 1 ";

enum Douglas1DPointType
{
	SOM,	BP,		NE,
	MAE,	TAE,	BAE,

	kPointTypeNumber,
	kPointTypeError
};

const char* gs_Douglas1DPointNames[] = 
{
	"som",	"bp ",	"ne ",
	"mae",	"tae",	"bae"
};

static Douglas1DPointType PointNameToType(const String& name)
{
	for(u32 n=0;n<kPointTypeNumber;++n)
		if(name == gs_Douglas1DPointNames[n])
			return (Douglas1DPointType)n;

	return kPointTypeError;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

class Douglas1DParser : public SimpleParser
{
public:
	Douglas1DParser(Input& rIn, Neuron3D &nrn) : SimpleParser(rIn), m_nrn(nrn)
	{
		waitingForRoot = true;
	}

	bool Parse()
	{
		MorphologyBuilder builder(m_nrn);
		String comment;
		String item;

		// skip comment block (if there is one)
		SeekNextItem();

		// read them all in and handle appropriately
		while(rIn.remaining() > 0)
		{
			String line = rIn.readLine();
			const char* linePtr = line.c_str();

			const Douglas1DPointType pointType =	PointNameToType( String(linePtr + 50, 3) );
			const float l	=						std::stof( String(linePtr + 31, 9) )	* 0.1f;
			const float d	=						std::stof( String(linePtr + 40, 9) )	* 0.1f;

			SamplePoint dataPoint(l, 0.0f, 0.0f, d);

			switch( pointType ) 
			{
			case SOM:
				builder.NewSomaSinglePoint(dataPoint);
				break;

			case BP:
				if(waitingForRoot)
				{
					waitingForRoot = false;
					builder.NewDendriteWithSample( builder.CurrentSoma().GetLastSample() );
				}

				builder.CbAddSample(dataPoint);
				builder.CbNewChildLast(true);
				break;

			case NE:
			case MAE:
			case TAE:
			case BAE:
				if(waitingForRoot)
				{
					waitingForRoot = false;
					builder.NewDendriteWithSample( builder.CurrentSoma().GetLastSample() );
				}

				builder.CbAddSample(dataPoint);
				if(builder.CbAscendToUnaryBranchPoint())
					builder.CbNewChildLast(true);

				break;
			}

			SeekNextItem();

			if(builder.HasFailed())
				return false;
		}

		builder.ApplyChanges();
		return true;
	}

private:
	Neuron3D& m_nrn;
	bool waitingForRoot;
};

bool ImportDouglas1D(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	Douglas1DParser parser(rIn, nrn);
	return parser.Parse();
}

bool ImportRawDouglas1D(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportDouglas1D(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

void WriteDouglas1DDataPoint(Output& rOut, u32 segmentcount, Douglas1DPointType type, float length, float diameter)
{
	rOut.writeLine(	PrependSpaces(dec(length, 2), 40)		+
					PrependSpaces(dec(diameter, 2), 8)		+ " " +
					String(gs_Douglas1DPointNames[type])	);
}

int ExportDouglas1DSoma(Output& rOut, const Neuron3D &nrn, int segment)
{
	const u32 numPoints = u32( nrn.SomaBegin()->m_samples.size() );

	for(u32 i=0;i<numPoints;++i)
	{
		++segment;
		WriteDouglas1DDataPoint(rOut, segment, SOM, nrn.SomaBegin()->m_samples[i].x, nrn.SomaBegin()->m_samples[i].d);
	}
	return segment;
}

int ExportDouglas1DBranch(Output& rOut, Neuron3D::DendriteTree::const_iterator it, int segment)
{
	float length = 0.0f;
	float diameter = 0.0f;

	do
	{
		for(u32 i=1; i<(*it).m_samples.size();++i)
		{
		}
	}
	while (it.child() && !it.child().peer());

	Douglas1DPointType type;

	type = it.child() ? BP : NE;

	++segment;
	WriteDouglas1DDataPoint(rOut, segment, type, length, diameter);

	if(it.child()) segment = ExportDouglas1DBranch(rOut, it.child(), segment);
	if(it.peer()) segment = ExportDouglas1DBranch(rOut, it.peer(), segment);

	return segment;
}

bool ExportDouglas1D(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	rOut.writeLine( String(gs_Douglas1DHeader) );

	int segmentcount = 0;

	segmentcount = ExportDouglas1DSoma(rOut, nrn, segmentcount);

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		segmentcount = ExportDouglas1DBranch(rOut, (*d).root(), segmentcount);
	}

	rOut.writeLine( String(gs_Douglas1DFooter) );

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validation
//

ValidationResult ValidateDouglas1D(Input& rIn, const ValidationOptions& options)
{
	u32 linecount = 0;
	while(linecount < 100 && rIn.remaining())
	{
		String line = rIn.readLine();

		if (StringBegins(line, gs_Douglas1DHeader))
		{
			// should check a few lines just to be sure...

			return kValidationTrue;
		}

		++linecount;
	}

	return kValidationFalse;
}
