//
//
//

#include "ImportExportCommonIncludes.h"

#include <vector>
#include <algorithm>

//#include "HOCParser.h"



//
extern bool ImportNeuronNTSCable(Input& rIn, Neuron3D &nrn, const HintOptions& options);
extern bool ExportNeuronNTSCable(Output& rOut, const Neuron3D &nrn, const StyleOptions& options);
extern ValidationResult ValidateNeuronNTSCable(Input& rIn, const ValidationOptions& options);
//

//
extern bool ImportNeuronSWC2HOC(Input& rIn, Neuron3D &nrn, const HintOptions& options);
extern bool ExportNeuronSWC2HOC(Output& rOut, const Neuron3D &nrn, const StyleOptions& options);
extern ValidationResult ValidateNeuronSWC2HOC(Input& rIn, const ValidationOptions& options);
//


/** 

	NeuronHOC generates a file suitable for inclusion in a Neuron Simulator script

	Here we provide functions for loading/saving this style of script.


	SWC2HOC.awk 
	CNIC variation of SWC2HOC
	NTSCable
	geometry_to_neuron.pl
	Oz_neuron.pl

	SLSR style

	other styles...



	Import uses boost::spirit.

**/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//


bool ImportNeuronHOC(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	// NTSCable special case
	ValidationOptions vOptions;
	if(ValidateNeuronNTSCable(rIn, vOptions))
	{
		rIn.seek(0, IoBase::Absolute);
		return ImportNeuronNTSCable(rIn, nrn, options);
	}
	//

	rIn.seek(0, IoBase::Absolute);
	// SWC2HOC special case
	if(ValidateNeuronSWC2HOC(rIn, vOptions))
	{
		rIn.seek(0, IoBase::Absolute);
		return ImportNeuronSWC2HOC(rIn, nrn, options);
	}
	//

	return false;

/*
	// put file in memory buffer
	u32 fileSize = rIn.seek(0, IO::FromEnd);
	rIn.seek(0, IO::Absolute);
	char* mem = new char[fileSize+1];
	rIn.read(mem, fileSize);
	mem[fileSize] = 0;
	//

	String fileAsString(mem, fileSize);

	// Boost::Spirit - based partial-HOC parser for remaining styles..
	using ascii::space;

	hoc_parser<String::const_iterator> parser;

	String::const_iterator iter = fileAsString.begin();
	String::const_iterator end = fileAsString.end();
	bool r = phrase_parse(iter, end, parser, space);


	delete [] mem;

	if (r && iter == end)
	{
		std::cout << "-------------------------\n";
		std::cout << "Parsing succeeded\n";
		std::cout << "-------------------------\n";
		return true;
	}
	else
	{
		std::cout << "-------------------------\n";
		std::cout << "Parsing failed\n";
		std::cout << "-------------------------\n";
		return false;
	}*/
}


bool ImportRawNeuronHOC(const char* file, Neuron3D &nrn, const HintOptions& options)
{
	Input* pIn = InputFile::open(file);
	return ImportNeuronHOC(*pIn, nrn, options);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

// 
// Style: SLSR
//
static void SLSRWriteSection(Output& rOut, const String& name, const String& parent, const std::vector<SamplePoint>& samples, u32 precision)
{
	rOut.writeLine("");
	rOut.writeLine("{create " + name + "}");

	if(parent != "")
		rOut.writeLine("{" + parent + " connect " + name + "(0), 1.000000}");

	rOut.writeLine("{access " + name + "}");
	rOut.writeLine("{nseg = " + dec( std::max<int>(1,samples.size()/2)) + "}"); // arbitrary choice..
	rOut.writeLine("{pt3dclear()}");

	for(u32 n=0;n<samples.size();++n)
	{
		rOut.writeLine( "{pt3dadd(" +	dec(samples[n].x,precision) + ", " + 
										dec(samples[n].y,precision) + ", " + 
										dec(samples[n].z,precision) + ", " + 
										dec(samples[n].d,precision) + ")}"  );
	}
}

static void SLSRWriteBranchSamples(Output& rOut, Neuron3D::DendriteTreeConstIterator it, String rootName, String parent, int currentPeerId)
{
	String name = rootName + dec(currentPeerId);

	SLSRWriteSection(rOut, name, parent, (*it).m_samples, 2);

	if(it.child()) 
		SLSRWriteBranchSamples(rOut, it.child(), name, name, 1);

	if(it.peer())
	{
		SLSRWriteBranchSamples(rOut, it.peer(), rootName, parent, currentPeerId+1);
	}
}

bool ExportNeuronHOC_SLSR(Output& rOut, const Neuron3D &nrn)
{
	// soma
	if(nrn.CountSomas() > 0)
		SLSRWriteSection(rOut, "somaA", "", nrn.SomaBegin()->m_samples, 6);
	else
	{
		std::vector<SamplePoint> fake;
		fake.push_back(SamplePoint(0,0,0,0));
		SLSRWriteSection(rOut, "somaA", "", fake, 6);
	}

	// axons
	u32 numAxons = 0;
	for(Neuron3D::AxonConstIterator d = nrn.AxonBegin(); d != nrn.AxonEnd();++d, ++numAxons)
	{
		SLSRWriteBranchSamples(rOut, (*d).root(), "axonA" + dec(numAxons+1) + "_", String("somaA"), 1);
	}

	// dendrites
	u32 numDends = 0;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d, ++numDends)
	{
		SLSRWriteBranchSamples(rOut, (*d).root(), "dendA" + dec(numDends+1) + "_", String("somaA"), 1);
	}

	return true;
}

