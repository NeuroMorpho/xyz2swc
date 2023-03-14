//
// OpenGLCell3D.cpp
//

#include "OpenGLCell3D.h"

#ifdef CUSTOMIZE_NEURON_COLOURS
#include "NeuronRepresentation/NeuronComponentDisplayProperties.h"
#include <sstream>
#endif//fdef CUSTOMIZE_NEURON_COLOURS

#define PI 3.1415926f

OpenGLCell3D::OpenGLCell3D(/*Neuron3D& nrn*/)
{
	m_rotationAxis = v4f(0.0f, 1.0f, 0.0f, 0.0f);
	m_rotationAngle = 0.0f;
	m_rotationVelocity = 0.05f;

	m_listIndexEnd = 0;
	m_zoom = 100.0f;

	m_baseRotation	= m44f::Identity();
	m_currentMat	= m_baseRotation;
	
	m_timeElapsed	= 1.0f;

	mDrawMode = GL_LINE_STRIP;
}

void OpenGLCell3D::PauseRotation()
{
	m_rotationVelocity = 0.0f;
	m_baseRotation = m_currentMat;
	m_rotationAngle = 0.0f;
}

void OpenGLCell3D::UnpauseRotation(v4f newAxis, float w)
{
	m_rotationVelocity = w;
	m_rotationAxis = newAxis;
	m_baseRotation = m_currentMat;
	m_rotationAngle = 0.0f;
}

void OpenGLCell3D::SetRotationAxisAndAngle(v4f axis, float angle)
{
	m_rotationAxis = axis;
	m_rotationAngle = angle;
}

void OpenGLCell3D::Render()
{
	if(m_listIndexEnd)
	{
		glPointSize(1.5f);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -(m_extent*0.75f+m_zoom));

		m_timeElapsed = 1.0f;
		m_rotationAngle += m_timeElapsed * m_rotationVelocity;

		// TODO - move angle bounding into specific type of generic value-binding/value-wrapping class
		if(m_rotationAngle > PI)
			m_rotationAngle -= 2*PI;
		if(m_rotationAngle < -PI)
			m_rotationAngle += 2*PI;
		// TODO

		m_currentMat = m44f(m_rotationAxis, m_rotationAngle) * m_baseRotation;

		glMultMatrixf((GLfloat*)&m_currentMat);

		#ifndef CUSTOMIZE_NEURON_COLOURS
			glColor3f(0.0, 0.0, 1.0);
		#endif

		for(u32 i=m_listIndexSoma;i<m_listIndexDendrites;++i)
		{
			#ifdef CUSTOMIZE_NEURON_COLOURS
			SetGlColourForNeuronComponent(i);
			#endif
			glBegin(mDrawMode);
			glCallList(i);
			glEnd();

		}
#ifndef CUSTOMIZE_NEURON_COLOURS
		glColor3f(1.0, 0.0, 0.0);
#endif //ifndef CUSTOMIZE_COMPS_COLOURS

		for(u32 i=m_listIndexDendrites;i<m_listIndexDendriteSpines;++i)
		{
		#ifdef CUSTOMIZE_NEURON_COLOURS
			SetGlColourForNeuronComponent(i);
		#endif
			glBegin(mDrawMode);
			glCallList(i);
			glEnd();
		}
		for(u32 i=m_listIndexDendriteSpines;i<m_listIndexAxons;++i)
		{
		#ifdef CUSTOMIZE_NEURON_COLOURS
			SetGlColourForNeuronComponent(m_listIndexSpines);
		#endif
			glBegin(mDrawMode);
			glCallList(i);
			glEnd();
		}

		#ifndef CUSTOMIZE_NEURON_COLOURS
		glColor3f(0.0, 1.0, 0.0);
		#endif //ifndef CUSTOMIZE_COMPS_COLOURS

		for(u32 i=m_listIndexAxons;i<m_listIndexAxonSpines;++i)
		{
			#ifdef CUSTOMIZE_NEURON_COLOURS
			SetGlColourForNeuronComponent(i);
			#endif
			glBegin(mDrawMode);
			glCallList(i);
			glEnd();
		}
		for(u32 i=m_listIndexAxonSpines;i<m_listIndexMarkers;++i)
		{
			#ifdef CUSTOMIZE_NEURON_COLOURS
			SetGlColourForNeuronComponent(m_listIndexSpines);
			#endif
			glBegin(mDrawMode);
			glCallList(i);
			glEnd();
		}


		// markers
		if(m_listIndexMarkers < m_listIndexSpines)
		{
#ifdef CUSTOMIZE_NEURON_COLOURS
			SetGlColourForNeuronComponent(m_listIndexMarkers);
#endif
			glBegin(GL_POINTS);
			glCallList(m_listIndexMarkers);
			glEnd();
		}

		// spines
		if(m_listIndexSpines < m_listIndexEnd)
		{
#ifdef CUSTOMIZE_NEURON_COLOURS
			SetGlColourForNeuronComponent(m_listIndexSpines);
#endif
			glBegin(GL_POINTS);
			glCallList(m_listIndexSpines);
			glEnd();
		}
	}
}

