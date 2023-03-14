#ifndef NEURON2D_HPP_
#define NEURON2D_HPP_ 

#include <vector>
#include "Core/String.hpp"

#include "NeuronGeneric.h"
#include "Neuron3D.h"

struct AxialPoint
{
	float x;
	float d;

	AxialPoint()
	{
	}

	AxialPoint(float _x, float _d) : x(_x), d(_d)
	{
	}
};

class Branch2D
{
public:
	Branch2D() {}
	~Branch2D() {};

	std::vector<AxialPoint> m_samples;

	int m_nodejunc;
	int m_nodefirst;
};

typedef NeuralTree<Branch2D> dendrite_axial;
typedef NeuralTree<Branch2D> axon_axial;

typedef std::vector<AxialPoint>::iterator PIT;
typedef std::vector<AxialPoint>::const_iterator CPIT;

typedef NeuralTree<Branch2D>::iterator TAIT;
typedef NeuralTree<Branch2D>::const_iterator CTAIT;

typedef std::list< NeuralTree<Branch2D> >::iterator DAIT;
typedef std::list< NeuralTree<Branch2D> >::const_iterator CDAIT;


class Neuron2D : public NeuronGeneric<Branch2D>
{
	void BuildAxial(TAIT root, Neuron3D::DendriteTreeConstIterator src);

public:
	Neuron2D();
	Neuron2D(const class Neuron3D &);
	~Neuron2D();

	double m_cm, m_ga, m_gm, m_gs;
};




#endif // NEURON2D_HPP_