//
// Styles: geometry_to_neuron.pl, and Oz_geometry.pl
//
static void G2NWriteSection(Output& rOut, const String& name, const std::vector<SamplePoint>& samples, u32 precision)
{
	rOut.writeLine("create " + name + "}");
	rOut.writeLine(name + " {");
	rOut.writeLine("  pt3dclear()}");

	for(u32 n=0;n<samples.size();++n)
	{
		rOut.writeLine( "  pt3dadd(" +	dec(samples[n].x,precision) + "," + 
										dec(samples[n].y,precision) + "," + 
										dec(samples[n].z,precision) + "," + 
										dec(samples[n].d,precision) + ")"  );
	}
	rOut.writeLine("}");
}

static void G2NWriteSomaSection(Output& rOut, const Neuron3D& nrn)
{
	if(nrn.CountSomas() > 0)
		G2NWriteSection(rOut, "soma", nrn.SomaBegin()->m_samples, 4);
	else
	{
		std::vector<SamplePoint> fake;
		fake.push_back(SamplePoint(0,0,0,0));
		G2NWriteSection(rOut, "soma", fake, 4);
	}
}


static void G2NWriteConnection(Output& rOut, const String& parent, const String& child)
{
	rOut.writeLine(parent + " connect " + child + "(0),1");
}

static void G2NWriteBranchSamples(Output& rOut, Neuron3D::DendriteTreeConstIterator it, String rootName, int currentPeerId)
{
	String name = rootName + dec(currentPeerId);

	G2NWriteSection(rOut, name, (*it).m_samples, 1);

	if(it.child()) 
		G2NWriteBranchSamples(rOut, it.child(), name, 1);

	if(it.peer())
	{
		G2NWriteBranchSamples(rOut, it.peer(), rootName, currentPeerId+1);
	}
	else
	{
		// write connections, in peer-order for all peers ( 
		if(it.parent())
		{
			Neuron3D::DendriteTreeConstIterator pit = it.parent().child();
			int id = 1;
			while(pit)
			{
				G2NWriteConnection(rOut, name, rootName + dec(id) );
				pit = pit.peer();
				++id;
			}
		}
	}

	// no good - generates connections in reverse order (would work ok, but not consistent with actual G2N output)
	//if(it.parent())
	//	G2NWriteConnection(rOut, parent, name);
}