void OpenGLCell3D::SetCell(Neuron3D& nrn)
{
	DestroyCellRenderData();

	GenerateNeuron(nrn, 6);

	m_zoom = 100;
}

void OpenGLCell3D::SetCellFaults(Neuron3D& nrn, const MorphologyDataFaults& faults)
{
	DestroyFaultRenderData();

	GenerateFaults(nrn, faults);
}

void OpenGLCell3D::Clear()
{
	DestroyCellRenderData();
}

void OpenGLCell3D::SetDrawMode(NeuronDrawMode drawmode)
{
	switch(drawmode)
	{
	case kNeuronModePoints:
		mDrawMode = GL_POINTS;
		break;
	case kNeuronModeLine:
		mDrawMode = GL_LINE_STRIP;
		break;
	case kNeuronModeCylinders:
		mDrawMode = GL_QUAD_STRIP;
		break;
	}
}

void OpenGLCell3D::DestroyCellRenderData()
{
	if(m_listIndexEnd > 0)
	{
		glDeleteLists(m_listIndexSoma, m_listIndexEnd-1);
		m_listIndexEnd = 0;
		mComponentListEndIndex.clear();
	}
}

void OpenGLCell3D::DestroyFaultRenderData()
{
}

bool OpenGLCell3D::GenerateBranchSegmentDL(const Branch3D &bran, int dl, bool closed, bool spines)
{
	const bool isSpineContour = (bran.m_spines.size() > 0 && bran.m_spines[0].type == kSpineContourRoot);

	if(isSpineContour)
	{
		assert(bran.m_spines.size() == 1);
	}

	if( (spines && !isSpineContour) ||
		(!spines && isSpineContour) )
	{
		return false;
	}

	glNewList(dl, GL_COMPILE);

	u32 samples = (u32)bran.m_samples.size();
	for( u32 s = 0; s < samples; ++s )
	{
		float x = bran.m_samples[s].x;
		float y = bran.m_samples[s].y;
		float z = bran.m_samples[s].z;

		glVertex3f(x, y, z);

		m_minX = min(x, m_minX);
		m_minY = min(y, m_minY);
		m_minZ = min(z, m_minZ);

		m_maxX = max(x, m_maxX);
		m_maxY = max(y, m_maxY);
		m_maxZ = max(z, m_maxZ);
	}

	if(closed)
	{
		float x = bran.m_samples[0].x;
		float y = bran.m_samples[0].y;
		float z = bran.m_samples[0].z;

		glVertex3f(x, y, z);
	}

	glEndList();

	return true;
}

