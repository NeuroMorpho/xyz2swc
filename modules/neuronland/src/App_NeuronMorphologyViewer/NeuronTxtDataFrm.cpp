//---------------------------------------------------------------------------
//
// Name:        NeuronTxtDataFrm.cpp
// Author:      VanessaNessie
// Created:     23/01/2009 11:15:29
// Description: NeuronTxtDataFrm class implementation
//
//---------------------------------------------------------------------------

#include "NeuronTxtDataFrm.h"

//Do not add custom headers between
//Header Include Start and Header Include End
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

//----------------------------------------------------------------------------
// NeuronTxtDataFrm
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(NeuronTxtDataFrm,wxFrame)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(NeuronTxtDataFrm::OnClose)
	
	EVT_LIST_ITEM_SELECTED(ID_WXLISTCTRL1,NeuronTxtDataFrm::WxListCtrl1Selected)
	EVT_LIST_ITEM_FOCUSED(ID_WXLISTCTRL1,NeuronTxtDataFrm::WxListCtrl1ItemFocused)
	EVT_BUTTON(ID_WXBUTTONCOMPCOLOUR,NeuronTxtDataFrm::WxButtonCompColourClick)
	EVT_BUTTON(ID_WXBUTTONSHOWMARKERS,NeuronTxtDataFrm::WxButtonShowMarkersClick)
	EVT_BUTTON(ID_WXBUTTONSHOWCOMP,NeuronTxtDataFrm::WxButtonShowCompClick)
END_EVENT_TABLE()
////Event Table End

NeuronTxtDataFrm::NeuronTxtDataFrm(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{
	CreateGUIControls();
}

NeuronTxtDataFrm::~NeuronTxtDataFrm()
{
}

void NeuronTxtDataFrm::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	WxBoxSizer1 = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(WxBoxSizer1);
	this->SetAutoLayout(true);

	WxBoxSizerComponentsList = new wxBoxSizer(wxHORIZONTAL);
	WxBoxSizer1->Add(WxBoxSizerComponentsList, 4, wxALIGN_TOP | wxALIGN_CENTER | wxEXPAND | wxALL, 5);

	wxStaticBox* WxStaticBoxSizerComponetsProp_StaticBoxObj = new wxStaticBox(this, wxID_ANY, wxT("Neuron Components"));
	WxStaticBoxSizerComponetsProp = new wxStaticBoxSizer(WxStaticBoxSizerComponetsProp_StaticBoxObj, wxHORIZONTAL);
	WxBoxSizerComponentsList->Add(WxStaticBoxSizerComponetsProp, 1, wxALIGN_CENTER | wxEXPAND | wxALL, 5);

	WxBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	WxStaticBoxSizerComponetsProp->Add(WxBoxSizer2, 1, wxALIGN_CENTER | wxEXPAND | wxALL, 5);

	wxArrayString arrayStringFor_WxListBoxNeuronComponents;
	WxListBoxNeuronComponents = new wxListBox(this, ID_WXLISTBOXNEURONCOMPONENTS, wxPoint(5,9), wxSize(121,97), arrayStringFor_WxListBoxNeuronComponents, wxLB_SINGLE);
	WxListBoxNeuronComponents->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Segoe UI")));
	WxBoxSizer2->Add(WxListBoxNeuronComponents,4,wxALIGN_CENTER | wxEXPAND | wxALL,5);

	WxBoxSizerCompButtons = new wxBoxSizer(wxVERTICAL);
	WxBoxSizer2->Add(WxBoxSizerCompButtons, 1, wxALIGN_CENTER | wxEXPAND | wxALL, 5);

	WxButtonShowComp = new wxButton(this, ID_WXBUTTONSHOWCOMP, wxT("Show"), wxPoint(5,5), wxSize(75,25), 0, wxDefaultValidator, wxT("WxButtonShowComp"));
	WxButtonShowComp->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Segoe UI")));
	WxBoxSizerCompButtons->Add(WxButtonShowComp,0,wxALIGN_CENTER | wxALL,5);

	WxButtonShowMarkers = new wxButton(this, ID_WXBUTTONSHOWMARKERS, wxT("Markers ON"), wxPoint(5,40), wxSize(75,25), 0, wxDefaultValidator, wxT("WxButtonShowMarkers"));
	WxButtonShowMarkers->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Segoe UI")));
	WxBoxSizerCompButtons->Add(WxButtonShowMarkers,0,wxALIGN_CENTER | wxALL,5);

	WxButtonCompColour = new wxButton(this, ID_WXBUTTONCOMPCOLOUR, wxT("Colour"), wxPoint(5,75), wxSize(75,25), 0, wxDefaultValidator, wxT("WxButtonCompColour"));
	WxButtonCompColour->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Segoe UI")));
	WxBoxSizerCompButtons->Add(WxButtonCompColour,0,wxALIGN_CENTER | wxALL,5);

	wxStaticBox* WxStaticBoxSizerComponentStats_StaticBoxObj = new wxStaticBox(this, wxID_ANY, wxT("ComponentStatistics"));
	WxStaticBoxSizerComponentStats = new wxStaticBoxSizer(WxStaticBoxSizerComponentStats_StaticBoxObj, wxHORIZONTAL);
	WxBoxSizer1->Add(WxStaticBoxSizerComponentStats, 1, wxALIGN_BOTTOM | wxALIGN_CENTER | wxEXPAND | wxALL, 5);

	/* 1
	2
	*/
	WxListCtrl1 = new wxListCtrl(this, ID_WXLISTCTRL1, wxPoint(5,190), wxSize(250,150), wxLC_LIST);
	WxListCtrl1->InsertColumn(0,wxT(""),wxLIST_FORMAT_LEFT,50 );
	WxListCtrl1->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Segoe UI")));
	WxBoxSizer1->Add(WxListCtrl1,0,wxALIGN_CENTER | wxALL,5);

	WxStaticBox1 = new wxStaticBox(this, ID_WXSTATICBOX1, wxT("WxStaticBox1"), wxPoint(37,350), wxSize(185,105));
	WxStaticBox1->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxBOLD, false, wxT("Segoe UI")));
	WxBoxSizer1->Add(WxStaticBox1,0,wxALIGN_CENTER | wxALL,5);

	wxStaticBox* WxStaticBoxSizer1_StaticBoxObj = new wxStaticBox(this, wxID_ANY, wxT(""));
	WxStaticBoxSizer1 = new wxStaticBoxSizer(WxStaticBoxSizer1_StaticBoxObj, wxHORIZONTAL);
	WxBoxSizer1->Add(WxStaticBoxSizer1, 0, wxALIGN_CENTER | wxALL, 5);

	SetTitle(wxT("NeuronTxtDataFrm"));
	SetIcon(wxNullIcon);
	
	GetSizer()->Layout();
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Center();
	
	////GUI Items Creation End
}

