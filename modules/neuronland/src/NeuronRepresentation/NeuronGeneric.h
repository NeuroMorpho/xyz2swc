#ifndef NEURONGENERIC_HPP_
#define NEURONGENERIC_HPP_

#include <list>
#include <vector>
#include <algorithm>
#include <functional>

#include "Core/cptree.h"

#include "MarkerPoint.h"
#include "NeuralTree.h"

/** 

	@brief	Provides the morphology/connectivity framework for the main elements of a neural structure, 
			with template parameters allowing customisation for specific uses.

**/

template<typename D, typename S=D, typename A=D> 
class NeuronGeneric
{
public:

	typedef S											SomaElement;
	typedef std::list<S>								SomaList;
	typedef typename SomaList::iterator					SomaIterator;
	typedef typename SomaList::const_iterator			SomaConstIterator;

	typedef A											AxonElement;
	typedef NeuralTree<A>								AxonTree;
	typedef typename NeuralTree<A>::iterator			AxonTreeIterator;
	typedef typename NeuralTree<A>::const_iterator		AxonTreeConstIterator;
	typedef std::list<AxonTree>							AxonList;
	typedef typename AxonList::iterator					AxonIterator;
	typedef typename AxonList::const_iterator			AxonConstIterator;

	typedef D											DendriteElement;
	typedef NeuralTree<D>								DendriteTree;
	typedef typename NeuralTree<D>::iterator			DendriteTreeIterator;
	typedef typename NeuralTree<D>::const_iterator		DendriteTreeConstIterator;
	typedef std::list<DendriteTree>						DendriteList;
	typedef typename DendriteList::iterator				DendriteIterator;
	typedef typename DendriteList::const_iterator		DendriteConstIterator;



														NeuronGeneric();

	DendriteIterator									CreateDendrite(bool front = false);
	AxonIterator										CreateAxon(bool front = false);
	SomaIterator										CreateSoma();

	DendriteIterator									DeleteDendrite(DendriteIterator it);
	AxonIterator										DeleteAxon(AxonIterator it);
	SomaIterator										DeleteSoma(SomaIterator it);

	void												DeleteAllSomas();
	void												DeleteAllDendrites();
	void												DeleteAllAxons();

	DendriteConstIterator								DendriteBegin() const;
	DendriteIterator									DendriteBegin();
	DendriteConstIterator								DendriteEnd() const;
	DendriteIterator									DendriteEnd();

	AxonConstIterator									AxonBegin() const;
	AxonIterator										AxonBegin();
	AxonConstIterator									AxonEnd() const;
	AxonIterator										AxonEnd();

	SomaConstIterator									SomaBegin() const;
	SomaIterator										SomaBegin();
	SomaConstIterator									SomaEnd() const;
	SomaIterator										SomaEnd();

	u32 												CountSomas() const;
	u32 												CountAxons() const;
	u32 												CountDendrites() const;

	u32													CountAxonElements() const;
	u32													CountDendriteElements() const;

	void												MoveDendrite(DendriteIterator toMove, DendriteIterator aheadOf);
	void												MoveAxon(AxonIterator toMove, AxonIterator aheadOf);

	void												CreateDendriteCopy(DendriteConstIterator it);
	void												CreateAxonCopy(AxonConstIterator it);
	void												CreateSomaCopy(SomaConstIterator it);

	void												CopyMergeNeuron(const NeuronGeneric<D,S,A>& nrn);
	

	template<class V>
	u32 CountBranchElementsFromRoot(typename NeuralTree<V>::const_iterator it) const
	{
		return NeuralTree<V>::size(it);
	}

	template<class unaryfunc>
	void DoForAllDendrites(unaryfunc& f) const
	{
		DendriteConstIterator it = m_dendrites.begin();
		DendriteConstIterator end = m_dendrites.end();

		for(;it!=end;++it)
			f(it);
	}

	template<class unaryfunc>
	void DoForAllAxons(unaryfunc& f) const
	{
		AxonConstIterator it = m_axons.begin();
		AxonConstIterator end = m_axons.end();

		for(;it!=end;++it)
			f(it);
	}

	template<class unaryfunc>
	void DoForAllTrees(unaryfunc& f)
	{
		DoForAllDendrites(f);
		DoForAllAxons(f);
	}

	template<class unaryfunc>
	void DoForAllDendrites(unaryfunc& f)
	{
		DendriteIterator it = m_dendrites.begin();
		DendriteIterator end = m_dendrites.end();

		for(;it!=end;++it)
			f(it);
	}

	template<class unaryfunc>
	void DoForAllAxons(unaryfunc& f)
	{
		AxonIterator it = m_axons.begin();
		AxonIterator end = m_axons.end();

		for(;it!=end;++it)
			f(it);
	}

