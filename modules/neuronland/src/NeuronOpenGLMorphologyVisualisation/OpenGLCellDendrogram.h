#ifndef OPENGLCELLDENDROGRAM_HPP_
#define OPENGLCELLDENDROGRAM_HPP_

#include "Core/v4f.h"
#include "Core/m44f.h"

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "NeuronRepresentation/Neuron3D.h"

class OpenGLCellDendrogram
{
public:

	OpenGLCellDendrogram(/*Neuron3D& nrn*/);

	virtual void	Render();

	void			SetCell(Neuron3D& nrn);
	void			Clear();

private:

	void 			GenerateNeuron(Neuron3D &nrn, int sides);
	int				GenerateBranch(Neuron3D::DendriteTreeConstIterator bit, float xoffset, float yoffset, float xspace, int dl);
	void			DestroyRenderData();

	GLuint 	m_listIndexSoma;
	GLuint 	m_listIndexDendrites;
	GLuint	m_listIndexAxons;
	GLuint 	m_listIndexEnd;

	float m_extent;
};

#endif // OPENGLCELLDENDROGRAM_HPP_