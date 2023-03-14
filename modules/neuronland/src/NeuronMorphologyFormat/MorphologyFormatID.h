#ifndef MORPHOLOGYFORMATID_HPP_
#define MORPHOLOGYFORMATID_HPP_

#include "Core/HashBasedID.h"

/**

	@brief Unique ID for Morphology Format

**/

class MorphologyFormatID : public HashBasedID
{
public:	
	MorphologyFormatID();
	MorphologyFormatID(HString name);
	MorphologyFormatID& operator=(const MorphologyFormatID& id);
	bool				operator== (MorphologyFormatID rhs) const;
};


// inlines
inline MorphologyFormatID::MorphologyFormatID() 
{
}

inline MorphologyFormatID::MorphologyFormatID(HString name) : HashBasedID(name) 
{
}

inline MorphologyFormatID& MorphologyFormatID::operator=(const MorphologyFormatID& rhs) 
{ 
	return (MorphologyFormatID&)HashBasedID::operator =(rhs); 
}

inline bool MorphologyFormatID::operator== (MorphologyFormatID rhs) const
{
	return HashBasedID::operator ==(rhs); 
}

#endif // MORPHOLOGYFORMATID_HPP_
