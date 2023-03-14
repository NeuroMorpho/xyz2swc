//
//
//

#include "NeurolucidaClassicImportExport.h"

#include "ImportExportCommonIncludes.h"

#include "NeurolucidaMarkers.h"

#include <float.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

static bool ExtractSamplePoint(const String& line, u32& major, u32& minor, SamplePoint& sample)
{
	if(! StringBegins(line, "["))
		return false;

	major = std::stoi( StringBefore( StringAfter(line, "["), ",") );
	minor = std::stoi( StringBefore( StringAfter(line, ","), "]") );

	String point3D = StringBefore( StringAfter(line, "("), ")");
	Strings coords = StringSplit(point3D, ',');

	sample.x = std::stof( coords[0] );
	sample.y = std::stof( coords[1] );
	sample.z = std::stof( coords[2] );

	sample.d = std::stof( StringAfter(line, ")") );

	return true;
}


class NeurolucidaASCClassicImporter : public NeurolucidaClassicImporter
{
private:

	virtual bool ReadHeader(Input& rIn, NCHeaderDetails& details)
	{
		// do nothing.... let the sample reader do the header skipping..
		return true;
	}

	virtual bool ReadSample(Input& rIn, u32& major, u32& minor, SamplePoint& sample)
	{
		String line;

		while(rIn.remaining())
		{
			line = rIn.readLine();

			// skip header and spaces
			if(! StringBegins(line, "["))
				continue;
			else
			{
				if(ExtractSamplePoint(line, major, minor, sample))
				{
					return true;
				}
				else
				{
					LogImportFailure("Malformed sample!");
					return false;
				}
			}
		}

		return true;
	}
};

bool ImportNeurolucidaASCClassic(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	NeurolucidaASCClassicImporter importer;
	return importer.Import(rIn, nrn, options);
}


bool ImportRawNeurolucidaASCClassic(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportNeurolucidaASCClassic(*pIn, nrn, options);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

class NeurolucidaASCClassicExporter : public NeurolucidaClassicExporter
{
public:
	virtual void WriteHeader(Output& rOut, const NCHeaderDetails& details)
	{
		rOut.writeLine("version 3"); 
		rOut.writeLine("file id: " + details.fileName);
		rOut.writeLine("MAX:(" + dec(details.max[0],2) + ", " + dec(details.max[1],2) + ", " + dec(details.max[2],2) + ")");
		rOut.writeLine("MIN:(" + dec(details.min[0],2) + ", " + dec(details.min[1],2) + ", " + dec(details.min[2],2) + ")");
		rOut.writeLine("filetype: " + dec(details.fileType));
		rOut.writeLine("id1: " + dec(details.id1));
		rOut.writeLine("id2: " + dec(details.id2));
		rOut.writeLine("REF:(" + dec(details.ref[0],2) + ", " + dec(details.ref[1],2) + ", " + dec(details.ref[2],2) + ")");
		rOut.writeLine("rotation: " + dec(details.rotation,2));
		rOut.writeLine("gx_alpha: " + dec(details.gx_alpha));
		rOut.writeLine("gx_beta: " + dec(details.gx_beta));
		rOut.writeLine("gx_gamma: " + dec(details.gx_gamma));
		rOut.writeLine("");
		rOut.writeLine("");
		rOut.writeLine("");
	}

	virtual void WriteSample(Output& rOut, u32 major, u32 minor, const SamplePoint& sample)
	{
		rOut.writeLine(	"[" + dec(major) + "," + dec(minor) + "]  " +
						"(" + dec(sample.x, 2) + ", "
							+ dec(sample.y, 2) + ", "
							+ dec(sample.z, 2) + ")  "
							+ dec(sample.d, 2) );
	}
};

bool ExportNeurolucidaASCClassic(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	NeurolucidaASCClassicExporter exporter;

	return exporter.Export(rOut, nrn, options);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateNeurolucidaASCClassic(Input& rIn, const ValidationOptions& options)
{
	String firstLine = rIn.readLine();
	String secondLine = rIn.readLine();
	String thirdLine = rIn.readLine();
	String fourthLine = rIn.readLine();

	if( StringBegins(firstLine, "version ") && 
		StringBegins(secondLine, "file id:") &&
		StringBegins(thirdLine, "MAX:") &&
		StringBegins(fourthLine, "MIN:") )
	{
		return kValidationTrue;
	}

	return kValidationFalse;
}