	template<class unaryfunc>
	void DoForAllTrees(unaryfunc& f) const
	{
		DoForAllDendrites(f);
		DoForAllAxons(f);
	}

	template<class unaryfunc>
	void DoForAllDendriteElements(unaryfunc& f)
	{
		for(DendriteIterator it = DendriteBegin();it != DendriteEnd();++it)
			DendriteTree::recurse_it( (*it).root(), f);
	}

	template<class unaryfunc>
	void DoForAllDendriteElements(unaryfunc& f) const
	{
		for(DendriteConstIterator it = DendriteBegin();it != DendriteEnd();++it)
			DendriteTree::recurse_it( (*it).root(), f);
	}

	template<class unaryfunc>
	void DoForDendriteElements(DendriteTreeConstIterator it, unaryfunc f) const
	{
		DendriteTree::recurse_it(it, f);
	}

	template<class unaryfunc>
	void DoForAllAxonElements(unaryfunc& f)
	{
		for(AxonIterator it = AxonBegin();it != AxonEnd();++it)
			DendriteTree::recurse_it( (*it).root(), f);
	}

	template<class unaryfunc>
	void DoForAllAxonElements(unaryfunc& f) const
	{
		for(AxonConstIterator it = AxonBegin();it != AxonEnd();++it)
			DendriteTree::recurse_it( (*it).root(), f);
	}

	template<class unaryfunc>
	void DoForAxonElements(AxonTreeConstIterator it, unaryfunc& f) const
	{
		AxonTree::recurse_it(it, f);
	}

	template<class unaryfunc>
	void DoForAllTreeElements(unaryfunc& f)
	{
		DoForAllDendriteElements(f);
		DoForAllAxonElements(f);
	}

	template<class unaryfunc>
	void DoForAllTreeElements(unaryfunc& f) const
	{
		DoForAllDendriteElements(f);
		DoForAllAxonElements(f);
	}

	template<class unaryfunc>
	void DoForAllSomas(unaryfunc& f) const
	{
		SomaConstIterator it = m_somas.begin();
		SomaConstIterator end = m_somas.end();

		for(;it!=end;++it)
			f(it);
	}

	template<class unaryfunc>
	void DoForAllSomas(unaryfunc& f)
	{
		SomaIterator it = m_somas.begin();
		SomaIterator end = m_somas.end();

		for(;it!=end;++it)
			f(it);
	}

	DendriteIterator ChangeDendriteToAxon(DendriteIterator it)
	{
		m_axons.push_back(*it);
		return m_dendrites.erase(it);
	}

	AxonIterator ChangeAxonToDendrite(AxonIterator it)
	{		
		m_dendrites.push_back(*it);
		return m_axons.erase(it);
	}

	void Clear()
	{
		for_each(m_somas.begin(), m_somas.end(), std::mem_fun_ref(&S::clear) );
		for_each(m_axons.begin(), m_axons.end(), std::mem_fun_ref(&NeuralTree<A>::clear) );
		for_each(m_dendrites.begin(), m_dendrites.end(), std::mem_fun_ref(&NeuralTree<D>::clear) );

		m_somas.clear();
		m_axons.clear();
		m_dendrites.clear();
	}

private:
	SomaList						m_somas;
	AxonList						m_axons;
	DendriteList					m_dendrites;
};


// inlines

