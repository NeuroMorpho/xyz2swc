#ifndef SAMPLEPOINT_HPP_
#define SAMPLEPOINT_HPP_

#include <math.h>

struct SamplePoint
{
	float x;
	float y;
	float z;
	float d;

	SamplePoint() : x(0.0f), y(0.0f), z(0.0f),d(0.0f)
	{
	}

	SamplePoint(float _x, float _y, float _z, float _d) : x(_x), y(_y), z(_z), d(_d)
	{
	}

	float Distance(const SamplePoint&sample) const
	{
		const float xdist = sample.x - x;
		const float ydist = sample.y - y;
		const float zdist = sample.z - z;

		return (float) sqrt( xdist*xdist + ydist*ydist + zdist*zdist);
	}

	bool operator==(const SamplePoint& rhs) const
	{
		return	x == rhs.x &&
				y == rhs.y &&
				z == rhs.z &&
				d == rhs.d;
	}

	bool operator!=(const SamplePoint& rhs) const
	{
		return !operator==(rhs);
	}

	SamplePoint& operator+=(const SamplePoint& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;

		return (*this);
	}

	SamplePoint operator+(const SamplePoint& rhs)
	{
		return SamplePoint(x + rhs.x, y+rhs.y, z+rhs.z, d);
	}

	v4f tov4f() const { return v4f(x,y,z,d); }
};

#endif // SAMPLEPOINT_HPP_