bool ExportNeuronHOC_G2N(Output& rOut, const Neuron3D &nrn, bool oz)
{
	rOut.writeLine("// neuron.hoc -- This file was automatically generated by");	
	if(oz)
		rOut.writeLine("// from neuron.??? by ./geometry_to_neuron.pl on date");
	else
		rOut.writeLine("// from neuron.??? by ./Oz_neuron.pl on date");
	rOut.writeLine("// DO NOT EDIT!  The file may be generated again and you'll lose your edits.");
	
	// style options...comments etc..

	rOut.writeLine("strdef neuron_name");
	rOut.writeLine("neuron_name = \"neuron\"");

	if(oz)
		G2NWriteSomaSection(rOut, nrn);

	// dendrites
	u32 numDends = 0;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d, ++numDends)
	{
		G2NWriteBranchSamples(rOut, (*d).root(), "dend" + dec(numDends+1) + "_", 1);
	}

	// axons
	u32 numAxons = 0;
	for(Neuron3D::AxonConstIterator d = nrn.AxonBegin(); d != nrn.AxonEnd();++d, ++numAxons)
	{
		G2NWriteBranchSamples(rOut, (*d).root(), "axon" + dec(numAxons+1) + "_", 1);
	}

	if(!oz)
		G2NWriteSomaSection(rOut, nrn);

	// soma connections
	rOut.writeLine("access soma");
	for(u32 n=0;n<nrn.CountDendrites();++n)
		rOut.writeLine("connect dend" + dec(n+1) + "_1(0),1");

	for(u32 n=0;n<nrn.CountAxons();++n)
		rOut.writeLine("connect axon" + dec(n+1) + "_1(0),1");
	//


	return true;
}

//
// Styles: SWC2HOC + CNIC variation
//
static int SWC2HOCWriteBranchConnection(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int parent, int	branchcount, bool axon)
{
	String connectee;
	if(parent == -1)
		connectee = "soma(" + dec(0.5f, 1) + ")";
	else
		connectee = (axon ? "axon[" : "dendrite[") + dec(parent) + "](1)";

	rOut.writeLine("connect " + String(axon ? "axon[" : "dendrite[") + dec(branchcount) + "](0), " + connectee);

	++branchcount;

	if(it.child()) branchcount = SWC2HOCWriteBranchConnection(rOut, it.child(), branchcount-1, branchcount, axon);
	if(it.peer()) branchcount = SWC2HOCWriteBranchConnection(rOut, it.peer(), parent, branchcount, axon);

	return branchcount;
}

static void SWC2HOCWriteConnections(Output& rOut, const Neuron3D &nrn)
{
	int branchcount = 0;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		branchcount = SWC2HOCWriteBranchConnection(rOut, (*d).root(), -1, branchcount, false);
	}

	branchcount = 0;
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		branchcount = SWC2HOCWriteBranchConnection(rOut, (*a).root(), -1, branchcount, true);
	}
}

void SWC2HOCWriteSamples(Output& rOut, const String& name, const std::vector<SamplePoint>& samples)
{
	rOut.writeLine(name + " {");
	rOut.writeLine("   pt3dclear()");

	for(u32 n=0;n<samples.size();++n)
	{
		rOut.writeLine( "   pt3dadd(" +	dec(samples[n].x,6) + "," + 
			dec(samples[n].y,6) + "," + 
			dec(samples[n].z,6) + "," + 
			dec(samples[n].d,6) + ")"  );
	}

	rOut.writeLine("   }");
}

int SWC2HOCWriteBranchSamples(Output& rOut, Neuron3D::DendriteTreeConstIterator it, int current, bool axon)
{
	SWC2HOCWriteSamples(rOut, (axon ? "axon[" : "dendrite[") + dec(current) + "]", (*it).m_samples);

	++current;

	if(it.child()) current = SWC2HOCWriteBranchSamples(rOut, it.child(), current, axon);
	if(it.peer()) current = SWC2HOCWriteBranchSamples(rOut, it.peer(), current, axon);

	return current;
}

void SWC2HOCWriteSamplePoints(Output& rOut, const Neuron3D &nrn)
{
	if(nrn.CountSomas() > 0)
		SWC2HOCWriteSamples(rOut, "soma", nrn.SomaBegin()->m_samples);
	else
		SWC2HOCWriteSamples(rOut, "soma", std::vector<SamplePoint>());

	int branchcount = 0;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
	{
		branchcount = SWC2HOCWriteBranchSamples(rOut, (*d).root(), branchcount, false);
	}

	branchcount = 0;
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		branchcount = SWC2HOCWriteBranchSamples(rOut, (*a).root(), branchcount, true);
	}
}

bool ExportNeuronHOC_SWC2HOC(Output& rOut, const Neuron3D &nrn, const StyleOptions& options, bool cnic)
{
	rOut.writeLine("create soma");
	if(nrn.CountDendrites() > 0)
		rOut.writeLine("create dendrite[" + dec(nrn.CountDendriteElements()) + "]");
	if(nrn.CountAxons() > 0)
		rOut.writeLine("create axon[" + dec(nrn.CountAxonElements()) + "]");
	rOut.writeLine("access soma");

	SWC2HOCWriteConnections(rOut, nrn);
	SWC2HOCWriteSamplePoints(rOut, nrn);

	return true;
}
//
// Style: Topol 
//

