//
// FileFormatNINDS3D.cpp
//

#include "ImportExportCommonIncludes.h"

#include <iostream>
#include <sstream>

using std::istringstream;
using std::stringstream;

/**

	NINDS 3D format

	@status  Incomplete due to lack of examples

**/

enum NINDSType
{
	kContinuation	= 1,
	kBranchPoint	= 2,
	kTerminal		= 3
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

bool ImportNINDS3D(Input& rIn, Neuron3D &nrn, const HintOptions& options)	
{
	MorphologyBuilder builder(nrn);

	// assuming data is in correct order

	int dend, order, bran, seg, type;
	int x1, y1, z1, x2, y2, z2;
	int length;
	float diam;

	Neuron3D::DendriteIterator	currentDend;
	Neuron3D::DendriteTreeIterator currentBran;

	int lastorder =	 0;
	int lastbran =	 1;
	int lastdend =	-1;
	int lastseg =	-1;

	bool madeSoma = false;

	String line;


	while (rIn.remaining() > 0)
	{
		line = rIn.readLine();

		istringstream ss(line, stringstream::in);
		ss >> dend >> order >> bran >> seg >> type >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> length >> diam;

		SamplePoint dataPointStart( ((float)x1)/100.0f, ((float)y1)/100.0f, ((float)z1)/100.0f, diam);
		SamplePoint dataPointEnd( ((float)x2)/100.0f, ((float)y2)/100.0f, ((float)z2)/100.0f, diam);

		if(dend == 0)
		{
			// soma point..init soma
			builder.NewSomaOpenContour();
			builder.CsAddSample(dataPointStart);
			//builder.CsAddSample(dataPointEnd);
			madeSoma = true;

			//assert(builder.CurrentSoma().NumSamples() == 2);

			// then advance to next line immediately
			lastbran = bran;
			lastdend = dend;
			lastseg = seg;
			lastorder = order;

			if(rIn.remaining() > 0)
			{
				line = rIn.readLine();
				const char* linePtr = line.c_str();
				istringstream ss(linePtr, stringstream::in);
				ss >> dend >> order >> bran >> seg >> type >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> length >> diam;

				dataPointStart = SamplePoint(((float)x1)/100.0f, ((float)y1)/100.0f, ((float)z1)/100.0f, diam);
				dataPointEnd = SamplePoint(((float)x2)/100.0f, ((float)y2)/100.0f, ((float)z2)/100.0f, diam);
			}
			else
				break;
		}

		//if(builder.NumSomas() == 0)
		//	builder.NewSomaSinglePoint(dataPointStart);

		if(dend != lastdend)
		{
			builder.NewDendriteWithSample(dataPointStart);
			lastorder = 0;
		}

		builder.CbAddSample(dataPointEnd);

		switch(type)
		{
		case kContinuation:
			break;

		case kBranchPoint:
			if(builder.CurrentBranch())
			{
				builder.CbNewChildLast(true);
			}
			else
			{
				LogImportWarning( String("Unexpected 'order': ") + dec(type) + " (expecting: 0)");
				assert(order == 0);
			}
			break;

		case kTerminal:
			if(builder.CbAscendToUnaryBranchPoint())
			{
				builder.CbNewChildLast(true);
			}

			break;

		default:
			LogImportFailure( String("Unknown point type: ") + dec(type) );
			return false;
		}

		lastbran = bran;
		lastdend = dend;
		lastseg = seg;
		lastorder = order;

		if(builder.HasFailed())
		{
			LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
			return false;
		}
	}

	builder.ApplyChanges();
	return true;
}


bool ImportRawNINDS3D(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportNINDS3D(*pIn, nrn, options);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

static int NearestInt(float val)
{
	if(val > 0.0f)
		val += 0.5f;
	else if(val < 0.0f)
		val -= 0.5f;

	return (int)val;
	//(int)(  + 0.5f)
}

void WriteDataPointNINDS3D(Output& rOut, int dend, int order, int bran, int seg, int type, const SamplePoint& sampleA, const SamplePoint& sampleB)
{
	rOut.writeLine(	dec(dend) + "\t" +
					dec(order) + "\t" +
					dec(bran) + "\t" +
					dec(seg) + "\t" +
					dec(type) + "\t" +
					dec( NearestInt(sampleA.x * 100.0f) ) + "\t" +
					dec( NearestInt(sampleA.y * 100.0f) ) + "\t" +
					dec( NearestInt(sampleA.z * 100.0f) ) + "\t" +
					dec( NearestInt(sampleB.x * 100.0f) ) + "\t" +
					dec( NearestInt(sampleB.y * 100.0f) ) + "\t" +
					dec( NearestInt(sampleB.z * 100.0f) ) + "\t" +
					dec( NearestInt(sampleA.Distance(sampleB) * 100.0f) ) + "\t" +
					dec(  /*(sampleA.d + sampleB.d)/2.0f*/sampleB.d, 2)  );
}

void ExportNINDS3DBranch(Output& rOut, int dend, int order, int pbran, Neuron3D::DendriteTreeConstIterator it)
{
	int bran = (pbran * 2) - 1;
	if( order > 0 && !it.peer()) // second daughter of non-root branch
		++bran;

	int seg = 0;
	int type = 1;

	const int samplecount = (*it).m_samples.size();

	for(int i=1;i<samplecount;++i)
	{
		if(i == samplecount-1)
		{
			type = (it.child() ? 2 : 3);
		}
		//else
		//{
			// ignore duplicated points
			//if( (*it).m_samples[i+1].d == (*it).m_samples[i].d )
			//	continue;
		//}

		seg++;
		WriteDataPointNINDS3D(rOut, dend, order, bran, seg, type, (*it).m_samples[i-1], (*it).m_samples[i]);
	}

	if(it.child()) 
		ExportNINDS3DBranch(rOut, dend, order+1, bran, it.child());

	// note test which basically prevents export of non-binary structure
	if(it.peer() && (it.parent().child() == it)) 
		ExportNINDS3DBranch(rOut, dend, order, pbran, it.peer());
}


bool ExportNINDS3D(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	// root node
	if(nrn.SomaBegin() != nrn.SomaEnd())
		WriteDataPointNINDS3D(rOut, 0, 0, 0, 0, 0, nrn.SomaBegin()->m_samples[0], nrn.SomaBegin()->m_samples[0]);

	int i=1;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d, ++i)
	{
		ExportNINDS3DBranch(rOut, i, 0, 1, (*d).root());
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateNINDS3D(Input& rIn, const ValidationOptions& options)
{
	int dend, order, bran, seg, type, x1, y1, z1, x2, y2, z2, length;
	float diam;

	const u32 maxLines = 8;
	u32 lineCount = 0;

	String line;

	while(	rIn.remaining() > 0 && lineCount < maxLines )
	{
		// read line data
		line = rIn.readLine();

		const char* linePtr = line.c_str();

		istringstream ss(linePtr, stringstream::in);
		ss >> dend >> order >> bran >> seg >> type >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> length >> diam;

		if(lineCount == 0)
		{
			if( dend != 0 || order != 0 || bran != 0 || seg != 0 || type != 0) // is it not a soma section ?
			{
				if(order != 0 || bran != 1 || seg != 1) // is it not a dendrite root section ?
					return kValidationFalse;
			}
		}

		// check numbers are in range
		if( dend > 32000 || order > 50 || bran > 32000 || seg > 32000 || type > 3 || type < 1)
			return kValidationFalse;

		// many more potential sanity checks we could do..

		++lineCount;
	}

	return kValidationTrue;
}
