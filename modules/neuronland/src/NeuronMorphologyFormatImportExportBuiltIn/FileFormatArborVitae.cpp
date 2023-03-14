//
// FileFormatArborVitae.cpp
//

#include "ImportExportCommonIncludes.h"

#include <sstream>

#include "Core/Parser.h"


/**

	ArborVitae format

	Structure supported ->
		Soma		: POINT
		Dendrite	: BINARY TREE
		Axon		: NO
		Marker		: NO
		Spine		: NO

	Pending:
	* Confirm soma output! Should we export all points? Example files seem to only have one. Not surprising since the data is artificial.
	* validation of data values.


	@note  Haven't seen many examples, probably some cases unaccounted for.

**/


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

enum ArborVitaeImportError
{
	kErrorMalformedSample,
};

class ArborVitaeParser : public SimpleParser
{
public:
	ArborVitaeParser(Input& rIn, Neuron3D &nrn) : SimpleParser(rIn), builder(nrn)
	{
		SetIgnoreAfter('#');
	}

	bool Parse()
	{
		String comment;
		String item;

		// skip comment block (if there is one)
		SeekNextItem();

		while(rIn.remaining() > 0)
		{
			u32 			pointNumber;
			u32 			someNumber;
			char			pointType;
			SamplePoint 	dataPoint;

			String			line = rIn.readLine();
			istringstream	ss(line, stringstream::in);

			ss	>>	pointNumber
				>>	pointType
				>>	someNumber
				>>	dataPoint.x
				>>	dataPoint.y
				>>	dataPoint.z
				>>	dataPoint.d;

			switch(pointType)
			{
			case 'S':
				builder.NewSomaSinglePoint(dataPoint);
				// deliberate fall-through to 'P'

			case 'P':
				if(builder.NumSomas() == 0)
				{
					LogImportFailure("File format error - Encountered 'P' point type, before 'S'");
					return false;
				}

				builder.NewDendriteWithSample(dataPoint);
				break;

			case 'C':
				builder.CbAddSample(dataPoint);
				break;

			case 'B':
				builder.CbAddSample(dataPoint);
				builder.CbNewChildLast(true);
				break;

			case 'T':
				builder.CbAddSample(dataPoint);

				// assumes no unfulfilled branch after last terminal point...
				if(builder.CbAscendToUnaryBranchPoint())
					builder.CbNewChildLast(true);
				break;

			default:
				LogImportFailure( String("File format error - unknown point type: ") + std::to_string( (u32)pointType) );
				return false;
			}

			SeekNextItem();

			if(builder.HasFailed())
			{
				LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
				return false;
			}
		}

		// successfully constructed entire neuron
		builder.ApplyChanges();
		return true;
	}

private:
	MorphologyBuilder builder;
};

bool ImportArborVitae(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	ArborVitaeParser parser(rIn, nrn);
	return parser.Parse();
}

bool ImportRawArborVitae(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportArborVitae(*pIn, nrn, options);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

void WriteArborVitaeDataPoint(Output& rOut, u32 segmentcount, String id, u32 type, const SamplePoint& sample)
{
	rOut.writeLine( PrependSpaces(dec(segmentcount), 5) + 
					"  " + id + 
					PrependSpaces(dec(type), 5) + 
					PrependSpaces(dec(sample.x, 4), 12) +
					PrependSpaces(dec(sample.y, 4), 12) +
					PrependSpaces(dec(sample.z, 4), 12) +
					PrependSpaces(dec(sample.d, 4), 12) );
}

int ExportArborVitaeBranch(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int segment)
{
	String id;

	for(u32 i=1; i<(*it).m_samples.size();++i)
	{
		id = "C";

		if(i == (*it).m_samples.size() - 1)
		{
			if(it.child())
			{
				if(it.child().peer())
					id = "B";
			}
			else
				id = "T";
		}

		++segment;
		WriteArborVitaeDataPoint(rOut, segment, id, 12, (*it).m_samples[i]);
	}

	if(it.child()) 
		segment = ExportArborVitaeBranch(rOut, it.child(), segment);

	// note test which basically prevents export of non-binary structure
	if(it.peer() && (it.parent().child() == it) )  
		segment = ExportArborVitaeBranch(rOut, it.peer(), segment);


	return segment;
}

static void WriteHeader(Output& rOut)
{
	rOut.writeLine("");
	rOut.writeLine("# GROUP    1: '  ?????????????????  '");
	rOut.writeLine("# Start of CELL [  1,    1]");
}

bool ExportArborVitae(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	WriteHeader(rOut);

	int segment = 0;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		const String tag = (d == nrn.DendriteBegin() ? String("S") : String("P") );

		++segment;
		WriteArborVitaeDataPoint(rOut, segment, tag, 9, (*((*d).root())).m_samples[0] );

		segment = ExportArborVitaeBranch(rOut, (*d).root(), segment);
	}

	WriteHeader(rOut);

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

static bool IsPointTypeCode(char type)
{
	return	type == 'S' ||
			type == 'C' ||
			type == 'B' ||
			type == 'T' ||
			type == 'P';
}

ValidationResult ValidateArborVitae(Input& rIn, const ValidationOptions& options)
{
	u32 linesToRead = 8;
	bool foundAPointTypeCode;
	String line;
	
	while(linesToRead && rIn.remaining() > 0)
	{
		line = rIn.readLine();
		--linesToRead;

		if( StringBegins(line, "# GROUP") || StringBegins(line, "# Start of CELL") || line == "")
			continue;

		if(line.length() > 7 && IsPointTypeCode( line[7] ) )
		{
			foundAPointTypeCode = true;
			continue;
		}

		return kValidationFalse;
	}

	if(foundAPointTypeCode)
		return kValidationTrue;
	else
		return kValidationFalse;
}
