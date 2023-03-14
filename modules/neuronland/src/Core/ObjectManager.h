#ifndef __CORE_OBJECTMANAGER_H__NL__
#define __CORE_OBJECTMANAGER_H__NL__

#include <map>

template<typename T, typename IDGEN>
class ObjectManager
{
public:
	typedef typename IDGEN::id_type		id_type;

	ObjectManager() {}
	virtual 			~ObjectManager(){}

	id_type				Create()
	{
		return Insert( new T() );
	}

	id_type				Insert(T* pObject)
	{
		id_type id = mIdGenerator.GetId(); 
		if(id != IDGEN::bad_id)
			mObjects[id] = pObject;
		return id;
	}

	void				Remove(id_type id)
	{

		typename std::map<id_type, T*>::iterator it = mObjects.find(id);

		if(it != mObjects.end())
		{
			delete it->second;
			mObjects.erase(it);		
			mIdGenerator.FreeId(id) ;
		}
	}

	T*	GetObject(id_type id)
	{
		typename std::map<id_type, T*>::iterator it = mObjects.find(id);
		if(it != mObjects.end())
		{
			return (*it).second;
		}

		return 0;
	}

	T*	GetObject(id_type id) const
	{
		typename std::map<id_type, T*>::const_iterator it = mObjects.find(id);
		if(it != mObjects.end())
		{
			return (*it).second;
		}

		return 0;
	}

private:
	std::map<id_type, T*>	mObjects;
	IDGEN					mIdGenerator;
};

#endif // __CORE_OBJECTMANAGER_H__NL__
