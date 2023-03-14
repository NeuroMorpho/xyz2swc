//---------------------------------------------------------------------------
//
// Name:        wxNLMorphoAbout.h
// Author:      VanessaNessie
// Created:     19/01/2009 16:07:35
// Description: AppAbout class declaration
//
//---------------------------------------------------------------------------

#ifndef __APPABOUT_h__
#define __APPABOUT_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/dialog.h>
#else
	#include <wx/wxprec.h>
#endif

//Do not add custom headers between 
//Header Include Start and Header Include End.
//wxDev-C++ designer will remove them. Add custom headers after the block.
////Header Include Start
#include <wx/splitter.h>
////Header Include End

////Dialog Style Start
#undef AppAbout_STYLE
#define AppAbout_STYLE wxCAPTION | wxSYSTEM_MENU | wxDIALOG_NO_PARENT | wxMINIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End

class AppAbout : public wxDialog
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		AppAbout(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("About"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = AppAbout_STYLE);
		virtual ~AppAbout();
	
	private:
		//Do not add custom control declarations between 
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		wxSplitterWindow *WxSplitterWindow1;
		////GUI Control Declaration End
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_WXSPLITTERWINDOW1 = 1001,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
	
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
};

#endif
