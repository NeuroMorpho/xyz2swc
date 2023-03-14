//
//
//

#include "ImportExportCommonIncludes.h"

#include <algorithm>
#include <math.h>


/** 

	Oxford format

	@status  Incomplete due to lack of examples

**/

enum OxfordPointType
{
	kStart				= 3,
	kContinue			= 4,
	kFibreSwelling		= 5,
	kSpineBase			= 6,
	kBranchSplit		= 7,
	kBranchEnd			= 8
};

static const u32 gs_oxfordLineLength = 35;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

bool ImportOxford(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	MorphologyBuilder builder(nrn);

	const u32 numLines = std::stoi( rIn.readLine() );
	u32 countLines = 0;

	String dataLine;

	SamplePoint backupSomaPoint;
	bool		foundSoma = false;

	Neuron3D::DendriteIterator			currentDend;
	Neuron3D::DendriteTreeIterator		currentBran;

	while(rIn.remaining() > 0 && countLines < numLines)
	{
		dataLine = rIn.readLine();

		if(dataLine.length() < gs_oxfordLineLength)
		{
			LogImportFailure( String("File format error - line is too short: ") + dec((u32)dataLine.length()) + " (expecting: " + dec(gs_oxfordLineLength) + ")" );
			return false;
		}

		const char* linePtr = dataLine.c_str();
		const float x = 				std::stof(String(linePtr, 8))	/** 1000.0f*/;
		const float y = 				std::stof(String(linePtr + 9, 8))	/** 1000.0f*/;
		const float z = 				std::stof(String(linePtr + 18, 8))	/** 1000.0f*/;
		const OxfordPointType type =	(OxfordPointType) std::stoi( String(linePtr + 27, 3));
		const float d =					std::stof(String(linePtr + 31, 4))	/** 1000.0f*/;

		SamplePoint dataPoint(x, y, z, d);

		if(countLines == 0)
			backupSomaPoint = dataPoint;

		++countLines;

		switch(type)
		{
			case kStart:
				builder.NewDendriteWithSample(dataPoint);
				break;
			
			case kContinue:
				builder.CbAddSample(dataPoint);
				break;
			
			case kFibreSwelling:
				builder.NewSomaSinglePoint(dataPoint);
				foundSoma = true;
				break;
			
			case kSpineBase:
				break;

			case kBranchSplit:
				builder.CbAddSample(dataPoint);
				builder.CbNewChildLast(true);
				break;

			case kBranchEnd:
				builder.CbAddSample(dataPoint);
				if(builder.CbAscendToUnaryBranchPoint())
					builder.CbNewChildLast(true);
				break;

			default:
				LogImportFailure( String("Unknown point type: ") + dec(type) );
				return false;

		}

		if(builder.HasFailed())
		{
			LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
			return false;
		}
	}

	//if(!foundSoma)
	//	builder.CsAddSample(backupSomaPoint);

	builder.ApplyChanges();
	return true;
}


bool ImportRawOxford(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportOxford(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//


static int CountBranchPoints(Neuron3D::DendriteTreeConstIterator it, u32 count)
{
	count += (*it).m_samples.size() - 1;

	if(it.child()) 
		count = CountBranchPoints(it.child(), count);

	// note test which basically prevents export of non-binary structure
	if(it.peer() && (it.parent().child() == it)) 
		count = CountBranchPoints(it.peer(), count);

	return count;
}

static int CountNeuronPoints(const Neuron3D &nrn)
{
	u32 count = std::min(nrn.CountSomas(), (u32)1);	// for soma

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		count = CountBranchPoints( (*d).root(), count + 1);
	}

	/*for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		count = CountBranchPoints( (*a).root(), count + 1);
	}*/

	return count;
}


static char gs_zeroes[] = "0000000";

// TODO - more checks here for numbers which are too large!
String FloatToStringWithFrontPaddedZeros(float val, u32 precision, u32 totalLength)
{
	val = std::min(val, 99999.9f);

	String result;

	do
	{
		result = dec(val, precision);
	}
	while(result.length() > totalLength && precision-- > 0);

	u32 offset = StringBegins(result, "-") ? 1 : 0;
	result.insert(offset, String(gs_zeroes, totalLength - result.length()));

	return result;
}

void WriteOxfordDataPoint(Output& rOut, OxfordPointType type, const SamplePoint& sample)
{
	rOut.writeLine( FloatToStringWithFrontPaddedZeros(sample.x, 2, 8) + "," + 
					FloatToStringWithFrontPaddedZeros(sample.y, 2, 8) + "," + 
					FloatToStringWithFrontPaddedZeros(sample.z, 2, 8) + "," + 
					"00" + dec((u32)type) + "," + 
					/*FloatToStringWithFrontPaddedZeros(sample.d, 2, 4)*/
					dec(sample.d, 2) );
}

int ExportOxfordSoma(Output& rOut, const Neuron3D &nrn, int segment)
{
	if(nrn.CountSomas() > 0)
	{
		++segment;
		WriteOxfordDataPoint(rOut, kFibreSwelling, (*nrn.SomaBegin()).m_samples[0]);
	}
	return segment;
}

int ExportOxfordBranch(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int segment)
{
	OxfordPointType type;

	for(u32 i=1; i<(*it).m_samples.size();++i)
	{
		type = kContinue;

		if(i == (*it).m_samples.size() - 1)
		{
			if(it.child())
			{
				if(it.child().peer())
					type = kBranchSplit;
			}
			else
				type = kBranchEnd;
		}

		segment++;
		WriteOxfordDataPoint(rOut, type, (*it).m_samples[i]);
	}

	if(it.child()) 
		segment = ExportOxfordBranch(rOut, it.child(), segment);

	// note test which basically prevents export of non-binary structure
	if(it.peer() && (it.parent().child() == it)) 
		segment = ExportOxfordBranch(rOut, it.peer(), segment);

	return segment;
}

bool ExportOxford(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	u32 totalPoints = CountNeuronPoints(nrn);

	rOut.writeLine(dec(totalPoints));

	int segmentcount = 0;

	segmentcount = ExportOxfordSoma(rOut, nrn, segmentcount);

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		++segmentcount;
		WriteOxfordDataPoint(rOut, kStart, (*(*d).root()).m_samples[0]);

		segmentcount = ExportOxfordBranch(rOut, (*d).root(), segmentcount);
	}

	assert(segmentcount == totalPoints);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateOxford(Input& rIn, const ValidationOptions& options)
{
	u32 maxLines = 8;
	u32 lineCount = 0;

	rIn.readLine(); // skip the integer count..

	String line;

	while(lineCount < maxLines && rIn.remaining() > 0)
	{
		line = rIn.readLine();
		if(	line.length() < gs_oxfordLineLength || 
			line[8]  != ',' || 
			line[17] != ',' || 
			line[26] != ',' || 
			line[30] != ',' )
		{
			return kValidationFalse;
		}
		++lineCount;
	}

	return kValidationTrue;
}
