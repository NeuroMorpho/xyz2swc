// TEST - just messing about with some generic programming for generating data source managers...

/** 

	Sources -	BuiltIn
				PlugIn
				Script
				Internet...
				Other ?

	Objects -	PlugIn  (Create, Employ)
				Neuron  (Create, Modify, Extract, Employ..)
				Network (Create, Modify, Extract, Employ..)

	Multi-source Functionality -		Neuron - MorphologyFormat (Create)
										Neuron - NeuronGeneration (Create)
										Neuron - NeuronReduction (Modify)
										Neuron - GenericNeuronActionModifying (Modify)
										Neuron - GenericNeuronActionNonModifying (Extract)
										Neuron - GeometryAnalysis (Extract)
										Neuron - NeuronVisualisation - OpenGL  (Extract)
										Neuron - Simulate (Employ)

										GUI Components

	Ability to define new multi-source functionality - needs initial C++ work...
	How about starting new functionality within a script? Then exposing to rest of sources somehow... - in script derive from FunctionalitySourceBase, ???

	Individual FunctionalityManagers can be accessed to control which functionality instances are exposed...

*/

class FunctionalitySource
{
public:
	virtual void Scan(const String&);

};

/** 
	How should a new type be automatically integrated into application ?
*/
class FunctionalityType
{
public:
	virtual void Scan(const String&);
};

/**
	How is functionality to be registered... ?
*/
class FunctionalityManager
{
public:
	virtual void Scan(const String&);

};


