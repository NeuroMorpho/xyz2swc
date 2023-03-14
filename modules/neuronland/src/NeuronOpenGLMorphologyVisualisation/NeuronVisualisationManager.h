#ifndef NEURONVISUALISATIONMANAGER_HPP_
#define NEURONVISUALISATIONMANAGER_HPP_

#include "Core/String.hpp"

class Neuron3D;


class NeuronVisualisationManager
{
public:

					NeuronVisualisationManager();

	// setup
	static void		Initialise();

	void			Scan(const String& pathname);
	void			RegisterScript(ScriptNeuronVisualisationHandler* pHandler);

	NeuronVisualisationBase* Create(NeuronVisualisationID id);

private:

	void RegisterVisualisationHandlerBuiltIn();
	void RegisterVisualisationHandlerPlugIn(const String& rootPath);
	void RegisterVisualisationHandlerScript(const String& rootPath);

};

#endif // NEURONVISUALISATIONMANAGER_HPP_