int OpenGLCell3D::GenerateBranch(Neuron3D::DendriteTreeConstIterator bit, int sides, int dl, bool spines)
{
	float fScale = 1.0f;

	if( GenerateBranchSegmentDL( (*bit), dl, false, spines) )
		++dl;

	if(bit.child()) dl = GenerateBranch(bit.child(), sides, dl, spines);
	if(bit.peer()) dl = GenerateBranch(bit.peer(), sides, dl, spines);

	return dl;
}

void OpenGLCell3D::AddBranchMarkers(const Branch3D& bran, bool spines)
{
	if(spines)
	{
		// spine points: this will duplicate the contour spines, if such spines exist...
		u32 numSpines = (u32)bran.m_spines.size();

		for( u32 s = 0; s < numSpines; ++s )
		{
			float x = bran.m_spines[s].x;
			float y = bran.m_spines[s].y;
			float z = bran.m_spines[s].z;

			glVertex3f(x, y, z);
		}
		
	}
	else
	{
		for(Branch3D::Markers::const_iterator it=bran.m_markers.begin();it!=bran.m_markers.end();++it)
		{
			u32 markerIndex = (*it).second[0].shape;
			//String markerType = NeurolucidaMarkerId2Name(markerIndex);
			String markerName = (*it).first;

			for(u32 i=0; i<it->second.size();++i)
			{
				float x = it->second[i].x;
				float y = it->second[i].y;
				float z = it->second[i].z;

				glVertex3f(x, y, z);
			}
		}
	}
}

void OpenGLCell3D::GenerateBranchMarkers(Neuron3D::DendriteTreeConstIterator it, bool spines)
{
	AddBranchMarkers(*it, spines);

	if(it.child()) GenerateBranchMarkers(it.child(), spines);
	if(it.peer()) GenerateBranchMarkers(it.peer(), spines);
}

void OpenGLCell3D::GenerateMarkersOrSpines(Neuron3D &nrn, int dl, bool spines)
{
	glNewList(dl, GL_COMPILE);

	AddBranchMarkers(nrn.m_globalData, spines);

	// soma
	for(Neuron3D::SomaIterator it = nrn.SomaBegin();it != nrn.SomaEnd();++it)
		AddBranchMarkers( (*it), spines );

	// dendrites
	for(Neuron3D::DendriteIterator it = nrn.DendriteBegin();it != nrn.DendriteEnd();++it)
		GenerateBranchMarkers( (*it).root(), spines );

	// axons
	for(Neuron3D::AxonIterator it = nrn.AxonBegin();it != nrn.AxonEnd();++it)
		GenerateBranchMarkers((*it).root(), spines );

	glEndList();
}

