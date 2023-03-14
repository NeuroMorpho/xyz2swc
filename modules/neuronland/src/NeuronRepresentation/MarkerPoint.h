#ifndef MARKERPOINT_HPP_
#define MARKERPOINT_HPP_

#include "Core/HashBasedID.h"

#include "SamplePoint.h"

typedef HashBasedID MarkerID;

struct MarkerPoint : SamplePoint
{
	// useful for preserving data between Neurolucida formats.
	u32 shape;

	MarkerPoint()
	{
	}

	MarkerPoint(float _x, float _y, float _z, float _d) : SamplePoint(_x, _y, _z, _d), shape(0)
	{
	}
};

#endif // MARKERPOINT_HPP_
