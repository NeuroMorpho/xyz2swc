//
// conversion.cpp
//


//
// neuron data file formats are usually physical
// 


//
//
// sometimes we might want to convert to electrotonic units
//
// e.g. equivalent cable construction relies on electrotonic form
//
//
//  might as well be reversible operation....
//
//

#include "Neuron3D.h"
#include "Neuron2D.h"

#include "math.h"


// simple
struct branch_phys2elec_simple
{
	float m_gm, m_rho_i;

	branch_phys2elec_simple(float _gm, float _rho_i) : m_gm(_gm), m_rho_i(_rho_i)
	{
	}

	void operator() (Branch3D &b)
	{
		float e_distance = 0.0f;

		std::vector<SamplePoint>::iterator s = b.m_samples.begin();

		float xlast = (*s).x;
		float ylast = (*s).y;
		float zlast = (*s).z;
		float incx, incy, incz;

		++s;

		for( ;s!=b.m_samples.end();++s)
		{
			incx = (*s).x - xlast;
			incy = (*s).y - ylast;
			incz = (*s).z - zlast;

			float length = (float) sqrt(	(incx * incx) +
									(incy * incy) +
									(incz * incz) );

			float lambda = (float) sqrt( (*s).d / (4.0 * m_gm * m_rho_i) ) ;

			xlast = (*s).x;
			ylast = (*s).y;
			zlast = (*s).z;

			(*s).x = e_distance + (incx / lambda);
			(*s).y = e_distance + (incy / lambda);
			(*s).z = e_distance + (incz / lambda);

			e_distance += length;
		}
	}
};


struct branch_elec2phys_simple
{
	float m_gm, m_rho_i;

	branch_elec2phys_simple(float _gm, float _rho_i) : m_gm(_gm), m_rho_i(_rho_i)
	{
	}

	void operator() (Branch3D &b)
	{
		float distance = 0.0f;

		std::vector<SamplePoint>::iterator s = b.m_samples.begin();

		float xlast = (*s).x;
		float ylast = (*s).y;
		float zlast = (*s).z;
		float incx, incy, incz;

		++s;

		for( ;s!=b.m_samples.end();++s)
		{
			incx = (*s).x - xlast;
			incy = (*s).y - ylast;
			incz = (*s).z - zlast;

			float length = (float) sqrt(	(incx * incx) +
											(incy * incy) +
											(incz * incz) );

			float lambda = (float) sqrt( (*s).d / (4.0 * m_gm * m_rho_i) ) ;

			xlast = (*s).x;
			ylast = (*s).y;
			zlast = (*s).z;

			(*s).x = distance + (incx * lambda);
			(*s).y = distance + (incy * lambda);
			(*s).z = distance + (incz * lambda);

			distance += length;
		}
	}
};


void Physical2Electrotonic( Neuron3D &dst, Neuron3D &src)
{
	dst = src;
//	dst.DoForAllTreeElements( branch_phys2elec_simple (1.0, 1.0) );
}

void Electrotonic2Physical( Neuron3D &dst, Neuron3D &src)
{
	dst = src;
//	dst.DoForAllDendriteElements( branch_elec2phys_simple(1.0, 1.0) );
}


// AXIAL
struct branch_phys2elec_axial
{
	float m_gm, m_rho_i;

	branch_phys2elec_axial(float _gm, float _rho_i) : m_gm(_gm), m_rho_i(_rho_i)
	{
	}

	void operator () (Branch2D &b)
	{
		float e_distance = 0.0f;

		PIT s = b.m_samples.begin();

		float xlast = (*s).x;
		float incx;

		++s;

		for( ;s!=b.m_samples.end();++s)
		{
			incx = (*s).x - xlast;

			float lambda = (float) sqrt( (*s).d / (4.0 * m_gm * m_rho_i ) ) ;

			xlast = (*s).x;

			e_distance += (incx / lambda);

			(*s).x = e_distance;
		}

	}
};

struct branch_elec2phys_axial
{
	float m_gm, m_rho_i;

	branch_elec2phys_axial(float _gm, float _rho_i) : m_gm(_gm), m_rho_i(_rho_i)
	{
	}

	void operator() (Branch2D &b)
	{
		float p_distance = 0.0f;

		PIT s = b.m_samples.begin();

		float xlast = (*s).x;
		float incx;

		++s;

		for( ;s!=b.m_samples.end();++s)
		{
			incx = (*s).x - xlast;

			float lambda = (float) sqrt( (*s).d / (4.0 * m_gm * m_rho_i ) ) ;

			xlast = (*s).x;

			p_distance += (incx * lambda);

			(*s).x = p_distance;
		}
	}

};


void Physical2Electrotonic( Neuron2D &dst, Neuron2D &src)
{
	dst = src;
	//dst.DoForAllDendriteElements( branch_phys2elec_axial (1.0, 1.0) );
}

void Electrotonic2Physical( Neuron2D &dst, Neuron2D &src)
{
	dst = src;
	//dst.DoForAllDendriteElements( branch_elec2phys_axial(1.0, 1.0) );
}
