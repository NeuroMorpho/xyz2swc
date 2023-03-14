//
// OpenGLCellDendrogram.cpp
//

#include "OpenGLCellDendrogram.h"


OpenGLCellDendrogram::OpenGLCellDendrogram(/*Neuron3D& nrn*/)
{
}

void OpenGLCellDendrogram::Render()
{
	if(m_listIndexEnd)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glTranslatef(0.0f, -m_extent/2, -(m_extent*0.75f /*+m_zoom*/ ));

		//glColor3f(0.0, 0.0, 1.0);
		//for(u32 i=m_listIndexSoma;i<m_listIndexDendrites;++i)
		//	glCallList(i);

		glColor3f(1.0, 0.0, 0.0);
		for(u32 i=m_listIndexDendrites;i<=m_listIndexAxons;++i)
			glCallList(i);

		glColor3f(0.0, 1.0, 0.0);
		for(u32 i=m_listIndexAxons+1;i<=m_listIndexEnd;++i)
			glCallList(i);
	}
}

void OpenGLCellDendrogram::SetCell(Neuron3D& nrn)
{
	DestroyRenderData();

	GenerateNeuron(nrn, 6);
}

void OpenGLCellDendrogram::Clear()
{
	DestroyRenderData();
}

void OpenGLCellDendrogram::DestroyRenderData()
{
	if(m_listIndexEnd > 0)
	{
		glDeleteLists(m_listIndexSoma, m_listIndexEnd-1);
		m_listIndexEnd = 0;
	}
}

int OpenGLCellDendrogram::GenerateBranch(Neuron3D::DendriteTreeConstIterator bit, float xoffset, float yoffset, float xspace, int dl)
{
	const float zposition = 100.0f;

	const u32 numPeers	= Neuron3D::DendriteTree::countpeers(bit);
	const float xmodify = (numPeers > 1) ? xspace/(float)(numPeers-1) : 0.0f;

	float xpeershift = xoffset - 0.5f*xspace;

	Neuron3D::DendriteTreeConstIterator currentBranch = bit;
	do
	{
		const float length = (*currentBranch).GetLength();

		++dl;
		glNewList(dl, GL_COMPILE);
		glBegin(GL_LINE_STRIP);
		glVertex3f(xpeershift, yoffset, zposition);
		glVertex3f(xpeershift, yoffset+length, zposition);
		glEnd();
		glEndList();

		if(currentBranch.child())
			dl = GenerateBranch(currentBranch.child(), xpeershift, yoffset + (*currentBranch).GetLength(), 0.48f*xspace, dl);

		currentBranch = currentBranch.peer();

		if(currentBranch)
		{
			++dl;
			glNewList(dl, GL_COMPILE);
			glBegin(GL_LINE_STRIP);
			glVertex3f(xpeershift, yoffset, zposition);
			glVertex3f(xpeershift+xmodify, yoffset, zposition);
			glEnd();
			glEndList();
		}
		xpeershift += xmodify;

	}while(currentBranch);

	return dl;
}

void OpenGLCellDendrogram::GenerateNeuron(Neuron3D &nrn, int sides)
{
	const u32 axonElements = nrn.CountAxonElements();
	const u32 dendriteElements = nrn.CountDendriteElements();
	const u32 somaElements = nrn.CountSomas();

	const u32 totalElements = axonElements + dendriteElements + somaElements; 

	//	if(totalElements > 0)
	{
		int listCount = glGenLists(totalElements);

		const float totalxspace = 250.0f;
		// soma
		m_listIndexSoma = listCount;
		// nothing...

		//vector<int> m_treeListIndex;

		float xpos = -200.0f;

		// dendrites
		m_listIndexDendrites = listCount;
		for(Neuron3D::DendriteIterator it = nrn.DendriteBegin();it != nrn.DendriteEnd();++it)
		{
			if((*it).root())
			{
				listCount = GenerateBranch( (*it).root(), xpos, 0.0f, totalxspace, listCount);
				xpos+=totalxspace;
	//			m_treeListIndex.push_back(listCount);
			}
		}

		// axons
		m_listIndexAxons = listCount;
		for(Neuron3D::AxonIterator it = nrn.AxonBegin();it != nrn.AxonEnd();++it)
		{
			if((*it).root())
			{
				listCount = GenerateBranch((*it).root(), xpos, 0.0f, totalxspace, listCount);
				xpos+=totalxspace;
	//			m_treeListIndex.push_back(listCount);
			}
		}

		m_listIndexEnd = listCount;
	}

	m_extent = 400.0f;
}
