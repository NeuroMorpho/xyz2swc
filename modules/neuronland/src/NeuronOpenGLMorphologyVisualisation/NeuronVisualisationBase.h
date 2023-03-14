#ifndef NEURONVISUALISATIONBASE_HPP_
#define NEURONVISUALISATIONBASE_HPP_

class Neuron3D;

class NeuronVisualisationBase
{
public:

					NeuronVisualisationBase();

	virtual void	Render() = 0;
	virtual void	SetCell(const Neuron3D& cell) = 0;

	//virtual void	GetElement(float x, float y); ///< for selecting

private:

};

#endif // NEURONVISUALISATIONBASE_HPP_
