//
//
//

#include "OpenGLScene.h"
OpenGLScene::OpenGLScene():m_pOpenGLCell3D(NULL), m_pOpenGLCellDendrogram(NULL)
{

}
OpenGLScene::~OpenGLScene()
{
	if (m_pOpenGLCell3D) delete m_pOpenGLCell3D;
	if (m_pOpenGLCellDendrogram) delete m_pOpenGLCellDendrogram;
}

void OpenGLScene::Create()
{
	m_pOpenGLCell3D = new OpenGLCell3D;
//	m_pOpenGLCellDendrogram = new OpenGLCellDendrogram;

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_ALWAYS);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

}

void OpenGLScene::Resize(int w, int h)
{
	glViewport(0, 0, (GLint)w, (GLint)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, (GLfloat)w/(GLfloat)h, 0.1f, 5000.0f);
	//glOrtho((float)-w*0.5f,(float)w*0.5f, (float)-h*0.5f, (float)h*0.5f, 0.1f, 5000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//m_pOpenGLCell3D->Resize(w, h);
}

void OpenGLScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	// To create transparencies (alpha blending)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_pOpenGLCell3D->Render();
	//m_pOpenGLCellDendrogram->Render();
}

void OpenGLScene::SetCell(class Neuron3D& neuron)
{
	m_pOpenGLCell3D->SetCell(neuron);
	//m_pOpenGLCellDendrogram->SetCell(neuron);
}

void OpenGLScene::SetDrawMode(NeuronDrawMode drawmode)
{
	m_pOpenGLCell3D->SetDrawMode(drawmode);

}

void OpenGLScene::SetCellFaults(Neuron3D& nrn, const MorphologyDataFaults& faults)
{	
	m_pOpenGLCell3D->SetCellFaults(nrn, faults);
}

OpenGLCell3D* OpenGLScene::GetCell()
{
	return m_pOpenGLCell3D;
}

void OpenGLScene::Clear()
{
	m_pOpenGLCell3D->Clear();
}
