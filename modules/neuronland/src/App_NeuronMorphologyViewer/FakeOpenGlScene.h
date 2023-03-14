#ifndef OPENGLSCENE_HPP_
#define OPENGLSCENE_HPP_

namespace MorphologyDataManager
{
 typedef int   NeuronID;
}

class Neuron3D;
class MorphologyDataFaults;
struct v4f
{
    v4f(){};
    v4f(float, float, float, float){}; 
    v4f Normalised(){return v4f();}
};
class OpenGLCell3D
{
public:

	OpenGLCell3D(/*Neuron3D& nrn*/){};

	virtual void	Render(){};

	void			SetCell(Neuron3D& nrn){};
	void			SetCellFaults(Neuron3D& nrn, const MorphologyDataFaults& faults){};

	void			Clear(){};

	// control
	void			Zoom(float zoom){};
	void			Stop(){};

	void			PauseRotation(){};
	void			UnpauseRotation(v4f axis, float velocity){};
	void			SetRotationAxisAndAngle(v4f axis, float angle){};



};



class OpenGLScene
{
public:
	void Create(){};
	void Resize(int w, int h){};;
	void Render(){};

	void SetCell(class Neuron3D& neuron){};;
	void SetCellFaults(Neuron3D& nrn, const class MorphologyDataFaults& faults){};;

	void Clear(){};;

	OpenGLCell3D* GetCell(){return NULL;};;

private:

};


#endif // OPENGLSCENE_HPP_
