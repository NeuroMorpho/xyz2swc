#ifndef __SIMPLEINCREMENTIDGENERATOR__H__NL__
#define __SIMPLEINCREMENTIDGENERATOR__H__NL__


class SimpleIncrementIdGenerator
{
public:
	typedef int id_type;

	static const id_type bad_id = -1;

	SimpleIncrementIdGenerator() : mId(0) {}
	id_type GetId() { return mId++; }
	void FreeId(id_type id) {}

private:
	id_type mId;
};


#endif // __SIMPLEINCREMENTIDGENERATOR__H__NL__