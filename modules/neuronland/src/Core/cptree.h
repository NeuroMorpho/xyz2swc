#ifndef __CORE_CPTREE_H__NL__
#define __CORE_CPTREE_H__NL__

/** 

	@brief cptree:  child-peer N-ary tree container

	@note	Hasn't been much of an attempt to make this STL compliant!

**/

#include <memory>
#include <stdlib.h>

#include "Core/String.hpp"
#include "Core/Debug.h"


template< class T, class A=std::allocator<T> > 
class cptree
{
public:

	typedef T value_type;
	typedef A allocator_type;
	typedef typename A::size_type size_type;
	typedef typename A::difference_type difference_type;

	typedef typename A::pointer pointer;
	typedef typename A::const_pointer const_pointer;
	typedef typename A::reference reference;
	typedef typename A::const_reference const_reference;

	class E
	{
	public:
		E(const T &t) : item(t), m_parent(0), m_child(0), m_peer(0) { }
		~E() { }

	public:
		T item;

	private:
		E *m_parent;
		E *m_child;
		E *m_peer;

	public:
		E* Parent() const { return m_parent; }
		E* Child() const { return m_child; }
		E* Peer()  const { return m_peer; }

		void SetParent(E *parent) { m_parent = parent; }
		void SetChild(E *child) { m_child = child; }
		void SetPeer(E *peer) { m_peer = peer; }
		void SetItem(const T &i) { item = i; }
	};

	// extra funcs for handling destruction of subtrees 
	void destroytree(E *root)
	{
		if(root->Child()) destroytree(root->Child());
		if(root->Peer()) destroytree(root->Peer());
		delete( root );
	}

	class iterator;

	class const_iterator
	{
	public:
		const_iterator(E *p) : ptr(p) {}

	public:
		const_iterator() {}
		const_iterator(const const_iterator &i) : ptr(i.ptr) {}
		const_iterator(const iterator &i) : ptr(i.ptr) {}

		const_reference operator*() const {return ptr->item;}
		value_type *operator->() const {return &( ptr->item ); }
	
		const_iterator child() const
		{
			assert(ptr);
			return const_iterator(ptr->Child());
		}

		const_iterator peer() const
		{
			assert(ptr);
			return const_iterator(ptr->Peer());
		}

		const_iterator parent() const
		{
			assert(ptr);
			return const_iterator(ptr->Parent());
		}

		bool operator==(const const_iterator& _X) const
		{
			return (ptr == _X.ptr);
		}

		bool operator!=(const const_iterator& _X) const
		{
			return (!(*this == _X)); 
		}

		operator bool() const {  return (ptr!=0); }

		E *GetNode() { return ptr; }

		protected:
		E *ptr;
	};

	class iterator
	{
		friend class const_iterator;
		friend class cptree;
		friend class E;
	public:
		iterator(E *p) : ptr(p) {}

		String toString() const { return hex(ptr); }

	public:
		iterator() { }
		iterator(const iterator &i) : ptr(i.ptr) { }

		reference operator*() const {return ptr->item;}
		value_type *operator->() const {return &(ptr->item);}

		iterator child() const
		{
			assert(ptr);
			return iterator(ptr->Child());
		}

		iterator peer() const
		{
			assert(ptr);
			return iterator(ptr->Peer());
		}

		iterator parent() const
		{
			assert(ptr);
			return iterator(ptr->Parent());
		}

		bool operator==(const iterator& _X) const
		{
			return (ptr == _X.ptr);
		}

		operator bool() const {  return (ptr!=0); }

		bool operator!=(const iterator& _X) const
		{
			return (!(*this == _X)); 
		}

		E *GetNode() { return ptr; }

	protected:
		E *ptr;
	};

public:


	cptree()
	{
		m_root = 0;
	}

	cptree(const T &root) : m_root(0)
	{
		m_root = new E(root);
	}
	cptree(const cptree &t) : m_root(0)
	{
		(*this) = t;
	}
	~cptree()
	{
		if(m_root) 
		{
			destroytree(m_root);
			m_root = 0;
		}
	}

