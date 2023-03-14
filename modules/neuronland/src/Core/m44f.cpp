//
// m44f.cpp
//

#include <math.h>

#include "Core/m44f.h"


/** 

	Given unit vector (x,y,z), and angle A to rotate about vector direction, the 3x3 rotation matrix is given by -

		(x-axis)				(y-axis)					(z-axis)

	cosA + (1-cosA)x^2		(1-cosA)xy - (sinA)z		(1-cosA)xz + (sinA)y
	(1-cosA)yx + (sinA)z	cosA + (1-cosA)y^2			(1-cosA)yz - (sinA)x
	(1-cosA)zx - (sinA)y	(1-cosA)zy + (sinA)x		cosA + (1-cosA)z^2

*/

m44f::m44f(const v4f& dir, float angle)
{
	const float x				= dir.GetX();
	const float y				= dir.GetY();
	const float z				= dir.GetZ();
	const float xsq				= x*x;
	const float ysq				= y*y;
	const float zsq				= z*z;
	const float xy				= x*y;
	const float xz				= x*z;
	const float yz				= y*z;
	const float cosA			= cosf(angle);
	const float sinA			= sinf(angle);
	const float OneMinusCosA	= 1.0f - cosA;
	const float xsinA			= x*sinA;
	const float ysinA			= y*sinA;
	const float zsinA			= z*sinA;
	const float xyOneMinusCosA	= xy*OneMinusCosA;
	const float xzOneMinusCosA	= xz*OneMinusCosA;
	const float yzOneMinusCosA	= yz*OneMinusCosA;

	mXaxis = v4f(	cosA + OneMinusCosA*xsq,	xyOneMinusCosA + zsinA,		xzOneMinusCosA - ysinA,		0.0f	);
	mYaxis = v4f(	xyOneMinusCosA - zsinA,		cosA + OneMinusCosA*ysq,	yzOneMinusCosA + xsinA,		0.0f	);
	mZaxis = v4f(	xzOneMinusCosA + ysinA,		yzOneMinusCosA - xsinA,		cosA + OneMinusCosA*zsq,	0.0f	);
	mTranslation = v4f::Waxis();

#ifdef _DEBUG
	m44f copy = (*this);
	copy.OrthonormaliseX();

	float divergence =	(copy.mXaxis - mXaxis).GetLength() + 
						(copy.mYaxis - mYaxis).GetLength() + 
						(copy.mZaxis - mZaxis).GetLength();

#endif
}

m44f m44f::Identity()
{
	return m44f(v4f::Xaxis(),
				v4f::Yaxis(),
				v4f::Zaxis(),
				v4f::Waxis() );
}

m44f m44f::Zero()
{
	const v4f zero = v4f::Zero();
	return m44f(zero, zero, zero, zero);
}

m44f m44f::Diagonal(const v4f& v)
{
	m44f m( m44f::Identity() );

	m.mXaxis *= v.GetX();
	m.mYaxis *= v.GetY();
	m.mZaxis *= v.GetZ();

	return m;
}


m44f m44f::GetTranspose() const
{
	m44f transpose;

	transpose.mXaxis = v4f(mXaxis.GetX(), mYaxis.GetX(), mZaxis.GetX(), mTranslation.GetX());
	transpose.mYaxis = v4f(mXaxis.GetY(), mYaxis.GetY(), mZaxis.GetY(), mTranslation.GetY());
	transpose.mZaxis = v4f(mXaxis.GetZ(), mYaxis.GetZ(), mZaxis.GetZ(), mTranslation.GetZ());
	transpose.mTranslation = v4f(mXaxis.GetW(), mYaxis.GetW(), mZaxis.GetW(), mTranslation.GetW());

	return transpose;
}

void m44f::OrthonormaliseX()
{
	mXaxis.Normalise();

	const float dot = mXaxis.dot3(mYaxis);
	const v4f temp(mXaxis * dot);
	mYaxis -= temp;
	mYaxis.Normalise();

	const float zw = mZaxis.GetW();
	mZaxis = mXaxis.cross(mYaxis);
	mZaxis.SetW(zw);
}
