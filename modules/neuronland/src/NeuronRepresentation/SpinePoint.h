#ifndef SPINEPOINT_HPP_
#define SPINEPOINT_HPP_

#include <math.h>

#include "SamplePoint.h"

enum SpineType
{
	kSpineNoDetail,
	kSpineMushroom,
	kSpineStubby,
	kSpineThin, 
	kSpineContourRoot //
};

struct SpinePoint : public SamplePoint
{
	SpineType type;
	u32 associatedSegment; // offset on associated branch

	SpinePoint() : SamplePoint(0.0f, 0.0f, 0.0f, 0.0f), type(kSpineNoDetail), associatedSegment(0)
	{
	}

	SpinePoint(float _x, float _y, float _z, float _d) : SamplePoint(_x, _y, _z, _d), associatedSegment(0)
	{
	}

	SpinePoint(const SamplePoint& s, SpineType t, u32 seg) : SamplePoint(s), type(t), associatedSegment(seg)
	{
	}

	SpinePoint& operator+=(const SpinePoint& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;

		return (*this);
	}

	bool operator==(const SpinePoint& rhs) const
	{
		return	x == rhs.x &&
				y == rhs.y &&
				z == rhs.z &&
				d == rhs.d &&
				type == rhs.type &&
				associatedSegment == rhs.associatedSegment;
	}

	bool operator!= (const SpinePoint& rhs) const 
	{
		return !operator==(rhs);
	}
};

#endif // SPINEPOINT_HPP_
