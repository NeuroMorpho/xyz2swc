//---------------------------------------------------------------------------
//
// Name:        NeuronTxtDataFrm.h
// Author:      VanessaNessie
// Created:     23/01/2009 11:15:29
// Description: NeuronTxtDataFrm class declaration
//
//---------------------------------------------------------------------------

#ifndef __NEURONTXTDATAFRM_h__
#define __NEURONTXTDATAFRM_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/frame.h>
#else
	#include <wx/wxprec.h>
#endif

//Do not add custom headers between 
//Header Include Start and Header Include End.
//wxDev-C++ designer will remove them. Add custom headers after the block.
////Header Include Start
#include <wx/statbox.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
////Header Include End

////Dialog Style Start
#undef NeuronTxtDataFrm_STYLE
#define NeuronTxtDataFrm_STYLE wxRESIZE_BORDER | wxCLOSE_BOX
////Dialog Style End

class NeuronTxtDataFrm : public wxFrame
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		NeuronTxtDataFrm(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("NeuronTxtDataFrm"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = NeuronTxtDataFrm_STYLE);
		virtual ~NeuronTxtDataFrm();
		void WxButton1Click(wxCommandEvent& event);
		void WxButtonComponentMarkersClick(wxCommandEvent& event);
		void WxButtonShowCompClick(wxCommandEvent& event);
		void WxButtonShowMarkersClick(wxCommandEvent& event);
		void WxButtonCompColourClick(wxCommandEvent& event);
		void WxListBox1Selected(wxCommandEvent& event);
		void WxListCtrl1Selected(wxListEvent& event);
		void WxListCtrl1ItemFocused(wxListEvent& event);
		void WxListCtrl1Deselected(wxListEvent& event);
		
	private:
		//Do not add custom control declarations between
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		wxStaticBoxSizer *WxStaticBoxSizer1;
		wxStaticBox *WxStaticBox1;
		wxListCtrl *WxListCtrl1;
		wxStaticBoxSizer *WxStaticBoxSizerComponentStats;
		wxButton *WxButtonCompColour;
		wxButton *WxButtonShowMarkers;
		wxButton *WxButtonShowComp;
		wxBoxSizer *WxBoxSizerCompButtons;
		wxListBox *WxListBoxNeuronComponents;
		wxBoxSizer *WxBoxSizer2;
		wxStaticBoxSizer *WxStaticBoxSizerComponetsProp;
		wxBoxSizer *WxBoxSizerComponentsList;
		wxBoxSizer *WxBoxSizer1;
		////GUI Control Declaration End
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_WXSTATICBOX1 = 1041,
			ID_WXLISTCTRL1 = 1040,
			ID_WXBUTTONCOMPCOLOUR = 1030,
			ID_WXBUTTONSHOWMARKERS = 1029,
			ID_WXBUTTONSHOWCOMP = 1028,
			ID_WXLISTBOXNEURONCOMPONENTS = 1025,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
		
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
};

#endif
