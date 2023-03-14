//
//
//

#include "wx/wx.h"

#define PI 3.141592654f

#include "NeuronGLCanvas.h"

#ifndef TESTING_INTERFACEONLY
#include "NeuronRepresentation/Neuron3D.h"
#include "NeuronApplicationLogic/ApplicationLogic.h"
#endif //ifndef TESTING_INTERFACEONLY



#define ID_TIMER	1


BEGIN_EVENT_TABLE(NeuronGLCanvas, wxGLCanvas)
	EVT_SIZE(NeuronGLCanvas::OnSize)
	EVT_PAINT(NeuronGLCanvas::OnPaint)
	EVT_ERASE_BACKGROUND(NeuronGLCanvas::OnEraseBackground)
	EVT_ENTER_WINDOW( NeuronGLCanvas::OnEnterWindow )
	EVT_TIMER(ID_TIMER, NeuronGLCanvas::OnTimerEvent)
	EVT_MOUSE_EVENTS(NeuronGLCanvas::OnMouseEvent)
END_EVENT_TABLE()

NeuronGLCanvas::NeuronGLCanvas(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	: wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name )
{
	dragging = false;

	m_scene.Create();

	pTimer = new wxTimer(this, ID_TIMER);
	pTimer->Start(20);

	m_cellID = -1;
}

NeuronGLCanvas::~NeuronGLCanvas()
{
	if(pTimer->IsRunning())
		pTimer->Stop();

	delete pTimer;
}


void NeuronGLCanvas::Clear()
{
	m_scene.Clear();
}


void NeuronGLCanvas::Render()
{
	wxPaintDC dc(this);

#ifndef __WXMOTIF__
	if (!GetContext()) 
		return;
#endif

	SetCurrent();

	m_scene.Render();

	SwapBuffers();
}

void NeuronGLCanvas::ResizeScene(wxSize size)
{
#ifndef __WXMOTIF__
	if (GetContext())
#endif
	{
		SetCurrent();
		m_scene.Resize(size.GetWidth(), size.GetHeight());
	}
}


void NeuronGLCanvas::OnMouseEvent(wxMouseEvent &event)
{
	OpenGLCell3D* pCell = m_scene.GetCell();

	if(event.LeftDown())
	{
		startPos = event.GetPosition();
		//startTime = timeGetTime());
		pCell->PauseRotation();
	}

	if(event.Dragging())
	{
		dragging = true;

		const int deltaX = event.GetPosition().x - startPos.x;
		const int deltaY = event.GetPosition().y - startPos.y;

		if(deltaX != 0 || deltaY != 0)
		{
			const v4f rotationAxisVector = v4f( static_cast<float>(deltaY), static_cast<float>(deltaX), 0.0f, 0.0f).Normalised();

			const int screenWidth	= GetSize().x;
			const int screenHeight	= GetSize().y;
			const float angle = PI * sqrtf(deltaX*deltaX + deltaY*deltaY)/sqrtf(screenWidth*screenWidth + screenHeight*screenHeight);

			pCell->SetRotationAxisAndAngle(rotationAxisVector, angle);
		}
	}
	else
	{
		if(dragging)
		{
			dragging = false;

			const int deltaX = event.GetPosition().x - startPos.x;
			const int deltaY = event.GetPosition().y - startPos.y;

			if(deltaX != 0 || deltaY != 0)
			{
				const v4f rotationAxisVector = v4f( static_cast<float>(deltaY), static_cast<float>(deltaX), 0.0f, 0.0f).Normalised();

				/*const int screenWidth	= GetSize().x;
				const int screenHeight	= GetSize().y;
				const float angle = PI * sqrtf(deltaX*deltaX + deltaY*deltaY)/sqrtf(screenWidth*screenWidth + screenHeight*screenHeight);
				const float velocity = angle/timeElapsed;*/

				pCell->UnpauseRotation(rotationAxisVector, 1.0f*PI/180.0f);
			}
		}
	}

	// zoom in-out
	const int zoom = -event.GetWheelRotation()/10;	
	if(zoom)
	{
		pCell->Zoom(zoom);
	}
}


void NeuronGLCanvas::OnNeuronChanged(wxCommandEvent& event)
{
	if(m_cellID >= 0)
		SetCell(m_cellID);
}


/******************************************************************************
Functions actually using nlMorfology app data. not used during testing interface
*******************************************************************************/
void NeuronGLCanvas::SetCell(MorphologyDataManager::NeuronID cellId)
{
	m_cellID = cellId;
#ifndef TESTING_INTERFACEONLY
	Neuron3D& nrn = *g_applicationLogic.GetMorphologyDataManager().GetNeuron3D(m_cellID);

	m_scene.SetCell(nrn);
	m_scene.SetCellFaults(nrn, *g_applicationLogic.GetMorphologyDataManager().GetNeuronFaults(m_cellID));
#endif //ifdef TESTING_INTERFACEONLY	
}

MorphologyDataManager::NeuronID NeuronGLCanvas::GetCellId() const
{
	return m_cellID;
}


