#ifndef MORPHOLOGYREDUCER_HPP_
#define MORPHOLOGYREDUCER_HPP_

class MorphologyReducer
{
public:
				MorphologyReducer();
	virtual		~MorphologyReducer();

	void		Reduce(Neuron3D::DendriteTreeIterator root) = 0;
};

#endif // MORPHOLOGYREDUCER_HPP_