void TopolWriteConnection(Output& rOut, const String& parent, const String& child)
{
	rOut.writeLine( "connect" + child + "," + parent );
}

u32 TopolWriteBranchConnections(Output& rOut, Neuron3D::DendriteTreeConstIterator it, u32 branchcount, const String& structurename, const String& parentName)
{
	String name = structurename + ( (branchcount == 0) ? String("(0)") : String("[" +dec(branchcount)+ "]") );

	++branchcount;

	TopolWriteConnection(rOut, parentName, name);

	if(it.child())
		branchcount = TopolWriteBranchConnections(rOut, it.child(), branchcount, structurename, name);
	if(it.peer()) 
		branchcount = TopolWriteBranchConnections(rOut, it.peer(), branchcount, structurename, parentName);

	return branchcount;
}

void TopolWriteConnections(Output& rOut, const Neuron3D &nrn, bool enumTrees)
{
	String parent = "soma(0.5)";

	int branchcount = 0;
	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
	{
		branchcount = TopolWriteBranchConnections(rOut, (*a).root(), branchcount, "axon", parent);
	}

	branchcount = 0;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
		branchcount = TopolWriteBranchConnections(rOut, (*d).root(), branchcount, "dend", parent);
}

void TopolWriteBranchData(Output& rOut, Neuron3D::DendriteTreeConstIterator it, const String& structurename, u32& branchid, u32& functionCount, u32& pointCount)
{
	if(pointCount % 200 == 0)
	{
		++functionCount;
		if(functionCount > 1)
			rOut.writeLine("}");
		rOut.writeLine("proc shape3d_" + dec(functionCount) + "() {");
	}

	String branchName = structurename;
	if(branchid > 0)
		branchName += "[" + dec(branchid) + "]";

	rOut.writeLine("  " + branchName + "{pt3dclear()");

	for(u32 n=0;n<it->m_samples.size();++n)
	{
		if(n>0 && pointCount % 200 == 0)
		{
			rOut.writeLine("  }");
			rOut.writeLine("}");
			++functionCount;
			rOut.writeLine("proc shape3d_" + dec(functionCount) + "() {");
			rOut.writeLine("  " + branchName + "{");
		}

		SamplePoint& pt = it->m_samples[n];
		rOut.writeLine("  pt3dadd(" + dec(pt.x) + ", " + dec(pt.x) + ", " + dec(pt.x) + ", " + dec(pt.d) + ")");
		++pointCount;
	}

	rOut.writeLine("  }");


	++branchid;

	if(it.child())
		TopolWriteBranchData(rOut, it.child(), structurename, branchid, functionCount, pointCount);
	if(it.peer()) 
		TopolWriteBranchData(rOut, it.peer(), structurename, branchid, functionCount, pointCount );
}

u32 TopolWriteData(Output& rOut, const Neuron3D &nrn, bool enumTrees)
{
	u32 functionCount = 0;
	u32 pointCount = 0;

	String currentObject = "soma";

	u32 branchId = 0;

	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
		TopolWriteBranchData(rOut, (*a).root(), "axon", branchId, functionCount, pointCount);

	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
		TopolWriteBranchData(rOut, (*d).root(), "dend", branchId, functionCount, pointCount);

	rOut.writeLine("}");

	return functionCount;
}

