//
//
//

#include "ImportExportCommonIncludes.h"

#include <algorithm>
#include <math.h>

// #if !defined(__linux__)
// #define HDF_ENABLE
// #endif

// #if defined(HDF_ENABLE)
#include "hdf5.h"
#include "hdf5_utils.h"
// #endif

#include "Core/Parser.h"

/**

	Imaris HDF format (imaris 5.5 only, earlier versions have no morphology data..)

	Binary, Little-Endian

	Use general HDF5 loading code to access.. (not so different from xml, in a way..)

	Structure supported ->


	GROUP(Scene):GROUP(Content)
					ATTRIB:NumberOfFilaments
					GROUP(Filaments0):GROUP(GraphTracks)
											ATTRIB(Units)
											DATASET(GraphVertex) // ?
									 :GROUP(Graphs)
											ATTRIB(Units)
											DATASET(Segments)	// <--- connectivity
											DATASET(Vertices)	// <--- vertex position/diameter + other info?

*/


const size_t HDF_HEADER_SIZE = 8;

const u8 hdf_header[HDF_HEADER_SIZE] =
{
	0x89,  'H',  'D',  'F', 0x0D, 0x0A, 0x1A, 0x0A
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Import
//


bool ImportImarisHDF(Input& rIn, Neuron3D &nrn, const HintOptions& options)
{
	// not possible..maybe think about new type of input, which doesnt open, jusst contains file name!?
	return false;
}


struct SegmentConnection
{
	u64 parent;
	u64 current;
};

struct SegmentVertex
{
	float x;
	float y;
	float z;
	float d;
	float spine; // 1, spine, 0, dendrite
};

struct RootsAndCounts
{
	u64 times;
	u64 vertices;
	u64 edges;
	u64 roots;
};

bool ImportRawImarisHDF(const char* file, Neuron3D &nrn, const HintOptions& options)
{
// #if defined(HDF_ENABLE)
	const String& kTagSampleNumber	= "samplenum";
	const String& kTagSpine			= "spine";

	bool success = true;

	if(H5::H5File::isHdf5(file))
	{
		try
		{
			auto_ptr<H5::H5File> h5file( new H5::H5File( file, H5F_ACC_RDONLY ) );

			H5::Group groupS(h5file->openGroup("Scene"));
			H5::Group groupC(groupS.openGroup("Content") );

			const u32 numFilaments = read_hdf5_scalar_attribute<int>(groupC, "NumberOfFilaments");

			MorphologyBuilder builder(nrn);

			for(u32 n=0;n<numFilaments;++n)
			{
				String filamentName = String("Filaments") + dec(n);

				H5::Group groupF( groupC.openGroup( filamentName) );
				H5::Group groupG( groupF.openGroup("Graphs") );


				// Units
				String unitStr = read_hdf5_scalar_attribute<String>(groupG, "Unit");
				float multiplier = 1.0f;
				bool rescale = false;

				if(unitStr.find_first_of("um") == string::npos)
				{
					rescale = true;

					if(unitStr == "mm")
					{
						multiplier = 1000.f;
					}
					else if(unitStr == "m")
					{
						multiplier = 1000.f*1000.f;
					}
					else
					{
						LogImportWarning("Graphs:Unit: unknown unit.. assuming um!!" );
					}
				}


				std::vector<SegmentConnection>	segmentConnections;
				std::vector<SegmentVertex>		segmentVertices;
				std::vector<RootsAndCounts>		counts;

				hsize_t numConnections = 0;
				hsize_t numVertices = 0;


				// general temp storage
				hsize_t dims_out[2];

				// Extract segments
				{
					try
					{
						H5::DataSet segments( groupG.openDataSet("Segments") );
						H5::DataSpace dsSegments = segments.getSpace();
						const hsize_t rank = dsSegments.getSimpleExtentDims(dims_out, NULL);
						const hsize_t numElemsPerConnection = dims_out[1];
						numConnections = dims_out[0];

						if(rank != 2)
						{
							LogImportFailure("DATASET: 'Segments' does not have rank '2'" );
							success = false;
							break;
						}
						if(numElemsPerConnection != 2)
						{
							LogImportFailure("DATASET: 'Segments' elements per sample is not '2'" );
							success = false;
							break;
						}

						segmentConnections.resize(numConnections);
						segments.read(&segmentConnections.front(), PredType::STD_U64LE);
					}
					catch( ... ) {}
				}


				// Extract vertices
				{
					try
					{
						H5::DataSet vertices( groupG.openDataSet("Vertices") );
						H5::DataSpace dsVertices = vertices.getSpace();

						const hsize_t rank = dsVertices.getSimpleExtentDims(dims_out, NULL);
						const hsize_t numElemsPerVertex = dims_out[1];
						numVertices = dims_out[0];

						if(rank != 2)
						{
							LogImportFailure("DATASET: 'Vertices' does not have rank '2'" );
							success = false;
							break;
						}
						if(numElemsPerVertex != 5)
						{
							LogImportFailure("DATASET: 'Vertices' elements per sample is not '5'" );
							success = false;
							break;
						}

						segmentVertices.resize(numVertices);
						vertices.read(&segmentVertices.front(), PredType::IEEE_F32LE);
					}
					catch( ... ) {}
				}


				// Roots and Counts
				{
					try
					{
						H5::DataSet rootsAndCounts( groupG.openDataSet("TimesNVerticesNEdgesRoots") );
						H5::DataSpace dsrootsAndCounts = rootsAndCounts.getSpace();

						const hsize_t rank = dsrootsAndCounts.getSimpleExtentDims(dims_out, NULL);
						const hsize_t numRoots = dims_out[0];

						counts.resize(numRoots);
						rootsAndCounts.read(&counts.front(), PredType::STD_U64LE);
					}
					catch( ... ) {}
				}

				// Modify data if necessary
				if(rescale)
				{
					for(u32 n=0;n<numVertices;++n)
					{
						segmentVertices[n].x *= multiplier;
						segmentVertices[n].y *= multiplier;
						segmentVertices[n].z *= multiplier;
						segmentVertices[n].d *= multiplier;
					}
				}


				// now build the morphology
				if(numConnections > 0)
				{
					u64 edgeOffset = 0;
					u64 vertexOffset = 0;

					for(u64 t = 0; t < counts.size();++t)
					{
						// some assumptions for now..
						assert(segmentConnections[edgeOffset].parent == 0);
						assert(segmentConnections[edgeOffset].current == 1);
						//

						builder.ClearTags();

						for(u64 n=0;n<counts[t].edges;++n)
						{
							const u64 parent = segmentConnections[edgeOffset + n].parent;
							const u64 current = segmentConnections[edgeOffset + n].current;
							const SegmentVertex& currentVertex = segmentVertices[vertexOffset + current];
							const SamplePoint currentSample(currentVertex.x, currentVertex.y, currentVertex.z, currentVertex.d);

							const bool isSpine = currentVertex.spine;

							if(current < parent)
							{
								LogImportFailure("Problem with connectivity assumptions: current less than parent");
								return false;
							}

							MorphologyBuilder::Branch parentBran = builder.CdFindBranchWithTagCondition(kTagSampleNumber, MorphologyBuilder::CheckIfTagInRangeFunctor(parent));
							const bool unacceptableParent = (parentBran == MorphologyBuilder::NullBranch());

							if(unacceptableParent)
							{
								const SegmentVertex& rootVertex = segmentVertices[vertexOffset + parent];
								const SamplePoint rootSample(rootVertex.x, rootVertex.y, rootVertex.z, rootVertex.d);

								builder.NewDendriteWithSample(rootSample);
								builder.CbSetTag(kTagSampleNumber, current);
								builder.CbSetTag(kTagSpine, 0);
							}
							else
							{
								builder.SetCurrentBranch(parentBran);
							}

							builder.CbNewChildLast(true);
							builder.CbSetTag(kTagSampleNumber, current);

							if(isSpine)
								builder.CbAddSpine( SpinePoint(currentSample, kSpineContourRoot, current) );

							builder.CbAddSample(currentSample);

							if(builder.HasFailed())
							{
								LogImportFailure( String("Morphology Builder failure: ") + builder.GetError() );
								return false;
							}
						}

						edgeOffset += counts[t].edges;
						vertexOffset += counts[t].vertices;
					}
				}
			}

			// Finished with HDF5 file
			h5file->close();

			builder.ApplyChanges();

			nrn.MergeAllUnaryBranchPoints();
			//nrn.EnhanceSingleSampleTrees(); // unnecessary..cant be any

			return true;
		}
		// Unfortunately, HDF C++ API uses exceptions as a general error handling mechanism..
		catch( H5::FileIException error )
		{
			LogImportWarning( String("No Scene/Content/Filament data... ") );
			return true;
			//LogImportFailure( String("H5::FileIException: ") + error.getDetailMsg().c_str() );
		}
		catch( H5::GroupIException error )
		{
			//LogImportFailure( String("H5::GroupIException: ") + error.getDetailMsg().c_str() );
			LogImportWarning( String("No Scene/Content/Filament data... ") );
			return true;
		}
		catch( H5::AttributeIException error )
		{
			LogImportFailure( String("H5::AttributeIException: ") + error.getDetailMsg().c_str() );
		}
		catch( H5::DataSetIException error )
		{
			LogImportFailure( String("H5::DataSetIException: ") + error.getDetailMsg().c_str() );
		}
	}
	else
	{
		LogImportFailure("Not HDF5 file");
		return false;
	}
// #endif

	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Export
//

bool ExportImarisHDF(Output& rOut, const Neuron3D &nrn, const StyleOptions& options)
{
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Validate
//

ValidationResult ValidateImarisHDF(Input& rIn, const ValidationOptions& options)
{
	// HDF header
	char header[HDF_HEADER_SIZE];
	rIn.read(header, HDF_HEADER_SIZE);
	const bool valid = (strncmp(header, (const char*)hdf_header, HDF_HEADER_SIZE) == 0);

	// TODO: additional check for Imaris-specific data ?

	return valid ? kValidationTrue : kValidationFalse;
}