	cptree &operator =(const cptree &t)
	{
		if(m_root) 
		{
			destroytree(m_root);
			m_root = 0;
		}

		copytree(root(), t.root());

		return (*this);
	}

	void copytree(iterator parent, const_iterator inserting)
	{
		if( parent.GetNode() == 0 && inserting.GetNode() == 0) return;
		// assuming single-rooted tree at the moment !!!
		iterator inserted;
		if(parent.GetNode() == 0 /*parent == null()*/ )
		{
			inserted = set_root(*inserting);
		}
		else
		{
			inserted = insert_child_last(parent, *inserting);
		}

		if( inserting.GetNode()->Child() != 0 ) copytree(inserted, const_iterator(inserting.GetNode()->Child()) );
		if( inserting.GetNode()->Peer() != 0  ) copytree(parent, const_iterator(inserting.GetNode()->Peer()) );
	}


	// access
	iterator root() const
	{
		return iterator(m_root);
	}

	iterator root()
	{
		return iterator(m_root);
	}

	static iterator null()
	{
		return iterator( (E*)0 );
	}

	/*
	bool operator ==(const cptree &)
	{
		return false;
	}

	bool operator !=(const cptree &)
	{
		return true;
	}
	bool operator <(const cptree &right)
	{
		return true;
		//return size() < right.size();
	}
	*/

	void erase(iterator stem)
	{
		if(stem.GetNode()->Parent()/*stem.parent() != null()*/)
		{
			stem.GetNode()->Parent()->SetChild(0);
		}
		else
		{
			//assert(stem.GetNode() == m_root);
			m_root = 0;
		}

		destroytree(stem.GetNode());
	}

	// modifying
	void clear()
	{
		if( root() != null() )
			erase(root());
	}


	void prune(iterator it)
	{
		if(it.child() != null())
		{
			erase(it.child());
		}
	}

	/*
	iterator attach(iterator it1, iterator it2)
	{

	}

	iterator attach(iterator it1, cptree &tree)
	{
		recurse(tree.root(), );
	}
	*/

	iterator set_root(const value_type &item)
	{
		clear();

		if(m_root)
		{
			m_root->SetItem(item);
		}
		else
		{
			m_root = new E(item);
		}

		return root();
	}

	static iterator insert_peer_after(iterator it, const value_type &item)
	{
		E *orig = it.GetNode();
		E *ins = new E(item); // allocator.construct ? !!

		ins->SetPeer(orig->Peer());
		ins->SetParent(orig->Parent());
		orig->SetPeer(ins);

		return iterator(ins);
	}

	static iterator insert_child_first(iterator it, const value_type &item)
	{
		E *orig = it.GetNode();
		E *ins = new E(item); // allocator.construct ? !!

		ins->SetParent(orig);
		ins->SetPeer(orig->Child());
		orig->SetChild(ins);

		return iterator(ins);
	}

	static iterator insert_child_last(iterator it, const value_type &item)
	{
		if(it.GetNode()->Child())
		{
			E *orig = it.GetNode();
			orig = orig->Child();

			while (orig->Peer())
			{
				orig = orig->Peer();
			}

			return insert_peer_after(iterator(orig), item);
		}
		else
		{
			return insert_child_first(it, item);
		}
	}


	static iterator insert(iterator parent, const value_type &item)
	{
		E *p = parent.GetNode();
		E *ins = new E(item);
		E *c = p->Child();

		ins->SetParent(p);
		ins->SetChild(c);

		p->SetChild(ins);

		while(c)
		{
			c->SetParent(ins);
			c = c->Peer();
		}

		return iterator(ins);
	}

