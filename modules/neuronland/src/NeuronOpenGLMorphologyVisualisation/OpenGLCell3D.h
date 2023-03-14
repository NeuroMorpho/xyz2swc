#ifndef OPENGLCELL3D_HPP_
#define OPENGLCELL3D_HPP_

#include "Core/v4f.h"
#include "Core/m44f.h"

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>


#include "NeuronRepresentation/Neuron3D.h"

#include "NeuronMorphologyDataManager/MorphologyDataManager.h"

#include "NeuronRepresentation/NeuronComponentDisplayProperties.h"


enum NeuronDrawMode
{
	kNeuronModeLine,
	kNeuronModePoints,
	kNeuronModeCylinders,
};

/** 

	Neuron morphology visualisation. Should be able to render some or all of the following - 

	1) Neuron geometry
	2) Marker data
	3) Fault data
	4) Highlighted structure, activate/de-activate structure rendering
	5) Colour-coded arbitrary data...

	Set Render options - colours, 


	Select using mouse -

	Query functions for supported visualisation...

**/

class OpenGLCell3D
{
public:

					OpenGLCell3D(/*Neuron3D& nrn*/);

	virtual void	Render();

	void			SetCell(Neuron3D& nrn);
	void			SetCellFaults(Neuron3D& nrn, const MorphologyDataFaults& faults);
	void SetDrawMode(NeuronDrawMode drawmode);

	void			Clear();

	// control
	void			Zoom(float zoom);
	void			Stop();

	void			PauseRotation();
	void			UnpauseRotation(v4f axis, float velocity);
	void			SetRotationAxisAndAngle(v4f axis, float angle);

private:

	void 			GenerateNeuron(Neuron3D &nrn, int sides);
	bool 			GenerateBranchSegmentDL(const Branch3D &bran, int dl, bool closed = false, bool spines=false);
	int				GenerateBranch(Neuron3D::DendriteTreeConstIterator bit, int sides, int dl, bool spines=false);

	void 			AddBranchMarkers(const Branch3D& bran, bool spines);
	void 			GenerateBranchMarkers(Neuron3D::DendriteTreeConstIterator it, bool spines);
	void 			GenerateMarkersOrSpines(Neuron3D &nrn, int dl, bool spines);


	int				GenerateFaults(Neuron3D &nrn, const MorphologyDataFaults& faults);

	void			DestroyCellRenderData();
	void			DestroyFaultRenderData();



#ifdef CUSTOMIZE_NEURON_COLOURS
	/************************************************************************/
	/* Sets the drawing colour from the component display settings.
	/*	@todo Decide with james what the id is. Currently it is the index   */
	/************************************************************************/
	void SetGlColourForNeuronComponent(u32 componentId );

#endif //ifndef CUSTOMIZE_COMPS_COLOURS

	GLuint 	m_listIndexSoma;
	GLuint 	m_listIndexDendrites;
	GLuint 	m_listIndexDendriteSpines;
	GLuint	m_listIndexAxons;
	GLuint 	m_listIndexAxonSpines;
	GLuint	m_listIndexMarkers;
	GLuint	m_listIndexSpines;
	GLuint 	m_listIndexEnd;

	float	m_maxX;
	float	m_maxY;
	float	m_maxZ;
	float	m_minX;
	float	m_minY;
	float	m_minZ;
	float	m_extent;
	float	m_center;

	m44f	m_baseRotation;
	m44f	m_currentMat;

	v4f		m_rotationAxis;
	float	m_rotationAngle;
	float	m_rotationVelocity;
	float	m_timeElapsed;

	float	m_zoom;

	int		mDrawMode;

	std::vector<int> mComponentListEndIndex; //opengl list end for each Neuron Component
	/*
		[2,5,10] ->
		comp 1 -> list 0-2
		comp 2 ->list 2 to 5
		comp 3 -> list 5 to 10
	*/
};

// inline
inline void OpenGLCell3D::Zoom(float zoom)
{
	// TODO : set cell-dependent limits on zoom range
	m_zoom += zoom;
}

#endif // OPENGLCELL3D_HPP_