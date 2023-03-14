#ifndef MORPHOLOGYHANDLERID_HPP_
#define MORPHOLOGYHANDLERID_HPP_

#include "Core/HashBasedID.h"

/**

@brief Unique ID for Morphology Format

**/

class MorphologyHandlerID : public HashBasedID
{
public:	
	MorphologyHandlerID();
	MorphologyHandlerID(HString name);
	MorphologyHandlerID& operator=(const MorphologyHandlerID& rhs);
};


// inlines
inline MorphologyHandlerID::MorphologyHandlerID() 
{
}

inline MorphologyHandlerID::MorphologyHandlerID(HString name) : HashBasedID(name) 
{
}

inline MorphologyHandlerID& MorphologyHandlerID::operator=(const MorphologyHandlerID& rhs) 
{ 
	return (MorphologyHandlerID&)HashBasedID::operator =(rhs); 
}


#endif // MORPHOLOGYHANDLERID_HPP_
