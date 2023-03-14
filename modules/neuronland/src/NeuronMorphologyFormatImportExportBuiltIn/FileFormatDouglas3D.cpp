//
//
//

#include "ImportExportCommonIncludes.h"

#include <algorithm>
#include <math.h>

#include "Core/Parser.h"


/** 

	Douglas3D format

	Structure supported ->
		Soma		: MULTIPLE OPEN CONTOUR
		Dendrite	: BINARY TREE
		Axon		: NO
		Marker		: NO
		Spine		: NO

	Pending:
	* Need more examples.

**/

static char gs_Douglas3DHeader[] = "3d";

enum Douglas3DPointType
{
	MTO,	DCP,	BP,		NE,
	MAE,	TAE,	BAE,
	SCP,

	kPointTypeNumber,
	kPointTypeError
};

const char* gs_Douglas3DPointNames[] = 
{
	"mto",	"dcp",	"bp ",	"ne ",
	"mae",	"tae",	"bae",
	"scp"
};

static Douglas3DPointType PointNameToType(const String& name)
{
	for(u32 n=0;n<kPointTypeNumber;++n)
		if(name == gs_Douglas3DPointNames[n])
			return (Douglas3DPointType)n;

	return kPointTypeError;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

class Douglas3DParser : public SimpleParser
{
public:
	Douglas3DParser(Input& rIn, Neuron3D &nrn) : SimpleParser(rIn), m_nrn(nrn)
	{
		lastPointWasMTO = false;
		doingSoma = false;
	}

	bool Parse()
	{
		MorphologyBuilder builder(m_nrn);

		while(rIn.remaining() /*&& !builder.HasFailed()*/)
		{
			String line = rIn.readLine();

			if(line == "" || line == String(gs_Douglas3DHeader))
				continue;

			const char* linePtr = line.c_str();

			const Douglas3DPointType pointType =	PointNameToType( String(linePtr, 3) );
			const float x	=						std::stof( String(linePtr + 3, 13))	* 0.1f;
			const float y	=						std::stof( String(linePtr + 16, 11))	* 0.1f;
			const float z	=						std::stof( String(linePtr + 27, 11))	* 0.1f;
			const float d	=						std::stof( String(linePtr + 38, 11))	* 0.1f;

			SamplePoint dataPoint(x, y, z, d);

			// special case - can't decide how to treat the MTO until we get to the subsequent data point
			if (lastPointWasMTO)
			{
				lastPointWasMTO = false;

				if(pointType != MTO && pointType != SCP)
				{
					builder.NewDendriteWithSample(prevDataPoint);
				}
				else
				{
					builder.NewSomaOpenContour();
					builder.CsAddSample(prevDataPoint);
					doingSoma = true;
				}
			}
			//

			switch(pointType)
			{
				case MTO:
					lastPointWasMTO = true;
					break;

				case BP:
					builder.CbAddSample(dataPoint);
					builder.CbNewChildLast(true);
					break;

				case DCP:
					builder.CbAddSample(dataPoint);
					break;

				case NE:
				case TAE:
				case BAE:
					builder.CbAddSample(dataPoint);
					if(builder.CbAscendToUnaryBranchPoint())
						builder.CbNewChildLast(true);
					break;

				case SCP:
					builder.CsAddSample(dataPoint);
					break;

				case MAE:
					if(doingSoma)
					{
						doingSoma = false;
						builder.CsAddSample(dataPoint);
					}
					break;
				default:
					LogImportFailure( String("File format error - unknown point type: ") + std::to_string((int)pointType));
					return false;
			}

			prevDataPoint = dataPoint;

			if(builder.HasFailed())
			{
				LogImportFailure(String("Morphology Builder failure: ") + builder.GetError());
				return false;
			}
		}

		builder.ApplyChanges();
		return true;
	}

private:
	Neuron3D&		m_nrn;
	bool			lastPointWasMTO;
	bool			doingSoma;
	SamplePoint 	prevDataPoint;

};

bool ImportDouglas3D(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	Douglas3DParser parser(rIn, nrn);
	return parser.Parse();
}


bool ImportRawDouglas3D(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportDouglas3D(*pIn, nrn, options);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

void WriteDouglas3DDataPoint(Output& rOut, u32 segmentcount, Douglas3DPointType type, const SamplePoint& sample)
{
	rOut.writeLine(	String(gs_Douglas3DPointNames[type])	+
					PrependSpaces(dec(sample.x * 10.0f, 2), 13)		+
					PrependSpaces(dec(sample.y * 10.0f, 2), 11)		+
					PrependSpaces(dec(sample.z * 10.0f, 2), 11)		+
					PrependSpaces(dec(sample.d * 10.0f, 2), 11)		);
}

int ExportDouglas3DSoma(Output& rOut, const Neuron3D &nrn, int segment)
{
	for(Neuron3D::SomaConstIterator s = nrn.SomaBegin(); s!=nrn.SomaEnd(); ++s)
	{
		const u32 numPoints = u32( s->m_samples.size() );

		for(u32 i=0;i<numPoints;++i)
		{
			Douglas3DPointType type = SCP;
			if(i == 0)
				type = MTO;
			else if (i == numPoints-1)
				type = MAE;

			++segment;
			WriteDouglas3DDataPoint(rOut, segment, type, s->m_samples[i]);

			// extra sample to ensure identification as soma.
			if(numPoints == 2 && i==0)
			{
				++segment;
				WriteDouglas3DDataPoint(rOut, segment, SCP, s->m_samples[0]);
			}
		}
	}
	return segment;
}

int ExportDouglas3DBranch(Output& rOut, Neuron3D::DendriteTree::const_iterator it, int segment)
{
	for(u32 i=1; i<(*it).m_samples.size();++i)
	{
		Douglas3DPointType type = DCP;

		if(i == (*it).m_samples.size() - 1)
		{
			if(it.child())
			{
				if(it.child().peer())
					type = BP;
			}
			else
				type = NE;
		}

		++segment;
		WriteDouglas3DDataPoint(rOut, segment, type, (*it).m_samples[i]);
	}

	if(it.child()) 
		segment = ExportDouglas3DBranch(rOut, it.child(), segment);

	// note test which basically prevents export of non-binary structure
	if(it.peer() && (it.parent().child() == it) ) 
		segment = ExportDouglas3DBranch(rOut, it.peer(), segment);

	return segment;
}

bool ExportDouglas3D(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	rOut.writeLine( String(gs_Douglas3DHeader) );

	int segmentcount = ExportDouglas3DSoma(rOut, nrn, 0);

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		++segmentcount;
		WriteDouglas3DDataPoint(rOut, segmentcount, MTO, (* (*d).root()).m_samples[0]);

		segmentcount = ExportDouglas3DBranch(rOut, (*d).root(), segmentcount);
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validation
//

ValidationResult ValidateDouglas3D(Input& rIn, const ValidationOptions& options)
{
	u32 linecount = 0;
	while(linecount < 100 && rIn.remaining())
	{
		String line = rIn.readLine();

		if (StringBegins(line, gs_Douglas3DHeader))
		{
			// should check a few lines just to be sure...

			return kValidationTrue;
		}

		++linecount;
	}

	return kValidationFalse;
}