template<typename D, typename S, typename A> 
inline NeuronGeneric<D,S,A>::NeuronGeneric()
{
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::DendriteIterator NeuronGeneric<D,S,A>::CreateDendrite(bool front /*= false*/)
{
	if(front)
		return m_dendrites.insert(m_dendrites.begin(), DendriteTree() );
	else
		return m_dendrites.insert(m_dendrites.end(), DendriteTree() );
}
template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::AxonIterator	NeuronGeneric<D,S,A>::CreateAxon(bool front /*= false*/)
{
	if(front)
		return m_axons.insert(m_axons.begin(), AxonTree() );
	else
		return m_axons.insert(m_axons.end(), AxonTree() );
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::SomaIterator	NeuronGeneric<D,S,A>::CreateSoma()
{
	return m_somas.insert(m_somas.end(), SomaElement() );
}

template<typename D, typename S, typename A>
inline typename NeuronGeneric<D,S,A>::DendriteIterator NeuronGeneric<D,S,A>::DeleteDendrite(DendriteIterator it)
{
	return m_dendrites.erase(it);
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::AxonIterator NeuronGeneric<D,S,A>::DeleteAxon(AxonIterator it)
{
	return m_axons.erase(it);
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::SomaIterator NeuronGeneric<D,S,A>::DeleteSoma(SomaIterator it)
{
	return m_somas.erase(it);
}

template<typename D, typename S, typename A> 
inline void NeuronGeneric<D,S,A>::DeleteAllSomas()
{
	m_somas.clear();
}

template<typename D, typename S, typename A> 
inline void NeuronGeneric<D,S,A>::DeleteAllDendrites()
{
	m_dendrites.clear();
}

template<typename D, typename S, typename A> 
inline void NeuronGeneric<D,S,A>::DeleteAllAxons()
{
	m_axons.clear();
}

template<typename D, typename S, typename A>
inline typename NeuronGeneric<D,S,A>::DendriteIterator	NeuronGeneric<D,S,A>::DendriteBegin()
{
	return m_dendrites.begin();
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::DendriteConstIterator NeuronGeneric<D,S,A>::DendriteBegin() const
{
	return m_dendrites.begin();
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::DendriteIterator NeuronGeneric<D,S,A>::DendriteEnd()
{
	return m_dendrites.end();
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::DendriteConstIterator NeuronGeneric<D,S,A>::DendriteEnd() const
{
	return m_dendrites.end();
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::AxonIterator NeuronGeneric<D,S,A>::AxonBegin()
{	
	return m_axons.begin();
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::AxonConstIterator NeuronGeneric<D,S,A>::AxonBegin() const
{
	return m_axons.begin();
}

template<typename D, typename S, typename A>
inline typename NeuronGeneric<D,S,A>::AxonIterator NeuronGeneric<D,S,A>::AxonEnd()
{
	return m_axons.end();
}

template<typename D, typename S, typename A>
inline typename NeuronGeneric<D,S,A>::AxonConstIterator NeuronGeneric<D,S,A>::AxonEnd() const
{
	return m_axons.end();
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::SomaIterator NeuronGeneric<D,S,A>::SomaBegin()
{
	return m_somas.begin();
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::SomaConstIterator NeuronGeneric<D,S,A>::SomaBegin() const
{
	return m_somas.begin();
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::SomaIterator NeuronGeneric<D,S,A>::SomaEnd()
{
	return m_somas.end();
}

template<typename D, typename S, typename A> 
inline typename NeuronGeneric<D,S,A>::SomaConstIterator NeuronGeneric<D,S,A>::SomaEnd() const
{
	return m_somas.end();
}

template<typename D, typename S, typename A> 
inline u32 NeuronGeneric<D,S,A>::CountSomas() const
{
	return (u32) m_somas.size();
}

template<typename D, typename S, typename A> 
inline u32 NeuronGeneric<D,S,A>::CountAxons() const
{
	return (u32) m_axons.size();
}

template<typename D, typename S, typename A> 
inline u32 NeuronGeneric<D,S,A>::CountDendrites() const
{
	return (u32) m_dendrites.size();
}

template<typename D, typename S, typename A> 
inline u32 NeuronGeneric<D,S,A>::CountAxonElements() const
{
	typename AxonTree::BranchCounter c;
	DoForAllAxonElements( c );
	return c.GetCount();
}

template<typename D, typename S, typename A>
inline u32 NeuronGeneric<D,S,A>::CountDendriteElements() const
{
	typename DendriteTree::BranchCounter c;
	DoForAllDendriteElements( c );
	return c.GetCount();
}

template<typename D, typename S, typename A>
void NeuronGeneric<D,S,A>::MoveDendrite(DendriteIterator aheadOf, DendriteIterator toMove)
{
	m_dendrites.splice(aheadOf, m_dendrites, toMove);
}

template<typename D, typename S, typename A>
void NeuronGeneric<D,S,A>::MoveAxon(AxonIterator toMove, AxonIterator aheadOf)
{
	m_axons.splice(aheadOf, m_axons, toMove);
}

template<typename D, typename S, typename A>
void NeuronGeneric<D,S,A>::CreateDendriteCopy(DendriteConstIterator it)
{
	m_dendrites.push_back(*it);
}

template<typename D, typename S, typename A>
void NeuronGeneric<D,S,A>::CreateAxonCopy(AxonConstIterator it)
{
	m_axons.push_back(*it);
}

template<typename D, typename S, typename A>
void NeuronGeneric<D,S,A>::CreateSomaCopy(SomaConstIterator it)
{
	m_somas.push_back(*it);
}

template<typename D, typename S, typename A>
void NeuronGeneric<D,S,A>::CopyMergeNeuron(const NeuronGeneric<D,S,A>& nrn)
{
	for(SomaConstIterator sit = nrn.SomaBegin(); sit != nrn.SomaEnd(); ++sit)
		CreateSomaCopy(sit);

	for(DendriteConstIterator dit = nrn.DendriteBegin(); dit != nrn.DendriteEnd(); ++dit)
		CreateDendriteCopy(dit);

	for(AxonConstIterator ait = nrn.AxonBegin(); ait != nrn.AxonEnd(); ++ait)
		CreateAxonCopy(ait);
}


#endif // NEURONGENERIC_HPP_
