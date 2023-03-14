//
// FileFormatNINDS1D.cpp
//

#include "ImportExportCommonIncludes.h"

#include <iostream>
#include <sstream>

using std::stringstream;
using std::istringstream;

/**

	NINDS 2D format

	@status  Incomplete due to lack of examples

**/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//

bool ImportNINDS1D(Input& rIn, Neuron3D &nrn, const HintOptions& options)	
{
	MorphologyBuilder builder(nrn);

	// assuming data is in correct order

	int dend, order, bran, seg, type;
	int x1, y1, z1, x2, y2, z2;
	int length;
	float diam;

	Neuron3D::DendriteIterator currentDend;
	Neuron3D::DendriteTreeIterator currentBran;

	int lastorder =	 0;
	int lastbran =	 1;
	int lastdend =	-1;
	int lastseg =	-1;

	String line;

	do
	{
		line = rIn.readLine();
		const char* linePtr = line.c_str();
		istringstream ss(linePtr, stringstream::in);
		ss >> dend >> order >> bran >> seg >> type >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> length >> diam;

		SamplePoint dataPointStart( ((float)x1)/100.0f, ((float)y1)/100.0f, ((float)z1)/100.0f, diam);
		SamplePoint dataPointEnd( ((float)x2)/100.0f, ((float)y2)/100.0f, ((float)z2)/100.0f, diam);

		if(dend == 0)
		{
			assert(builder.NumSomas() == 0);

			builder.NewSomaOpenContour();
			builder.CsAddSample(dataPointStart);
			builder.CsAddSample(dataPointEnd);

			// then advance to next line immediately
			lastbran = bran;
			lastdend = dend;
			lastseg = seg;
			lastorder = order;

			line = rIn.readLine();
			const char* linePtr = line.c_str();
			//sscanf(linePtr, "%d %d %d %d %d %d %d %d %d %d %d %d %f", &dend, &order, &bran, &seg, &type, x1, y1, z1, x2, y2, z2, length, diam);
			istringstream ss(linePtr, stringstream::in);
			ss >> dend >> order >> bran >> seg >> type >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> length >> diam;

			dataPointStart = SamplePoint(((float)x1)/100.0f, ((float)y1)/100.0f, ((float)z1)/100.0f, diam);
			dataPointEnd = SamplePoint(((float)x2)/100.0f, ((float)y2)/100.0f, ((float)z2)/100.0f, diam);
		}

		// new dendrite
		if(dend != lastdend)
		{
			builder.NewDendrite();
			lastorder = 0;
		}

		if((*currentBran).m_samples.size() == 0)
		{
			builder.CbAddSample(dataPointStart);
		}
		else
		{
			assert(seg == lastseg+1);
		}

		builder.CbAddSample(dataPointEnd);

		switch(type)
		{
		case 1:	// continuation
			break;
		case 2:	// branch
			if(builder.CurrentBranch())
			{
				builder.CbNewChildLast(false);
			}
			else
			{
				assert(order == 0);
			}
			break;
		case 3:	// terminal
			{
				if(builder.CbAscendToUnaryBranchPoint())
					builder.CbNewChildLast(false);
			}
			break;

		default:
			break;
		}

		lastbran = bran;
		lastdend = dend;
		lastseg = seg;
		lastorder = order;

		if(builder.HasFailed())
			return false;

	} while (rIn.remaining() > 0);

	builder.ApplyChanges();
	return true;
}

bool ImportRawNINDS1D(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportNINDS1D(*pIn, nrn, options);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

void WriteDataPointNINDS1D(Output& rOut, int dend, int order, int bran, int seg, int type, const SamplePoint& sampleA, const SamplePoint& sampleB)
{
	rOut.writeLine(	AppendSpaces(dec(dend), 8)	+
		AppendSpaces(dec(order), 8)	+
		AppendSpaces(dec(bran), 8)	+
		AppendSpaces(dec(seg), 8)	+
		AppendSpaces(dec(type), 8)	+
		AppendSpaces(dec( (int)( sampleA.x * 100.0f ) ), 8) +
		AppendSpaces(dec( (int)( sampleA.y * 100.0f ) ), 8) +
		AppendSpaces(dec( (int)( sampleA.z * 100.0f ) ), 8) +
		AppendSpaces(dec( (int)( sampleB.x * 100.0f ) ), 8) +
		AppendSpaces(dec( (int)( sampleB.y * 100.0f ) ), 8) +
		AppendSpaces(dec( (int)( sampleB.z * 100.0f ) ), 8) +
		AppendSpaces(dec( (int)( sampleA.Distance(sampleB) * 100.0f ) ), 8) +
		dec( (int) ( (sampleA.d + sampleB.d)*100.0f/2.0f) )  );
}

void ExportNINDS1DBranch(Output& rOut, int dend, int order, int pbran, Neuron3D::DendriteTreeConstIterator it)
{
	int bran = (pbran * 2) - 1;
	if( it.peer() ) // first daughter
		++bran;

	int seg = 1;
	int type = 1;

	const int samplecount = (*it).m_samples.size();

	for(int i=1;i<samplecount;++i)
	{
		if(i == samplecount-1)
		{
			type = (it.child() ? 2 : 3);
		}
		else
		{
			// ignore duplicated points
			if( (*it).m_samples[i+1].d == (*it).m_samples[i].d )
				continue;
		}

		WriteDataPointNINDS1D(rOut, dend, order, bran, seg, type, (*it).m_samples[i-1], (*it).m_samples[i]);
		seg++;
	}

	if(it.child()) ExportNINDS1DBranch(rOut, dend, order+1, bran, it.child());
	if(it.peer()) ExportNINDS1DBranch(rOut, dend, order, pbran, it.peer());
}


bool ExportNINDS1D(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	// root node
	if(nrn.SomaBegin() != nrn.SomaEnd())
		WriteDataPointNINDS1D(rOut, 0, 0, 0, 0, 0, nrn.SomaBegin()->m_samples[0], nrn.SomaBegin()->m_samples[0]);

	int i = 1;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		ExportNINDS1DBranch(rOut, i, 0, 1, (*d).root());
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateNINDS1D(Input& rIn, const ValidationOptions& options)
{
	int dend, order, bran, seg, type, x1, y1, z1, x2, y2, z2, length;
	float diam;

	const u32 maxLines = 8;
	u32 lineCount = 0;

	String line;
	do
	{
		// read line data
		line = rIn.readLine();
		const char* linePtr = line.c_str();

		istringstream ss(linePtr, stringstream::in);
		ss >> dend >> order >> bran >> seg >> type >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> length >> diam;
		//sscanf(linePtr, "%d %d %d %d %d", &dend, &order, &bran, &seg, &type/*, x1, y1, z1, x2, y2, z2, length, diam*/);

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
	while(	rIn.remaining() > 0 && lineCount < maxLines );

	return kValidationFalse;
}