void OpenGLCell3D::GenerateNeuron(Neuron3D &nrn, int sides)
{
	const u32 axonElements = nrn.CountAxonElements();
	const u32 dendriteElements = nrn.CountDendriteElements();
	const u32 somaElements = nrn.CountSomas();
	const u32 markerElements = (nrn.CountAllMarkers() > 0) ? 1 : 0 ;
	const u32 spineElements = (nrn.CountAllSpines() > 0) ? 1 : 0 ;

	const u32 totalElements = axonElements + dendriteElements + somaElements + markerElements + spineElements; 

	mComponentListEndIndex.reserve(10);

//	if(totalElements > 0)
	{
		int listCount = glGenLists(totalElements);


		m_maxX = m_maxY = m_maxZ = -1e20f;
		m_minX = m_minY = m_minZ = 1e20f;

		// soma
		m_listIndexSoma = listCount;
		for(Neuron3D::SomaIterator it = nrn.SomaBegin();it != nrn.SomaEnd();++it)
		{
			GenerateBranchSegmentDL( (*it), listCount, (*it).somaType == Soma3D::kContourClosed, false );
			++listCount;
			mComponentListEndIndex.push_back(listCount);
		}

		// dendrites
		m_listIndexDendrites = listCount;
		for(Neuron3D::DendriteIterator it = nrn.DendriteBegin();it != nrn.DendriteEnd();++it)
		{
			if((*it).root())
				listCount = GenerateBranch( (*it).root(), sides, listCount, false);
			mComponentListEndIndex.push_back(listCount);
		}
		// dendrite spine contours
		m_listIndexDendriteSpines = listCount;
		for(Neuron3D::DendriteIterator it = nrn.DendriteBegin();it != nrn.DendriteEnd();++it)
		{
			if((*it).root())
				listCount = GenerateBranch( (*it).root(), sides, listCount, true);
		}
		//if(listCount > m_listIndexDendriteSpines)
		//	mComponentListEndIndex.push_back(listCount);

		// axons
		m_listIndexAxons = listCount;
		for(Neuron3D::AxonIterator it = nrn.AxonBegin();it != nrn.AxonEnd();++it)
		{
			if((*it).root())
				listCount = GenerateBranch((*it).root(), sides, listCount, false);
			mComponentListEndIndex.push_back(listCount);
		}
		// axons spine contours
		m_listIndexAxonSpines = listCount;
		for(Neuron3D::AxonIterator it = nrn.AxonBegin();it != nrn.AxonEnd();++it)
		{
			if((*it).root())
				listCount = GenerateBranch((*it).root(), sides, listCount, true);
		}
		//if(listCount > m_listIndexAxonSpines)
		//	mComponentListEndIndex.push_back(listCount);

		m_listIndexMarkers = listCount;

		if(nrn.CountAllMarkers() > 0)
		{
			GenerateMarkersOrSpines(nrn, listCount, false);
			++listCount;
			mComponentListEndIndex.push_back(listCount);
		}

		m_listIndexSpines = listCount;
		if(nrn.CountAllSpines() > 0)
		{
			GenerateMarkersOrSpines(nrn, listCount, true);
			++listCount;
			mComponentListEndIndex.push_back(listCount);
		}

		m_listIndexEnd = listCount;

		float max = max( max(m_maxX, m_maxY), m_maxZ);
		float min = min( min(m_minX, m_minY), m_minZ);

		m_center = (max-min)/2.0f;
		m_extent = (max-min)/*/2.0f*/;
		//m_rot = 0.0;
	}

}

int OpenGLCell3D::GenerateFaults(Neuron3D &nrn, const MorphologyDataFaults& faults)
{
	return 0;
}



#ifdef CUSTOMIZE_NEURON_COLOURS
/************************************************************************/
/* Component id is the index!
/* Sets the drawing colour from the component display settings.
/************************************************************************/
void OpenGLCell3D::SetGlColourForNeuronComponent(u32 componentId )
{
	//float redc=redc=0.0f,greenc=0.0f,bluec=0.0f,alphac=0.0f;
	unsigned char redc=0,greenc=0,bluec=0,alphac=0;

	int size = GetNeuronElementsDisplaySettings().size();
	if (size>0)
	{
		unsigned int min = 0;
		int selectedindex = -1;
		for (int i= 0 ; i< mComponentListEndIndex.size();  i++ )
		{
			const u32 endComponent = mComponentListEndIndex[i];

			if ((min<=componentId ) && (componentId < endComponent))
			{
				selectedindex=i;
				break;
			}
			min=endComponent;
		}
		if (selectedindex>=0)
		{			
			NeuronComponentDisplayProperties& selected=GetNeuronElementsDisplaySettings()[selectedindex];
			redc = selected.mRed;///255.0f;
			greenc=selected.mGreen;///255.0f;
			bluec =selected.mBlue;///255.0f;
			alphac =selected.mAlpha;///255.0f;			
		}
	}
	//Else draw set background colour?
	//glColor4f(redc,greenc,bluec,alphac);
	glColor4ub(redc,greenc,bluec,alphac);
}



#endif //ifndef CUSTOMIZE_COMPS_COLOURS