void NeuronTxtDataFrm::OnClose(wxCloseEvent& event)
{
	Destroy();
}

/*
 * WxButton1Click
 */
void NeuronTxtDataFrm::WxButton1Click(wxCommandEvent& event)
{
	// insert your code here
}

/*
 * WxButtonComponentMarkersClick
 */
void NeuronTxtDataFrm::WxButtonComponentMarkersClick(wxCommandEvent& event)
{
	// insert your code here
}

/*
 * WxButtonShowCompClick
 */
void NeuronTxtDataFrm::WxButtonShowCompClick(wxCommandEvent& event)
{
	// insert your code here
}

/*
 * WxButtonShowMarkersClick
 */
void NeuronTxtDataFrm::WxButtonShowMarkersClick(wxCommandEvent& event)
{
	// insert your code here
}

/*
 * WxButtonCompColourClick
 */
void NeuronTxtDataFrm::WxButtonCompColourClick(wxCommandEvent& event)
{
	// insert your code here
}

/*
 * WxListBox1Selected
 */
void NeuronTxtDataFrm::WxListBox1Selected(wxCommandEvent& event)
{
	// insert your code here
}

/*
 * WxListCtrl1Selected
 */
void NeuronTxtDataFrm::WxListCtrl1Selected(wxListEvent& event)
{
	// insert your code here
}

/*
 * WxListCtrl1ItemFocused
 */
void NeuronTxtDataFrm::WxListCtrl1ItemFocused(wxListEvent& event)
{
	// insert your code here
}

/*
 * WxListCtrl1Deselected
 */
void NeuronTxtDataFrm::WxListCtrl1Deselected(wxListEvent& event)
{
	// insert your code here
}
