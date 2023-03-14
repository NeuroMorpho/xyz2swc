#ifndef _NEUROLUCIDACLASSICHELPERS_HPP_
#define _NEUROLUCIDACLASSICHELPERS_HPP_

/**

	Functionality which can be shared between 

	NeurolucidaASCClassic and NeurolucidaDATClassic, 

	which are very similar, only one is ASCII, the other binary..

*/

#include "ImportExportCommonIncludes.h"

class Output;
class Neuron3D;
struct StyleOptions;
struct SamplePoint;

struct NCHeaderDetails
{
	String	fileName;
	float	max[3];
	float	min[3];
	u32		fileType;
	u32		id1;
	u32		id2;
	float 	ref[3];
	float 	rotation;
	int 	gx_alpha;
	int 	gx_beta;
	int 	gx_gamma;
};


class NeurolucidaClassicImporter
{
public:
	bool Import(Input& rIn, Neuron3D &nrn, const HintOptions& options);

private:
	virtual bool ReadHeader(Input& rIn, NCHeaderDetails& details) = 0;
	virtual bool ReadSample(Input& rIn, u32& major, u32&minor, SamplePoint& sample) = 0;
};


class NeurolucidaClassicExporter
{
public:
	bool Export(Output& rOut, const Neuron3D &nrn, const StyleOptions& options);

private:
	virtual void WriteHeader(Output& rOut, const NCHeaderDetails& details) = 0;
	virtual void WriteSample(Output& rOut, u32 major, u32 minor, const SamplePoint& sample) = 0;

	void WriteAxons(Output& rOut, const Neuron3D &nrn);
	void WriteDendrites(Output& rOut, const Neuron3D &nrn);
	void WriteBranch(Output& rOut, Neuron3D::DendriteTreeConstIterator it, u32 dendriteId);
	void WriteSomas(Output& rOut, const Neuron3D &nrn);

};

#endif // _NEUROLUCIDACLASSICHELPERS_HPP_


