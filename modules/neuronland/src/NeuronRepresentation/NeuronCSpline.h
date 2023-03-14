#ifndef __NEURON_CSPLINE__H__TWIG__
#define __NEURON_CSPLINE__H__TWIG__

#include <vector>

#include "NeuronGeneric.h"

struct csplinePoint
{
	double x;
	double spl_coeff[5];
};

class branch_cspline
{
public:
	Branch2D();
	~Branch2D();

	vector<csplinePoint> m_ positions;
	vector<MarkerPoint>  m_markers;
};


class neuron_cspline : public NeuronGeneric<branch_cspline>
{
public:
	neuron_cspline(const class Neuron3D &);
	~neuron_cspline();
};


#endif // __NEURON_CSPLINE__H__TWIG__

