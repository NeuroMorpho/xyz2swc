//---------------------------------------------------------------------------
//
// Name:        wxNLMorphologyConverterFrm.h
// Author:      Satsumatwig
// Created:     19/01/2009 14:53:24
// Description: wxNLMorphologyConverterFrm class declaration
//
//---------------------------------------------------------------------------

#ifndef __WXNLMORPHOLOGYCONVERTERFRM_h__
#define __WXNLMORPHOLOGYCONVERTERFRM_h__

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
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/checklst.h>
////Header Include End

#include "NeuronRepresentation/NeuronComponentDisplayProperties.h"
//#include "nlNeuronComponentDisplayProperties.h"
#include "NeuronMorphologyDataManager/MorphologyDataManager.h"
//#include "NeuronTxtDataFrm.h"
////Dialog Style Start
#undef wxNLMorphologyConverterFrm_STYLE
#define wxNLMorphologyConverterFrm_STYLE wxCAPTION | wxRESIZE_BORDER | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End



//Forward declaration
class NeuronGLCanvas;
class NlNeuronComponentsInfoPanel;
class wxHtmlHelpController;

//! global help provider
extern wxHtmlHelpController *g_help;

class wxNLMorphologyConverterFrm : public wxFrame
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		wxNLMorphologyConverterFrm(wxWindow *parent, wxWindowID id = 1, 
        const wxString &title = _("NL Morphology Viewer"),
         const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
          long style = wxNLMorphologyConverterFrm_STYLE);
          
	virtual ~wxNLMorphologyConverterFrm();
	/**
	* Close and Exit.
	* 
	*/
	void MnuFileExitClick(wxCommandEvent& event);
	/**
	* Open About Dialog
	*/	
	void MenuAboutClick(wxCommandEvent& event);

    /**
    * Import Neuron Data File
    */
	void MenuOpenNeuronClick(wxCommandEvent& event);
	
	/**
	* Export the current neuron settings to the selected format
	*/
	void MenuExportCurrentStateAsClick(wxCommandEvent& event);
	
	/**
	* Export the orginial imported neuron data to a file with the selected format.
	*/
	void MenuExportOriginalAsClick(wxCommandEvent& event);

	/**
	* Import Neuron Data File
	*/
	void MenuCloseNeuronClick(wxCommandEvent& event);

    /**
    * Show/Hide Neuron Data panel
    */
	void MnuviewNeuronComponentsViewCheck(wxCommandEvent& event);


	void MnuviewNeuronViewPointsSelect(wxCommandEvent& event);
	void MnuviewNeuronViewLinesSelect(wxCommandEvent& event);
		
	private:
		wxMenuBar *WxMenuBar1;
		wxStatusBar *WxStatusBarMain;
		wxCheckListBox *WxCheckListBox1;
		////GUI Control Declaration End
		//nlMorfo specifics , OpenGL canvas   
    
		MorphologyDataManager::NeuronID mNeuronId; 
		NeuronGLCanvas *mNeuronGLCanvas;
		NlNeuronComponentsInfoPanel*  mNeuronComponentsDataPanel;

	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			IdMenuFile = 1001,
			ID_MENU_OPENNEURON = 1015,
			ID_MNU_SAVEIMAGE = 1016,
			ID_MNU_CLOSENEURON = 1006,
			ID_MNU_IMPORTNEURON = 1019,
			//ID_MNU_IMPORTFROMFILE = 1021,
			ID_MNU_EXPORT = 1020,
			ID_MNU_EXPORTORIGINALAS = 1023,
			ID_MNU_EXPORTCURRENTSTATEAS_1030 = 1030,
			IdMenuFileExit = 1008,
			IdMenuEdit = 1002,
			ID_MENU_VIEW = 1009,
			ID_MNU_VIEWTREEDATA = 1028,
			ID_MNU_VIEWLAYOUTPOINTS = 1033,
			ID_MNU_VIEWLAYOUTLINES = 1035,
			ID_MNU_STATUSBAR = 1029,
			ID_MNU_ADD_ONS = 1024,
			ID_MNU_SCRIPTMANAGER = 1025,
			ID_MNU_PLUGINMANAGER = 1026,
			ID_MENU_WINDOW = 1010,
			ID_MENU_HELP = 1011,
			ID_MENU_CONTENT = 1012,
			ID_MENU_ABOUT = 1014,
			
			ID_WXSTATUSBARMAIN = 1055,
			ID_WXCHECKLISTBOX1 = 1054,
			////GUI Enum Control ID End
			ID_WXNEURONCANVAS,
			ID_WXSPLITTERWINDOWMAIN = 1056,
			ID_WXNEURONCOMPSDATA,
			ID_WXNEURONMAINPANEL,
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
		
	private:
		void UpdateTitle ();
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();

		wxRect DetermineFrameSize () ;
		
		/**
    		Exports data to the given file .		
    		@return true if successful
		*/
        //bool ExportDataToFile(const wxString &fileName);
        
        /**
        Create the Neuron Components data window ,places it on the left of the split window.
        */
        void CreateNeuronTextDataWindow();
        /**
        Retrieves the current NeuronGLCanvas, if it is null creates it first
        */
        //NeuronGLCanvas* GetOrCreateNeuronGLCanvas(wxString name);
		
		/************************************************************************/
		/* Reset current neuron data
		/* Clear neuron panels, reset neuron asociated logic/display data
		/************************************************************************/
	void CloseNeuron ();
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
    void CreateMenus();

	/************************************************************************/
	/* Initializes g_applicationLogic                                       */
	/************************************************************************/
	void InitialiseApplicationLogic();
	
	/**
	 * Load the neuron components visual settings (colour,state, ...) i
	 nto mNeuronElementsDisplaySettings
	 */
	//bool InitialiseNeuronComponentsGraphicalData(MorphologyDataManager::NeuronID id );
	
	NeuronGLCanvas* GetOrCreateNeuronGLCanvas(wxString name);

	/************************************************************************/
	/* Return true if there is a neuron active (mNeuronID != -1             */
	/************************************************************************/
	bool  wxNLMorphologyConverterFrm::NeuronLoaded ();

	/************************************************************************/
	/* Enable/disable menus                                                 */
	/************************************************************************/
	void  UpdateMenus ();

	//Help Related functions
	void OnHelp (wxCommandEvent &event);
	wxHtmlHelpController& GetHelpController() {return *g_help; }

};


inline bool  wxNLMorphologyConverterFrm::NeuronLoaded () 
{
	return mNeuronId != -1;
}



#endif