void TopolWriteFunctions(Output& rOut, u32 numFunctions)
{
	rOut.writeLine("proc basic_shape() {");
	for(u32 n=1;n<=numFunctions;++n)
		rOut.writeLine("  shape3d_" + dec(n) + "()");
	rOut.writeLine("}");
}
bool ExportNeuronHOC_Topol(Output& rOut, const Neuron3D &nrn, const StyleOptions& options, bool enumTrees)
{
	rOut.writeLine("");
	rOut.writeLine("proc celldef() {");
	rOut.writeLine("  topol()");
	rOut.writeLine("}");

	//
	rOut.writeLine("");

	String create = "create soma";
	if(nrn.CountAxons() > 0)
	{
		if(enumTrees)
		{
			u32 axonId = 1;
			for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a, ++axonId)
			{
				create += ", axon" + dec(axonId);
				u32 numSections = (*a).size();
				if(numSections > 1)
					create += "[" + dec(numSections) + "]";
			}
		}
		else
		{
			create += ", axon";
			u32 numSections = nrn.CountAxonElements();
			if(numSections > 1)
				create += "[" + dec(numSections) + "]";
		}
	}
	if(nrn.CountDendrites() > 0)
	{
		if(enumTrees)
		{
			u32 dendId = 1;
			for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d, ++dendId)
			{
				create += ", dend" + dec(dendId);
				u32 numSections = (*d).size();
				if(numSections > 1)
					create += "[" + dec(numSections) + "]";
			}
		}
		else
		{
			create += ", dend";
			u32 numSections = nrn.CountDendriteElements();
			if(numSections > 1)
				create += "[" + dec(numSections) + "]";
		}
	}

	rOut.writeLine(create);
	rOut.writeLine("");
	//


	rOut.writeLine("proc topol() { local i");
	
	TopolWriteConnections(rOut, nrn, enumTrees);
	
	rOut.writeLine("  basic_shape()");
	rOut.writeLine("}");

	u32 numFunctions = TopolWriteData(rOut, nrn, enumTrees);

	rOut.writeLine("proc basic_shape() {");
	
	TopolWriteFunctions(rOut, numFunctions);
	
	rOut.writeLine("}");
	rOut.writeLine("access soma");
	rOut.writeLine("");
	rOut.writeLine("celldef()");
	rOut.writeLine("");


	return true;
};


//
// Style: MorphMLXSL
//
static void MorphMLXSLDeclareSection(Output& rOut, const String& name)
{
	rOut.writeLine("        create " + name);
	rOut.writeLine("        public " + name);
}

static void MorphMLXSLWriteSection(Output& rOut, const String& name, const String& parent, std::vector<SamplePoint>& samples, u32 cablecount)
{
	const u32 precision = 1;

	rOut.writeLine("        // Adding Section: " + name + ", ID: " + dec(cablecount) + " to cell");
	rOut.writeLine("        " + name + " {");
	rOut.writeLine("            pt3dclear()");
	for(u32 n=0;n<samples.size();++n)
	{
		const String comment = (n==0 ?	String("//proximal point of segment Seg0_") + name : 
										String("//distal point of segment Seg" + dec(n) + "_" + name) );

		rOut.writeLine("            pt3dadd(" +	dec(samples[n].x, precision) + ", " + 
												dec(samples[n].y, precision) + ", " + 
												dec(samples[n].z, precision) + ", " + 
												dec(samples[n].d, precision) + ")"  +
												comment);
	}

	rOut.writeLine("");
	rOut.writeLine("        }");

	String duplicatedComment = "// Parent section of " + name + " is: " + parent;
	rOut.writeLine("        " + duplicatedComment + duplicatedComment);

	if(parent != "none")
	{
		rOut.writeLine("        connect " + name + "(0), " + parent + "(1)");
		rOut.writeLine("");
	}
	rOut.writeLine("        // Finished section: " + name);
	rOut.writeLine("");
}

static u32 MorphMLXSLWriteBranchSectionDeclaration(Output& rOut, Neuron3D::DendriteTreeConstIterator it, u32 cablecount)
{
	MorphMLXSLDeclareSection(rOut, "cable" + dec(cablecount) );

	if(it.child())
		cablecount = MorphMLXSLWriteBranchSectionDeclaration(rOut, it.child(), cablecount+1);

	if(it.peer())
		cablecount = MorphMLXSLWriteBranchSectionDeclaration(rOut, it.peer(), cablecount+1);

	return cablecount;
}

static u32 MorphMLXSLWriteBranchSectionData(Output& rOut, Neuron3D::DendriteTreeConstIterator it, u32 cablecount, const String& parentName)
{
	String thisName = "cable" + dec(cablecount);

	MorphMLXSLWriteSection(rOut, thisName, parentName, it->m_samples, cablecount );

	++cablecount;

	if(it.child())
		cablecount = MorphMLXSLWriteBranchSectionData(rOut, it.child(), cablecount, thisName);

	if(it.peer())
		cablecount = MorphMLXSLWriteBranchSectionData(rOut, it.peer(), cablecount, parentName);

	return cablecount;
}

