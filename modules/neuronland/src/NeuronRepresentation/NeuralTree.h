#ifndef NEURALTREE_HPP_
#define NEURALTREE_HPP_


template<class branchtype>
class NeuralTree : public cptree<branchtype>
{
public:

	/*enum Type 
	{
		DendriticTree,
		AxonTree,
		UnknownTree
	};*/

	enum TreeSubType
	{
		Unspecified,
		Basal = Unspecified,
		Apical
	};

	NeuralTree(const branchtype &root) : cptree<branchtype>(root), mSubType(Unspecified)
	{
		set_root(root);
	}

	NeuralTree() : mSubType(Unspecified)
	{
	}

	template<class unaryfunc>
	void DoForAllElements(unaryfunc f) const
	{
		NeuralTree<branchtype>::recurse( NeuralTree<branchtype>::root(), f);
	}

	void SetTreeType(TreeSubType type) { mSubType = type; }
	TreeSubType GetTreeType() const { return mSubType; }

private:
	TreeSubType mSubType;
};


#endif // NEURALTREE_HPP_
