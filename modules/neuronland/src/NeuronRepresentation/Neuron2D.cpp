//
// Neuron2D.cpp
//
#include <math.h>

#include "Core/Debug.h"

#include "Neuron3D.h"
#include "Neuron2D.h"


// neuron-simple to neuron-axial
void Neuron2D::BuildAxial(TAIT root, Neuron3D::DendriteTreeConstIterator src)
{
	Branch2D bran;
	float distance=0.0f;

	std::vector<SamplePoint>::const_iterator p=(*src).m_samples.begin();
	double xlast = (double)(*p).x;
	double ylast = (double)(*p).y;
	double zlast = (double)(*p).z;

	bran.m_samples.push_back( AxialPoint(0.0, (*p).d) );
	++p;

	for(; p != (*src).m_samples.end();++p)
	{
		distance += (float) sqrt(	pow( (*p).x - xlast, 2.0) +
									pow( (*p).y - ylast, 2.0) +
									pow( (*p).z - zlast, 2.0) );

		xlast = (*p).x;
		ylast = (*p).y;
		zlast = (*p).z;

		bran.m_samples.push_back( AxialPoint(distance, (*p).d) );
	}

	TAIT i;
	if(root)
	{
//		i = insert_child_last(root, bran);
	}
	else
	{
//		i = tree.set_root(bran);
	}

//	if(src.child()) BuildAxial( tree, i, src.child() );
//	if(src.peer()) BuildAxial( tree, root, src.peer() );
}


Neuron2D::Neuron2D(const Neuron3D &src)
{
	m_ga = 1.0;
	m_gs = 1.0;
	m_gm = 1.0;
	m_cm = 1.0;

	for(Neuron3D::DendriteConstIterator d=src.DendriteBegin();d !=src.DendriteEnd(); ++d)
	{
		DendriteIterator dit = CreateDendrite();

		assert( !(*dit).root() );

	//	BuildAxial( (*da), (*da).root(), (*d).root() ); 
	}
}

Neuron2D::Neuron2D()
{
	m_ga = 1.0;
	m_gs = 1.0;
	m_gm = 1.0;
	m_cm = 1.0;
}

Neuron2D::~Neuron2D()
{
}

