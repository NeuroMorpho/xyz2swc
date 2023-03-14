//
//
//


#include <stdio.h>

#include "NeuronMorphologyFormatImportExportBuiltIn/BuiltInMorphologyFormat.h"


void PrintVersion()
{
	printf("NLMorphologyConverter version 0.10.0.alpha (August 2012)\n");
}

void PrintUsage()
{
	printf("\n--------------------------------------------------\n\n");
	PrintVersion();
	printf("\nNeuron Morphology 3D File Format Converter\n");
	printf("\nCopyright 2008-2021 James Ogden (Neuronland) www.neuronland.org\n");
	printf("\n--------------------------------------------------\n\n");
	printf("Usage (help)   : APPNAME [--version | --help]\n");
	printf("Usage (query)  : APPNAME <file> [--stats, --warnings]\n");
	printf("Usage (convert): APPNAME <in file> <out file> <out format> [modify options]\n");
	printf("Usage (merge)  : APPNAME --export <output file> <output format> \n");
	printf("                         --merge <input file 1> [modify options]\n");
	printf("                         --merge <input file 2> [modify options] .. \n\n");
	printf("\t<in file>           (the neuron data file to be queried/converted)\n");
	printf("\t<out file>          (name of 3D neuron data file to be generated)\n");
	printf("\t<out format>        (format of 3D neuron data file to be generated - \n");
	printf("\t                     can be one of the following: \n\n");
	u32 length = 0;
	for(u32 n=0;n<kMorphologyFormatBuiltInNumber;++n)
	{
		BuiltInMorphologyFormat format = (BuiltInMorphologyFormat)n;
		// don't mention non-3D formats for now...
		if(	IsFormatFull3D(format) && format != kMorphologyFormatImarisHDF)
		{
			String formatName = String( GetBuiltInMorphologyFormatDetails(format).GetName());

			length += formatName.length() + 2;

			if(length > 78)
			{
				printf("\n");
				length = formatName.length() + 2;
			}

			printf("%s, ", formatName.c_str() );
		}
	}
	printf("\n\n");
	printf("\n--------------------------------------------------\n\n");
	printf("(A) HELP OPTIONS (ignored unless specified as unique option)\n\n");
	printf("\t--version           (displays version number of this application)\n");
	printf("\t--help              (displays this usage message)\n");
	printf("\n--------------------------------------------------\n\n");
	printf("(B) INFO OPTIONS (input file must be specified)\n\n");
	printf("\t--verbose           (provides more details about the conversion)\n");
	printf("\t--report <filename> (output report file detailing import, stats,\n");
	printf("\t                     warnings, modifications & export, where relevant\n");
	printf("\t--test <filename> <format> <logfile> \n");
	printf("\t                     (run test regime on single file, assuming it is\n");
	printf("\t                      in given format. Overrides any other options. )\n");
	printf("\n--------------------------------------------------\n\n");
	printf("(C) IMPORT OPTIONS (input file must be specified)\n\n");
	printf("\t--stats             (output neuron and tree statistics)\n");
	printf("\t--warnings          (output a list of possible data problems)\n");
	printf("\t--strictValidation  (don't allow usually-accepted files which\n");
	printf("\t                     break the format rules slightly)\n");
	printf("\t--hintSWCStyle <style>  \n");
	printf("\t                    (explicitly specify the SWC format variation, \n");
	printf("\t                     to help importer algorithm: style can \n");
	printf("\t                     be one of 'srdswc','cnicswc','gulyasswc'.)\n");
	printf("\t--hintSoma <substring>  \n");
	printf("\t                    (specify name substring for data which\n"); 
	printf("\t                     should be treated as soma data. \n");
	printf("\t                     Default: depends on format. See user manual).\n");
	printf("\t--hintAxon <substring>  \n");
	printf("\t                    (specify name substring for data which\n"); 
	printf("\t                     should be treated as axon data. \n");
	printf("\t                     Default:  depends on format. See user manual).\n");
	printf("\t--hintDend <substring>  \n");
	printf("\t                    (specify name substring for data which\n"); 
	printf("\t                     should be treated as dendrite data. \n");
	printf("\t                     Default:  depends on format. See user manual).\n");
	printf("\t--hintAllContoursAsSoma\n");
	printf("\t                    (all contours are treated as soma data.\n");
	printf("\t                     NeurolucidaASC and NeuroZoom only.)\n");
	printf("\t--hintNeuronHOCEntryProc\n");
	printf("\t                    (If relevant HOC code is not triggered from global\n");
	printf("\t                     scope, specify start proc here. NeuronHOC only)\n");
	printf("\t--set1DBifurAngle <ANGLE>\n");
	printf("\t--set1DBifurAngleApical <ANGLE>\n");
	printf("\t--set1DBifurAngleAxon <ANGLE>\n");
	printf("\t                    (When importing 1D formats, specify the child \n");
	printf("\t                     branch bifurcation angle, in degrees.)\n");
	printf("\t--set1DRootAngleRange <ANGLE> <RANGE>\n");
	printf("\t--set1DRootAngleRangeApical <ANGLE> <RANGE>\n");
	printf("\t--set1DRootAngleRangeAxon <ANGLE> <RANGE>\n");
	printf("\t                    (When importing 1D formats, specify the center\n");
	printf("\t                     angle and angle range for root points)\n");
	printf("\t--set1DBranchingPattern <PATTERN>\n");
	printf("\t                    (When importing 1D formats, specify general \n");
	printf("\t                     branching pattern. Currently can only be 'planar')\n");
	printf("\t--merge <filename> [modify options] \n");
	printf("\t                    (neuron data is to be merged\n");
	printf("\t                     with other imported neuron data, subject\n");
	printf("\t                     to the subsequent modifying options)\n");
	printf("\n--------------------------------------------------\n\n");
	printf("(D) MODIFY OPTIONS (applied globally, or per merged file.)\n\n");
	printf("\t--omitAllSomas      (omit all soma data from output)\n");
	printf("\t--omitAllAxons      (omit all axon data from output)\n");
	printf("\t--omitAllDends      (omit all dendrite data from output)\n");
	printf("\t--omitSomas a,b,c   (omit specific soma data (contours) from \n");
	printf("\t                     output - comma delimited list of indices)\n");
	printf("\t--omitTrees a,b,c   (omit specific trees from output -\n");
	printf("\t                     comma delimited list of tree indices)\n");
	printf("\t--omitAllMarkers    (omit all marker data from output)\n");
	printf("\t--omitGlobalMarkers (omit global marker data from output)\n");
	printf("\t--omitSomaMarkers   (omit soma-related marker data from output)\n");
	printf("\t--omitDendMarkers   (omit dendrite related marker data from output)\n");
	printf("\t--omitAxonMarkers   (omit axon related marker data from output)\n");
	printf("\t--omitAllSpines     (omit all spine data from output)\n");
	printf("\t--omitGlobalSpines  (omit global spine data from output)\n");
	printf("\t--omitDendSpines    (omit dendrite related spine data from output)\n");
	printf("\t--omitAxonSpines    (omit axon related spine data from output)\n");
	printf("\t--allDendsAsAxons   (dendrites are exported as axons)\n");
	printf("\t--allAxonsAsDends   (axons are exported as dendrites)\n");
	printf("\t--flipTreeType a,b,c\n");
	printf("\t                    (flip specific axons and dendrites -\n");
	printf("\t                     comma delimited list of tree indices)\n");
	printf("\t--reverseAllTrees   (reverse the child order at all tree branch points)\n");
	printf("\t--reverseTrees a,b,c\n");
	printf("\t                    (reverse the child order at branch points of the\n");
	printf("\t                     specified trees - comma delimited list of\n");
	printf("\t                     tree indices)\n");
	printf("\t--markersToSpines [all | <string>] \n");
	printf("\t                    (marker names which match the specified string\n"); 
	printf("\t                     are treated as spine data. Only for formats with\n"); 
	printf("\t                     marker support.)\n"); 
	printf("\t--spinesToMarkers <marker name> \n");
	printf("\t                    (convert all spines to markers with the \n"); 
	printf("\t                     specified name)\n"); 
	printf("\t--spinesToBranches  (convert all tree spines to branches)\n"); 
	printf("\t--mapGlobalSpinesToBranches <proximity limit>\n");
	printf("\t                    (all global spines within a distance of limit(um)\n");
	printf("\t                     from a branch, are attached to the closest branch\n");
	printf("\t                     relevant input formats: NeuroZoom)\n");
	printf("\t--mapGlobalMarkersToBranches <limit>\n");
	printf("\t                    (all global markers within a distance of limit(um)\n");
	printf("\t                     from a branch, are attached to the closest branch)\n");
	printf("\t--forceSpinesToBeGlobal\n");
	printf("\t                    (All spine data is de-associated from branches\n");
	printf("\t                     and added to the list of global spine data.)\n");
	printf("\t--forceMarkersToBeGlobal\n");
	printf("\t                    (All marker data is de-associated from branches\n");
	printf("\t                     and added to the list of global marker data.)\n");
	printf("\t--forceSphereSoma <radius>\n");
	printf("\t                    (deletes all soma data, replacing with a \n");
	printf("\t                     single simple Sphere soma.)\n");
	printf("\t--forceCylinderSoma <radius> <length>\n");
	printf("\t                    (deletes all soma data, replacing with a \n");
	printf("\t                     single simple Cylinder soma.)\n");
	printf("\t--scale x y z d     (scaling factors for co-ordinates, diameter)\n");
	printf("\t--translate x y z   (shift all positional data by the given amount)\n");
	printf("\t--rotate x y z A    (rotate A degrees around direction vector x,y,z)\n");
	printf("\t--moveOriginToSoma  (entire neuron is translated so that soma or \n");
	printf("\t                     tree root mid-point is at the origin (0,0,0). ) \n");
	printf("\n--------------------------------------------------\n\n");
	printf("(E) EXPORT OPTIONS (input file must have been specified)\n\n");
	printf("\t--export <filename> <fileformat>	 \n");
	printf("\t                    (explicitly specify filename. This option is only\n");
	printf("\t                     required when using --merge)\n");
	printf("\t--version <version> (output a specific version of the output format.\n");
	printf("\t                     NeuroML: any valid version number\n");
	printf("\t                     e.g. 1.0, 1.3, 1.8.2.)\n");
	printf("\t--style <style>     (export a specific variation of the output format:\n");
	printf("\t                      AmiraMesh:      lneuron*, flycircuit,\n");
	printf("\t                                      skeletongraph\n");
	printf("\t                      NeuronHOC:      slsr*, swc2hoc, ntscable,\n");
	printf("\t                                      geometry_to_neuron, Oz_neuron \n");
	printf("\t                                      topol.\n");
	printf("\t                      NeurolucidaASC: modern*, classictags, verbose,\n");
	printf("\t                                      nocomments, compact, nmpml\n");
	printf("\t                      NeurolucidaDAT: badmarkersize*, correctmarkersize\n");
	printf("\t                      NeuroZoom:      longpath*, shortpath.\n");
	printf("\t                      SWC:            stdswc*, cnicswc.)\n");
	printf("\t--setPrefixSoma     (used to override name prefix for soma structures,\n");
	printf("\t                     for relevant formats.\n");
	printf("\t--setPrefixAxon     (used to override name prefix for axon structures,\n");
	printf("\t                     for relevant formats.\n");
	printf("\t--setPrefixDend     (used to override name prefix for dendrite\n");
	printf("\t                     structures, for relevant formats.\n");
	printf("\t--identifyNL        (use a comment, where possible to identify \n");
	printf("\t                     NLMorphologyConverter as creator)\n");
	printf("\t--cellName          (give cell a name, relevant for certain formats.)\n");
	printf("\t--addComment <text> (formats which support comments, will include text)\n");
	printf("\t--versionNumber <string>\n");
	printf("\t                    (specify specific file version number to export.\n");
	printf("\t                     - NeuroML only.)\n");
	printf("\t--colorSoma, --colourSoma,\n");
	printf("\t--colorAxon, --colourAxon, \n");
	printf("\t--colorDendrite, --colourDendrite,\n");
	printf("\t--colorSpine, --colourSpine, \n");
	printf("\t--colorMarker, --colourMarker\n");
	printf("\t  [ Red, Green, Blue, Yellow, Cyan, Magenta, White, DarkRed,\n");
	printf("\t    DarkGreen, DarkBlue, DarkYellow, DarkCyan, DarkMagenta,\n");
	printf("\t    Orange, MediumGray, BrightGray, SkyBlue, MoneyGreen,\n");
	printf("\t    Cream | RGB(RRR:GGG:BBB) ]\n");
	printf("\t                    (specify a color per structure type. Only \n");
	printf("\t                     relevant for Neurolucida and NeuroZoom formats.)\n");
	printf("\t--split <filenameprefix> <fileextension>\n");
	printf("\t                    (axonal and dendritic trees are exported to \n");
	printf("\t                     individual files. Soma data is ignored.)\n");
	printf("\n--------------------------------------------------\n\n");
}

void PrintHelpHint()
{
	PrintVersion();
	printf("#### use option '--help' for a full list of command line parameter ####\n");
}



