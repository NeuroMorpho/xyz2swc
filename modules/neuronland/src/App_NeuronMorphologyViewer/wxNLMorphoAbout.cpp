//---------------------------------------------------------------------------
//
// Name:        wxNLMorphoAbout.cpp
// Author:      VanessaNessie
// Created:     19/01/2009 16:07:35
// Description: AppAbout class implementation
//
//---------------------------------------------------------------------------

#include "wxNLMorphoAbout.h"

//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

//----------------------------------------------------------------------------
// AppAbout
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(AppAbout,wxDialog)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(AppAbout::OnClose)
END_EVENT_TABLE()
////Event Table End

AppAbout::AppAbout(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxDialog(parent, id, title, position, size, style)
{
	CreateGUIControls();
}

AppAbout::~AppAbout()
{
} 

void AppAbout::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End.
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	WxSplitterWindow1 = new wxSplitterWindow(this, ID_WXSPLITTERWINDOW1, wxPoint(0,0), wxSize(304,298));
	WxSplitterWindow1->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Segoe UI")));

	SetTitle(wxT("About"));
	SetIcon(wxNullIcon);
	SetSize(8,8,320,334);
	Center();
	
	////GUI Items Creation End
}

void AppAbout::OnClose(wxCloseEvent& /*event*/)
{
	Destroy();
}