	template<class mergefunc>
	static void merge_unary_branching(iterator it, mergefunc& func)
	{
		while(countchildren(it) == 1)
		{
			assert(it.child());
			assert(!it.child().peer());

			func(it, it.child());

			E* current = it.GetNode();
			E* toRemove = current->Child();
			E* newChild = toRemove->Child();

			current->SetChild(newChild);

			E* looper = newChild;
			while(looper)
			{
				looper->SetParent(current); 
				looper = looper->Peer();
			}

			delete toRemove;
		}

		if(it.child()) 
			merge_unary_branching(it.child(), func);
		if(it.peer()) 
			merge_unary_branching(it.peer(), func);
	}

	static bool is_binary(const_iterator it)
	{
		bool stillbinary = countchildren(it) <= 2;

		if(stillbinary && it.child()) 
			stillbinary = is_binary(it.child());
		if(stillbinary && it.peer()) 
			stillbinary = is_binary(it.peer());

		return stillbinary;
	}

	static void reverse_child_order(iterator it)
	{
		if(it.child() == null())
			return;

		E* current = it.GetNode();
		E* child = current->Child();

		std::vector<E*> children;
		children.reserve(4);
		while(child)
		{
			children.push_back(child);
			child = child->Peer();
		}

		typename std::vector<E*>::reverse_iterator rit = children.rbegin();
		E* currentChild = (*rit++);

		current->SetChild( currentChild );

		for(; rit != children.rend();++rit)
		{
			currentChild->SetPeer(*rit);
			currentChild = *rit;
		}

		currentChild->SetPeer(0);
	}

	template<typename V>
	struct MaxAccumulater
	{
		typedef V value_type;

		value_type initial;
		value_type max_value;

		MaxAccumulater(value_type initial_value) : initial(initial_value), max_value(initial_value) {}

		void MaxWith(value_type value)	{ if(value > max_value) max_value = value; }
		void Reset()					{ max_value = initial; }

		value_type	GetValue() const	{ return max_value; }
	};

	template<typename V>
	struct MinAccumulater
	{
		typedef V value_type;

		value_type initial;
		value_type min_value;

		MinAccumulater(value_type initial_value) : initial(initial_value), min_value(initial_value) {}

		void MinWith(value_type value)	{ if(value < min_value) min_value = value; }
		void Reset()					{ min_value = initial; }

		value_type	GetValue() const	{ return min_value; }
	};

	// 
	template<typename V>
	struct ValueAccumulater
	{
		typedef V value_type;

		value_type accumulator;

		ValueAccumulater(V initial_value) : accumulator(initial_value) {}

		void Add(V value)				{ accumulator += value; }
		void Reset()					{ accumulator -= accumulator; }

		value_type	GetValue() const	{ return accumulator; }
	};

	template<typename V>
	struct DepthAccumulator
	{
		typedef V value_type;

		value_type	current;
		value_type	maximum;

		DepthAccumulator(V initial_value) : current(initial_value), maximum(initial_value) {}

		void Increment(V value)		{ current += value; if(current > maximum) maximum = current; }
		void Decrement(V value)		{ current -= value; }

		void Reset()				{ current -= current; maximum -= maximum; }

		value_type GetValue()		{ return maximum; }
	};

	struct SingleItemCounter : public ValueAccumulater<u32>
	{
		SingleItemCounter() : ValueAccumulater<u32>(0) {}

		void	Increment()			{ ValueAccumulater<u32>::Add(1); }
		u32		GetCount() const	{ return ValueAccumulater<u32>::GetValue(); }
	};

	struct SingleItemAccumulator : public ValueAccumulater<float>
	{
		SingleItemAccumulator(float initial_value = 0.0f) : ValueAccumulater<float>(initial_value) {}
	};

	struct TerminalCounter : public SingleItemCounter
	{
		void operator()(const_iterator it)
		{
			if(!it.child()) 
				SingleItemCounter::Increment();
		}
	};

	struct BranchPointCounter : public SingleItemCounter
	{
		void operator()(const_iterator it)
		{
			if( countchildren(it) >= 2)
				SingleItemCounter::Increment();
		}
	};

	struct BranchCounter : public SingleItemCounter
	{
		void operator()(const_iterator it)
		{
			SingleItemCounter::Increment();
		}
	};

