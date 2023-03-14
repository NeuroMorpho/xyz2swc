#ifndef OPENGLSCENE_HPP_
#define OPENGLSCENE_HPP_


#include "OpenGLCell3D.h"
#include "OpenGLCellDendrogram.h"


class OpenGLScene
{
public:
	OpenGLScene();
	~OpenGLScene();
	void Create();
	void Resize(int w, int h);
	void Render();

	void SetDrawMode(NeuronDrawMode drawmode);
	void SetCell(class Neuron3D& neuron);
	void SetCellFaults(Neuron3D& nrn, const struct MorphologyDataFaults& faults);

	void Clear();

	OpenGLCell3D* GetCell();

private:
	OpenGLCell3D*			m_pOpenGLCell3D;
	OpenGLCellDendrogram*	m_pOpenGLCellDendrogram;
};


#endif // OPENGLSCENE_HPP_