static void MorphMLXSLWriteSectionDeclarations(Output& rOut, const Neuron3D& nrn)
{
	int cablecount = 0;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
		cablecount = MorphMLXSLWriteBranchSectionDeclaration(rOut, (*d).root(), cablecount);

	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
		cablecount = MorphMLXSLWriteBranchSectionDeclaration(rOut, (*a).root(), cablecount);
}

static void MorphMLXSLWriteSectionData(Output& rOut, const Neuron3D& nrn)
{
	int cablecount = 0;
	for(Neuron3D::DendriteConstIterator d = nrn.DendriteBegin(); d != nrn.DendriteEnd(); ++d)
		cablecount = MorphMLXSLWriteBranchSectionData(rOut, (*d).root(), cablecount, "");

	for(Neuron3D::AxonConstIterator a = nrn.AxonBegin(); a != nrn.AxonEnd(); ++a)
		cablecount = MorphMLXSLWriteBranchSectionData(rOut, (*a).root(), cablecount, "");
}

bool ExportNeuronHOC_MorphMLXSL(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	const String cellName = options.cellName;

	if(options.identifyNL)
	{
		rOut.writeLine("// This file generated by "  + options.appName + "(" + options.appVersion + ")");
		rOut.writeLine("");
	}
	if(options.addCommentText)
	{
		rOut.writeLine("// " + options.commentText);
		rOut.writeLine("");
	}

	rOut.writeLine("// This is a NEURON representation of the morphological components of a single cell described in a MorphML file.");
	rOut.writeLine("// Note: this mapping is only for Level 1 NeuroML morphologies. To convert a Level 2+ file (e.g. including channel densities)");
	rOut.writeLine("// to NEURON, use a NeuroML compliant application/metasimulator like neuroConstruct.");
	rOut.writeLine("");
	rOut.writeLine("// Cell name: " + cellName);
	rOut.writeLine("");
	rOut.writeLine("// Cell Description: ");


	// common preamble
	rOut.writeLine("");
	rOut.writeLine("begintemplate " + cellName);
	rOut.writeLine("");
	rOut.writeLine("    public init, topol");
	rOut.writeLine("    public all");
	rOut.writeLine("");
	rOut.writeLine("    objref all");
	rOut.writeLine("");
	rOut.writeLine("    proc init() {");
	rOut.writeLine("        topol()");
	rOut.writeLine("    }");
	rOut.writeLine("");
	rOut.writeLine("");

	//
	rOut.writeLine("    // Creating Sections");
	rOut.writeLine("");

	MorphMLXSLWriteSectionDeclarations(rOut, nrn);

	rOut.writeLine("");
	rOut.writeLine("    proc topol() {");
	rOut.writeLine("");

	MorphMLXSLWriteSectionData(rOut, nrn);

	rOut.writeLine("");
	rOut.writeLine("    } // end topol()");
	rOut.writeLine("");
	rOut.writeLine("");
	rOut.writeLine("endtemplate " + cellName);
	rOut.writeLine("");


	return true;
}



//
// Style: NTSCable - call separate exporter
//

bool ExportNeuronHOC_NTSCable(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	return ExportNeuronNTSCable(rOut, nrn, options);
};


