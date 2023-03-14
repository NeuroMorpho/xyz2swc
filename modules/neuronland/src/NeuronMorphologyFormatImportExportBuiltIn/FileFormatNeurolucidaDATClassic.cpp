//
// FileFormatNeurolucidaDATClassic.cpp
//

#include "ImportExportCommonIncludes.h"

#include <float.h>
#include <math.h>
#include <algorithm>

#include "NeurolucidaMarkers.h"
#include "NeurolucidaColours.h"
#include "NeurolucidaTerminals.h"

#include "NeurolucidaClassicImportExport.h"


/** 

	NeurolucidaDATClassic file format. 

**/

namespace
{
	const size_t headerSize = 18;

	const u8 datHeader[headerSize] = 
	{ 
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00
	};

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


	bool ReadHeaderStart(Input& rIn)
	{
		char header[headerSize];
		rIn.read(header, headerSize);

		return (strncmp(header, (const char*)datHeader, headerSize) == 0);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import 
//


template<typename T>
T ReadValue(Input& rIn)
{
	T value;

	rIn.read(value);

	eswap(value);

	return value;
}


class NeurolucidaDATClassicImporter : public NeurolucidaClassicImporter
{
private:

	virtual bool ReadHeader(Input& rIn, NCHeaderDetails& details)
	{
		if(!ReadHeaderStart(rIn))
			return false;

		details.min[0] = ReadValue<float>(rIn);
		details.min[1] = ReadValue<float>(rIn);
		details.min[2] = ReadValue<float>(rIn);
		details.max[0] = ReadValue<float>(rIn);
		details.max[1] = ReadValue<float>(rIn);
		details.max[2] = ReadValue<float>(rIn);

		u32 numSamples = ReadValue<u32>(rIn);

		(void)ReadValue<float>(rIn);
		(void)ReadValue<float>(rIn);
		(void)ReadValue<float>(rIn);
		(void)ReadValue<float>(rIn);
		(void)ReadValue<float>(rIn);
		(void)ReadValue<float>(rIn);

		return true;
	}

	virtual bool ReadSample(Input& rIn, u32& major, u32& minor, SamplePoint& sample)
	{
		major = ReadValue<u8>(rIn);
		minor = ReadValue<u8>(rIn);
		sample.x = ReadValue<float>(rIn);
		sample.y = ReadValue<float>(rIn);
		sample.z = ReadValue<float>(rIn);
		sample.d = ReadValue<float>(rIn);

		return true;
	}
};


bool ImportNeurolucidaDATClassic(Input& rIn, Neuron3D& nrn, const HintOptions& options)
{
	NeurolucidaDATClassicImporter importer;
	return importer.Import(rIn, nrn, options);
}


bool ImportRawNeurolucidaDATClassic(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportNeurolucidaDATClassic(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

template<typename T>
void WriteValue(Output& rOut, T val)
{
	eswap(val);
	rOut.write(val);
}

class NeurolucidaDATClassicExporter : public NeurolucidaClassicExporter
{
public:
	virtual void WriteHeader(Output& rOut, const NCHeaderDetails& details)
	{
		rOut.write(datHeader, headerSize);
		WriteValue<float>(rOut, details.min[0]);
		WriteValue<float>(rOut, details.min[1]);
		WriteValue<float>(rOut, details.min[2]);
		WriteValue<float>(rOut, details.max[0]);
		WriteValue<float>(rOut, details.max[1]);
		WriteValue<float>(rOut, details.max[2]);

		// TODO (sample count ?)
		WriteValue<float>(rOut, 0.0f);

		// 24 0s
		WriteValue<float>(rOut, 0.0f);
		WriteValue<float>(rOut, 0.0f);
		WriteValue<float>(rOut, 0.0f);
		WriteValue<float>(rOut, 0.0f);
		WriteValue<float>(rOut, 0.0f);
		WriteValue<float>(rOut, 0.0f);
	}

	virtual void WriteSample(Output& rOut, u32 major, u32 minor, const SamplePoint& sample)
	{
		WriteValue<u8>(rOut, (u8)major);
		WriteValue<u8>(rOut, (u8)minor);
		WriteValue<float>(rOut, sample.x);
		WriteValue<float>(rOut, sample.y);
		WriteValue<float>(rOut, sample.z);
		WriteValue<float>(rOut, sample.d);
	}
};


bool ExportNeurolucidaDATClassic(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	NeurolucidaDATClassicExporter exporter;
	return exporter.Export(rOut, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateNeurolucidaDATClassic(Input& rIn, const ValidationOptions& options)
{
	return ReadHeaderStart(rIn) ? kValidationTrue : kValidationFalse;
}
