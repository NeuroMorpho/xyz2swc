#ifndef NEURONGLCANVAS_HPP_
#define NEURONGLCANVAS_HPP_

#include "wx/glcanvas.h"

//#define TESTING_INTERFACEONLY

#ifdef TESTING_INTERFACEONLY
#include "FakeOpenGLScene.h"
#else
#include "NeuronOpenGLMorphologyVisualisation/OpenGLScene.h"
#include "NeuronMorphologyDataManager/MorphologyDataManager.h"
#endif //ifndef TESTING_INTERFACEONLY

class Neuron3D;

/** 
	@brief  wxWidgets wrapper of generic neuron openGL functionality
**/

class NeuronGLCanvas: public wxGLCanvas
{
public:
	NeuronGLCanvas( wxWindow *parent, wxWindowID id = wxID_ANY,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = 0, const wxString& name = _T("NeuronGLCanvas") );

	~NeuronGLCanvas();


	void Clear();

	void OnPaint(wxPaintEvent& event);
	void OnTimerEvent(wxTimerEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnEnterWindow(wxMouseEvent& event);
	void OnSize(wxSizeEvent &event);
	void OnMouseEvent(wxMouseEvent &event);

	void OnNeuronChanged(wxCommandEvent& event);

	void Render();
	void InitGL();
	void ResizeScene(wxSize size);

	
	//nlMorfology app
	void SetCell(MorphologyDataManager::NeuronID cellId);
	MorphologyDataManager::NeuronID GetCellId() const;
	
	void SetNeuronViewMode(NeuronDrawMode drawMode);

private:

	wxTimer *pTimer;
	OpenGLScene m_scene;
	
	bool dragging;
	wxPoint startPos;

	int m_cellID;

	DECLARE_EVENT_TABLE()
};

// inlines

inline void NeuronGLCanvas::OnSize(wxSizeEvent &event)						
{
	wxGLCanvas::OnSize(event);  ResizeScene(event.GetSize());  
	//Added by vanessa
	Refresh();
}
inline void NeuronGLCanvas::OnEnterWindow(wxMouseEvent& WXUNUSED(event))		
{ SetFocus(); 
}

inline void NeuronGLCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))				
{ Render(); }
inline void NeuronGLCanvas::OnTimerEvent(wxTimerEvent& WXUNUSED(event))			
{ Refresh(); }
inline void NeuronGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))	
{ }

inline void NeuronGLCanvas::SetNeuronViewMode(NeuronDrawMode drawMode)
{
	m_scene.SetDrawMode(drawMode);
}

#endif // NEURONGLCANVAS_HPP_