bool ExportNeuronHOC(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	switch(options.formatStyle)
	{
	case kStyleHoc_SLSR:
		return ExportNeuronHOC_SLSR(rOut, nrn);

	default:
	case kStyleHoc_SWC2HOC:
	case kStyleHoc_CNIC:
		return ExportNeuronHOC_SWC2HOC(rOut, nrn, options, (options.formatStyle == kStyleHoc_CNIC) );

	case kStyleHoc_GeometryToNeuronPL:
	case kStyleHoc_OzNeuronPL:
		return ExportNeuronHOC_G2N(rOut, nrn, (options.formatStyle == kStyleHoc_OzNeuronPL) );

	case kStyleHoc_NTSCable:
		return ExportNeuronHOC_NTSCable(rOut, nrn, options);

	case kStyleHoc_Topol:
	case kStyleHoc_TopolEnumTrees:
		return ExportNeuronHOC_Topol(rOut, nrn, options, (options.formatStyle == kStyleHoc_TopolEnumTrees) );

	case kStyleHoc_MorphMLXSL:
		return ExportNeuronHOC_MorphMLXSL(rOut, nrn, options);
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateNeuronHOC(Input& rIn, const ValidationOptions& options)
{
	String line;


	// search pt3dadd, create, connect keywords
	// if all 3 are present, then assume this is a HOC file..

	// alternatively, could do full import of the file, and check for failure

	if( ValidateNeuronNTSCable(rIn, options) )
		return kValidationTrue;

	rIn.seek(0, IoBase::Absolute);
	if( ValidateNeuronSWC2HOC(rIn, options) )
		return kValidationTrue;


	return kValidationFalse;
}



#if 0

// old NeuronGeneric importer (SLSR style only)

class NeuronHOCParser : public SimpleParser
{
public:
	NeuronHOCParser(Input& rIn, Neuron3D &nrn) : SimpleParser(rIn), builder(nrn)
	{
	}

	void ReadSamples(const String& name, vector<SamplePoint>& samples)
	{
		String line;
		SamplePoint sample;

		line = rIn.readLine();
		assert(line.begins(name));

		line = rIn.readLine();
		assert(line == "   pt3dclear()");

		while(1)
		{
			line = rIn.readLine();

			if(line.begins("   pt3dadd("))
			{
				String sampleDataStr = line.after("   pt3dadd(").before(")");
				Strings values = sampleDataStr.split(",");
				values.clean();

				sample.x = values[0].toFloat();
				sample.y = values[1].toFloat();
				sample.z = values[2].toFloat();
				sample.d = values[3].toFloat();

				samples.push_back( sample );
			}
			else if(line.begins("   }"))
			{
				break;
			}
			else
			{
				// bad
			}
		}
	}

	int ReadBranchSampleData(Neuron3D::DendriteTreeIterator it, int current)
	{
		ReadSamples("dendrite[" + dec(current) + "]", (*it).m_samples );
		++current;

		if(it.child()) current = ReadBranchSampleData(it.child(), current);
		if(it.peer()) current = ReadBranchSampleData(it.peer(), current);

		return current;
	}

	enum ObjectType 
	{
		kSoma,
		kAxon,
		kDendrite
	};

	bool Parse()
	{
		String line;
		map<String, MorphologyBuilder::Branch> connectionMap;

		while(rIn.remaining() > 0)
		{
			line = rIn.readLine();

			if(line.empty() || line.begins("//"))
				continue;

			if(line.begins("{create"))
			{
				// what is this object
				String objectName = line.after("{create ").before("}");
				String connecteeName("");
				ObjectType object;

				if(objectName.begins("soma"))
					object = kSoma;
				else if(objectName.begins("axon"))
					object = kAxon;
				else if(objectName.begins("dend"))
					object = kDendrite;
				else
					return false;

				// is it connected to another object
				if(object == kAxon || object == kDendrite)
				{
					line = rIn.readLine();
					if(!line.contains(" connect " + objectName))
						return false;

					connecteeName = line.after("{").before(" connect");

					if(connecteeName.begins("soma"))
					{
						if(object == kDendrite)
							builder.NewDendrite();
						else
							builder.NewAxon();
					}
					else
					{
						builder.SetCurrentBranch(connectionMap[connecteeName]);
						builder.CbNewChildLast(false);
					}

					connectionMap[objectName] = builder.CurrentBranch();
				}
				else
				{
					builder.NewSomaOpenContour();
				}


				// skip uninteresting things
				line = rIn.readLine();
				if(!line.begins("{access " + objectName))
					return false;

				line = rIn.readLine();
				if(!line.begins("{nseg = "))
					return false;

				line = rIn.readLine();
				if(line != "{pt3dclear()}")
					return false;

				// read 3D points
				SamplePoint sample;

				line = rIn.readLine();
				while(line != "")
				{
					if(!line.begins("{pt3dadd("))
						return false;

					String sampleData = line.after("{pt3dadd(").before(")}");
					Strings valueStrings = sampleData.split(",");
					valueStrings.clean();

					sample.x = valueStrings[0].toFloat();
					sample.y = valueStrings[1].toFloat();
					sample.z = valueStrings[2].toFloat();
					sample.d = valueStrings[3].toFloat();

					if(object == kSoma)
						builder.CsAddSample(sample);
					else
						builder.CbAddSample(sample);

					line = rIn.readLine();
				}
			}

			if(builder.HasFailed())
				return false;
		}

		builder.ApplyChanges();
		return true;
	}

private:
	MorphologyBuilder builder;
};


#endif