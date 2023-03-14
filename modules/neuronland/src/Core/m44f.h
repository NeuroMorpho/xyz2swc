#ifndef __CORE_M44F_H__NL__
#define __CORE_M44F_H__NL__

#include "Core/v4f.h"

/**

	Simple matrix class, providing enough functionality to simplify some operations

	Basically, some matrix multiplication functionality, and some other useful odds and ends

	Could be expanded for more general use.

*/

class m44f
{
public:
	// constructors
				m44f();
				m44f(const v4f& Xaxis, const v4f& Yaxis, const v4f& Zaxis, const v4f& trans);
				m44f(const v4f& direction, float angle);

	// presets
	static m44f Identity();
	static m44f Zero();
	static m44f Diagonal(const v4f& v);

	// operators
	void		operator=	(const m44f& m);

	v4f	operator*	(const v4f& v) const;		///< == 4x4matrix x 4vector

	m44f	operator*	(const m44f& m) const;	///< 4x4matrix x 4x4matrix
	void		operator*=	(const m44f& m);		///<

	// manipulators
	void		TranslateBy	(const v4f& v);
	void		OrthonormaliseX();

	// manipulated
	m44f	GetTranspose() const;

//private:
	v4f 	mXaxis;
	v4f 	mYaxis;
	v4f 	mZaxis;
	v4f 	mTranslation;
};


// inlines

inline m44f::m44f()
{
}

inline m44f::m44f(const v4f& x, const v4f& y, const v4f& z, const v4f& t)
	: mXaxis(x), mYaxis(y), mZaxis(z), mTranslation(t)
{
}


inline void m44f::operator= (const m44f& m)
{
	mXaxis	= m.mXaxis;
	mYaxis	= m.mYaxis;
	mZaxis	= m.mZaxis;
	mTranslation	= m.mTranslation;
}

inline v4f m44f::operator* (const v4f& v) const
{
	const m44f transpose = GetTranspose();

	return v4f(	transpose.mXaxis.dot4(v),
				transpose.mYaxis.dot4(v),
				transpose.mZaxis.dot4(v),
				transpose.mTranslation.dot4(v));
}

inline m44f m44f::operator* (const m44f& rhs) const
{
	return m44f((*this) * rhs.mXaxis,
				(*this) * rhs.mYaxis,
				(*this) * rhs.mZaxis,
				(*this) * rhs.mTranslation );
}

inline void m44f::operator*= (const m44f& m)
{
	(*this) = (*this) * m;
}


// manipulators
inline void m44f::TranslateBy(const v4f& v)
{
//	assert(v.getW() == 0.0f);
	mTranslation += v;
}

#endif // __CORE_M44F_H__NL__