	struct BranchingOrderCounter : public MaxAccumulater<u32>
	{
		BranchingOrderCounter() : MaxAccumulater<u32>(0) {}

		void operator()(const_iterator it)
		{
			if(!it.child())
			{
				const_iterator rootSeeker = it;
				u32 countOrder = 0;
				while(rootSeeker.parent())
				{
					if(countchildren(rootSeeker.parent()) > 1)
						++countOrder;

					rootSeeker = rootSeeker.parent();
				}

				MaxAccumulater<u32>::MaxWith(countOrder);
			}
		}

		u32 GetCount()
		{
			return MaxAccumulater<u32>::GetValue();
		}
	};

	// for applying multiple operations in one go
	template<typename T1, typename T2>
	struct multiple_functor
	{
		T1& m_t1;
		T2& m_t2;

		multiple_functor(T1& t1, T2 &t2) : m_t1(t1), m_t2(t2) { }

		void operator() (const_iterator it)
		{
			m_t1(it);
			m_t2(it);
		}
	};


	// non-modifying 
	int size(const_iterator it) const
	{
		BranchCounter c;

		recurse_it(it, c);

		return c.GetValue();
	}

	int size() const
	{
		if(root() != null() )
			return size( root() );

		return 0;
	}

	static int countchildren(const_iterator it)
	{
		int count=0;

		it = it.child();

		while(it != null() )
		{
			++count;
			it = it.peer();
		}

		return count;
	}

	static int countpeers(const_iterator it)
	{
		const_iterator start;

		if(it.parent() != null() )
		{
			start = it.parent().child();
		}
		else
		{
			start = it;
		}

		int count = 0;
		do
		{
			count++;
			start = start.peer();
		} 
		while(start != null() );

		return count;
	}

	int countterminals(const_iterator it) const
	{
		TerminalCounter tc;

		recurse_it(it, tc);

		return tc.count;
	}

	bool empty()
	{
		return( root() == null() );
	}

	bool terminal(const_iterator it)
	{
		return( it.child() == null() );
	}


	// recurse through tree
	template<class unaryfunc>
	static void recurse(iterator root, unaryfunc &f)
	{
		f( *root );
		if(root.child() != null() ) recurse(root.child(), f);
		if(root.peer() != null() ) recurse(root.peer(), f);
	}

	template<class unaryfunc>
	static void recurse(const_iterator root, unaryfunc &f)
	{
		f( *root );
		if(root.child() != null() ) recurse(root.child(), f);
		if(root.peer() != null() ) recurse(root.peer(), f);
	}

	template<class unaryfunc>
	static void recurse_it(const_iterator root, unaryfunc &f)
	{
		f( root );
		if(root.child() != null() ) {recurse_it(root.child(), f); }
		if(root.peer() != null() ) recurse_it(root.peer(), f);
	}

	template<class unaryfunc>
	static void recurse_it(iterator root, unaryfunc &f)
	{
		f( root );
		if(root.child() != null() ) {recurse_it(root.child(), f); }
		if(root.peer() != null() ) recurse_it(root.peer(), f);
	}

	template<class unaryfunc>
	static void recurse_pc(iterator root, unaryfunc &f)
	{
		f( *root );
		if(root.peer() != null() ) recurse_pc(root.peer(), f);
		if(root.child() != null() ) recurse_pc(root.child(), f);
	}

	template<class unaryfunc>
	static void recurse_reverse(iterator root, unaryfunc &f)
	{
		if(root.child() != null() ) recurse_reverse(root.child(), f);
		if(root.peer() != null() ) recurse_reverse(root.peer(), f);
		f( root );
	}

	template<class unaryfunc>
	static void recurse_reverse_pc(iterator root, unaryfunc &f)
	{
		if(root.peer() != null() ) recurse_reverse_pc(root.peer(), f);
		if(root.child() != null() ) recurse_reverse_pc(root.child(), f);
		f( *root );
	}

private:
	E *m_root;
};


#endif // __CORE_CPTREE_H__NL__
