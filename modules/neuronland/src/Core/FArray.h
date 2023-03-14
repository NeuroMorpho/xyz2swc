#ifndef __CORE_FARRAY_H__NL__
#define __CORE_FARRAY_H__NL__

template <class T, unsigned int SIZE>
class FArray
{
	T mEntries[SIZE];

public:
	inline FArray()					{}

	inline const T& operator[](unsigned int index) const
	{
#if defined(_DEBUG)
		assert(index < SIZE);
		if(index >= SIZE)
			fatal("FArray index out of bounds");
#endif
		return mEntries[index];
	}

	inline T& operator[](unsigned int index)
	{
#if defined(_DEBUG)
		assert(index < SIZE);
		if(index >= SIZE)
			fatal("FArray index out of bounds");
#endif
		return mEntries[index];
	}

	inline unsigned int size() const
	{
		return SIZE;
	}

	typedef T* iterator;
	typedef const T* const_iterator;
	
	const_iterator begin() const
	{
		return &mEntries[0];
	}

	iterator begin()
	{
		return &mEntries[0];
	}

	const_iterator end() const
	{
		return &mEntries[SIZE];
	}
};



#endif	// __CORE_FARRAY_H__NL__
