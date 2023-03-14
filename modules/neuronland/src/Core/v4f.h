#ifndef __CORE_V4F_H__NL__
#define __CORE_V4F_H__NL__

#include <math.h>
#include <algorithm>

/** 

	Simple 4-vector class for use in conjunction with m44f.

	Could be made more general by adding additional functionality.

	Not optimised particularly.

*/

class v4f
{
public:

	// constructors
							v4f ();
							v4f (float _x, float _y, float _z, float _w);

	// setters/getters
	float 					GetX() const;
	float 					GetY() const;
	float 					GetZ() const;
	float 					GetW() const;

	void 					SetX(float);
	void 					SetY(float);
	void 					SetZ(float);
	void 					SetW(float);

	// operators
	v4f	operator+	(const v4f& rhs) const;
	void		operator+=	(const v4f& rhs);

	v4f	operator-	(const v4f& rhs) const;
	void		operator-=	(const v4f& rhs);

	v4f	operator*	(const v4f& rhs) const;
	void		operator*=	(const v4f& rhs);

	v4f	operator*	(float scale) const;
	void		operator*=	(float scale);

	v4f	operator/	(float scale) const;
	void		operator/=	(float scale);

	// properties
	float		GetLength() const;

	// basic trigonometry (should append Get())
	float		dot3		(const v4f& v) const;		///< 3vector dot product
	float		dot4		(const v4f& v) const;		///< 4vector dot product
	v4f			cross		(const v4f& v) const;		///< 3vector cross product
	v4f			Normalised() const;

	// modifiers
	void		Normalise();

	void max(const v4f& v)
	{
		x = std::max(x, v.GetX());
		y = std::max(y, v.GetY());
		z = std::max(z, v.GetZ());
		w = std::max(w, v.GetW());
	}

	void min(const v4f& v)
	{
		x = std::min(x, v.GetX());
		y = std::min(y, v.GetY());
		z = std::min(z, v.GetZ());
		w = std::min(w, v.GetW());
	}

	//
	static v4f			Xaxis();		///< 1.0f,0.0f,0.0f,0.0f
	static v4f			Yaxis();		///< 0.0f,1.0f,0.0f,0.0f
	static v4f			Zaxis();		///< 0.0f,0.0f,1.0f,0.0f
	static v4f			Waxis();		///< 0.0f,0.0f,0.0f,1.0f
	static v4f			Zero();			///< 0.0f,0.0f,0.0f,0.0f

private:
	float x, y, z, w;
};

// non-members
v4f operator*	(float lhs, const v4f& rhs);
v4f operator+	(const v4f& v);
v4f operator-	(const v4f& v);



// inlines

// constructors
inline v4f::v4f()	{ }

inline v4f::v4f(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
{
}

// setters/getters
inline float v4f::GetX() const	{ return x; }
inline float v4f::GetY() const	{ return y; }
inline float v4f::GetZ() const	{ return z; }
inline float v4f::GetW() const	{ return w; }

inline void v4f::SetX(float _x) { x = _x; }
inline void v4f::SetY(float _y) { y = _y; }
inline void v4f::SetZ(float _z) { z = _z; }
inline void v4f::SetW(float _w) { w = _w; }

// operators

inline v4f v4f::operator+(const v4f& rhs) const 
{ 
	return v4f(GetX() + rhs.GetX(), 
					GetY() + rhs.GetY(), 
					GetZ() + rhs.GetZ(), 
					GetW() + rhs.GetW());
}

inline void v4f::operator+=(const v4f& rhs) 
{
	(*this) = (*this) + rhs;
}

inline v4f v4f::operator-(const v4f& rhs) const 
{ 
	return v4f(GetX() - rhs.GetX(),
					GetY() - rhs.GetY(),
					GetZ() - rhs.GetZ(),
					GetW() - rhs.GetW() );
}

inline void v4f::operator-=(const v4f& rhs) 
{ 
	(*this) = (*this) - rhs;
}

inline v4f v4f::operator*(const v4f& rhs) const 
{ 
	return v4f(	GetX() * rhs.GetX(),
				GetY() * rhs.GetY(),
				GetZ() * rhs.GetZ(),
				GetW() * rhs.GetW() );
}

inline void v4f::operator*=(const v4f& rhs) 
{ 
	(*this) = (*this) * rhs;
}

inline v4f v4f::operator*(float scale) const 
{ 
	return v4f(	GetX() * scale, 
				GetY() * scale, 
				GetZ() * scale, 
				GetW() * scale);
}

inline void v4f::operator*=(float scale) 
{
	(*this) = (*this) * scale;
}

inline v4f v4f::operator/(float scale) const
{
	v4f result = *this;
	result *= 1.0f/scale;
	return result;
}

inline void v4f::operator/=(float scale)
{
	*this *= 1.0f/scale;
}

// properties
inline float v4f::GetLength() const
{
	return sqrtf( (*this).dot3(*this) );
}

inline v4f v4f::Normalised() const
{
	return (*this)/GetLength();
}

inline void v4f::Normalise()
{
	(*this) = (*this).Normalised();
}


// trig
inline float v4f::dot3(const v4f& v) const
{
	const float x1 = GetX();
	const float y1 = GetY();
	const float z1 = GetZ();
	const float x2 = v.GetX();
	const float y2 = v.GetY();
	const float z2 = v.GetZ();

	return x1*x2 + y1*y2 + z1*z2;
}

inline float v4f::dot4(const v4f& v) const
{
	const float w1 = GetW();
	const float w2 = v.GetW();
	return dot3(v) + w1*w2;
}

inline v4f v4f::cross(const v4f& v) const
{
	const float x1	= GetX();
	const float y1	= GetY();
	const float z1	= GetZ();
	const float x2	= v.GetX();
	const float y2	= v.GetY();
	const float z2	= v.GetZ();

	return v4f( y1*z2 - z1*y2,
				z1*x2 - x1*z2,
				x1*y2 - y1*x2,
				GetW());
}

// non-members
inline v4f operator*(float lhs, const v4f& rhs)
{
	return (rhs * lhs);
}

inline v4f operator+(const v4f& v)
{
	return v;
}

inline v4f operator-(const v4f& v)
{
	return v4f(-v.GetX(), -v.GetY(), -v.GetZ(), -v.GetW());
}

inline v4f v4f::Xaxis() { return v4f(1.0f, 0.0f, 0.0f, 0.0f); }
inline v4f v4f::Yaxis() { return v4f(0.0f, 1.0f, 0.0f, 0.0f); }
inline v4f v4f::Zaxis() { return v4f(0.0f, 0.0f, 1.0f, 0.0f); }
inline v4f v4f::Waxis() { return v4f(0.0f, 0.0f, 0.0f, 1.0f); }
inline v4f v4f::Zero() { return v4f(0.0f, 0.0f, 0.0f, 0.0f); }

#endif // __CORE_V4F_H__NL__

