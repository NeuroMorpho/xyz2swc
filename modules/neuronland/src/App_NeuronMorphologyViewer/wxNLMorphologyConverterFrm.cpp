//---------------------------------------------------------------------------
//
// Name:        wxNLMorphologyConverterFrm.cpp
// Author:      Satsumatwig
// Created:     19/01/2009 14:53:24
// Description: wxNLMorphologyConverterFrm class implementation
//
//---------------------------------------------------------------------------

#include "wxNLMorphologyConverterFrm.h"
//NL custom INCLUDES
#include "xwNLMorphoConvAbout.h"
#include "NeuronGLCanvas.h"
#include "nlNeuronComponentsInfoPanel.h"
//#include "nlNeuronFormatSelectDlg.h"
#include "nlApplicationSettings.h"


#include "wx/stdpaths.h"


#include "NeuronApplicationLogic/ApplicationLogic.h"
#include "NeuronMorphologyFormat/MorphologyHandlerID.h"
#include "NeuronMorphologyFormat/MorphologyFormatID.h"



//Do not add custom headers between
//Header Include Start and Header Include End
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End


//----------------------------------------------------------------------------
// wxNLMorphologyConverterFrm
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(wxNLMorphologyConverterFrm,wxFrame)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(wxNLMorphologyConverterFrm::OnClose)
	EVT_MENU(ID_MENU_OPENNEURON, wxNLMorphologyConverterFrm::MenuOpenNeuronClick)
	EVT_MENU(ID_MNU_EXPORTORIGINALAS, wxNLMorphologyConverterFrm::MenuExportOriginalAsClick)
	EVT_MENU(ID_MNU_EXPORTCURRENTSTATEAS_1030, wxNLMorphologyConverterFrm::MenuExportCurrentStateAsClick)
	EVT_MENU(IdMenuFileExit, wxNLMorphologyConverterFrm::MnuFileExitClick)
	EVT_MENU(ID_MNU_VIEWTREEDATA, wxNLMorphologyConverterFrm::MnuviewNeuronComponentsViewCheck)
	EVT_MENU(ID_MENU_ABOUT, wxNLMorphologyConverterFrm::MenuAboutClick)
END_EVENT_TABLE()
////Event Table End

wxNLMorphologyConverterFrm::wxNLMorphologyConverterFrm(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style),mNeuronGLCanvas(NULL),   
  mNeuronComponentsDataPanel(NULL)
{
	CreateGUIControlsVane();

	InitialiseApplicationLogic();
}

wxNLMorphologyConverterFrm::~wxNLMorphologyConverterFrm()
{
}

void wxNLMorphologyConverterFrm::CreateGUIControls()
{

	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	wxArrayString arrayStringFor_WxCheckListBox1;
	WxCheckListBox1 = new wxCheckListBox(this, ID_WXCHECKLISTBOX1, wxPoint(318,18), wxSize(2,2), arrayStringFor_WxCheckListBox1, wxLB_SINGLE, wxDefaultValidator, wxT("WxCheckListBox1"));
	WxCheckListBox1->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Segoe UI")));

	WxStatusBarMain = new wxStatusBar(this, ID_WXSTATUSBARMAIN);

	WxMenuBar1 = new wxMenuBar();
	wxMenu *IdMenuFile_Mnu_Obj = new wxMenu(0);
	IdMenuFile_Mnu_Obj->Append(ID_MENU_OPENNEURON, wxT("&Open Neuron\tCtrl+O"), wxT("Loads a Neuron data from a file"), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->AppendSeparator();
	IdMenuFile_Mnu_Obj->Append(ID_MNU_SAVEIMAGE, wxT("&Save Image \tCtrl+S"), wxT(""), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->Append(ID_MNU_CLOSEWORKSPACE, wxT("&Close Neuron"), wxT(""), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->AppendSeparator();
	
	wxMenu *ID_MNU_IMPORTNEURON_Mnu_Obj = new wxMenu(0);
	ID_MNU_IMPORTNEURON_Mnu_Obj->Append(ID_MNU_IMPORTFROMFILE, wxT("Import from file\tCtrl+I"), wxT(""), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->Append(ID_MNU_IMPORTNEURON, wxT("&Import Neuron"), ID_MNU_IMPORTNEURON_Mnu_Obj);
	
	wxMenu *ID_MNU_EXPORT_Mnu_Obj = new wxMenu(0);
	ID_MNU_EXPORT_Mnu_Obj->Append(ID_MNU_EXPORTORIGINALAS, wxT("Export Original Neuron as"), wxT(""), wxITEM_NORMAL);
	ID_MNU_EXPORT_Mnu_Obj->Append(ID_MNU_EXPORTCURRENTSTATEAS_1030, wxT("Export Current State as"), wxT(""), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->Append(ID_MNU_EXPORT, wxT("&Export Neuron"), ID_MNU_EXPORT_Mnu_Obj);
	IdMenuFile_Mnu_Obj->AppendSeparator();
	IdMenuFile_Mnu_Obj->Append(IdMenuFileExit, wxT("E&xit"), wxT(""), wxITEM_NORMAL);
	WxMenuBar1->Append(IdMenuFile_Mnu_Obj, wxT("&File"));
	
	wxMenu *IdMenuEdit_Mnu_Obj = new wxMenu(0);
	WxMenuBar1->Append(IdMenuEdit_Mnu_Obj, wxT("&Edit"));
	wxMenu *ID_MENU_VIEW_Mnu_Obj = new wxMenu(0);
	ID_MENU_VIEW_Mnu_Obj->Append(ID_MNU_VIEWTREEDATA, wxT("&Tree Data"), wxT(""), wxITEM_CHECK);
	ID_MENU_VIEW_Mnu_Obj->Check(ID_MNU_VIEWTREEDATA,true);
	ID_MENU_VIEW_Mnu_Obj->AppendSeparator();
	ID_MENU_VIEW_Mnu_Obj->Append(ID_MNU_VIEWLAYOUT3D, wxT("Layout &3D"), wxT(""), wxITEM_RADIO);
	ID_MENU_VIEW_Mnu_Obj->Check(ID_MNU_VIEWLAYOUT3D,false);
	ID_MENU_VIEW_Mnu_Obj->Append(ID_MNU_LAYOUTX_1035, wxT("Layout X"), wxT(""), wxITEM_RADIO);
	ID_MENU_VIEW_Mnu_Obj->Check(ID_MNU_LAYOUTX_1035,false);
	ID_MENU_VIEW_Mnu_Obj->AppendSeparator();
	ID_MENU_VIEW_Mnu_Obj->Append(ID_MNU_STATUSBAR, wxT("Status b&ar"), wxT(""), wxITEM_CHECK);
	ID_MENU_VIEW_Mnu_Obj->Check(ID_MNU_STATUSBAR,false);
	WxMenuBar1->Append(ID_MENU_VIEW_Mnu_Obj, wxT("&View"));
	
	wxMenu *ID_MNU_ADD_ONS_Mnu_Obj = new wxMenu(0);
	ID_MNU_ADD_ONS_Mnu_Obj->Append(ID_MNU_SCRIPTMANAGER, wxT("Script &Manager"), wxT(""), wxITEM_NORMAL);
	ID_MNU_ADD_ONS_Mnu_Obj->Append(ID_MNU_PLUGINMANAGER, wxT("Plugin Manager"), wxT(""), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_MNU_ADD_ONS_Mnu_Obj, wxT("&Add-ons"));
	
	wxMenu *ID_MENU_WINDOW_Mnu_Obj = new wxMenu(0);
	WxMenuBar1->Append(ID_MENU_WINDOW_Mnu_Obj, wxT("&Window"));
	wxMenu *ID_MENU_HELP_Mnu_Obj = new wxMenu(0);
	ID_MENU_HELP_Mnu_Obj->Append(ID_MENU_CONTENT, wxT("&Content\tF1"), wxT("Open help file"), wxITEM_NORMAL);
	ID_MENU_HELP_Mnu_Obj->AppendSeparator();
	ID_MENU_HELP_Mnu_Obj->Append(ID_MENU_ABOUT, wxT("&About ...\tShift+F1"), wxT(""), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_MENU_HELP_Mnu_Obj, wxT("&Help"));
	SetMenuBar(WxMenuBar1);

	SetStatusBar(WxStatusBarMain);
	SetTitle(wxT("NL Morphology Viewer"));
	SetIcon(wxNullIcon);
	SetSize(8,8,640,480);
	Center();
	
    ////GUI Items Creation End
}


void wxNLMorphologyConverterFrm::CreateGUIControlsVane()
{ 
 
//	WxPanel1 = new wxPanel(this, ID_WXPANEL1, wxPoint(0,0), wxSize(524,417));

	WxStatusBarMain = new wxStatusBar(this, ID_WXSTATUSBARMAIN);
	   
	wxBoxSizer *mainbox = new wxBoxSizer(wxHORIZONTAL);

    mNeuronComponentsDataPanel = new NlNeuronComponentsInfoPanel(this,ID_WXNEURONCOMPSDATA,  wxPoint(0,0), wxSize(280,350));
   
	wxPanel* canvasPanel = new wxPanel(this);
	
	mainbox->Add(mNeuronComponentsDataPanel, 0, wxEXPAND | wxALL | wxFIXED_MINSIZE, 2);
    mainbox->Add(canvasPanel, 1, wxEXPAND | wxALL, 2);
	
	//Create Sizer for the NeuronCanvasGL
	wxBoxSizer *canvasSizer = new wxBoxSizer(wxHORIZONTAL);
	mNeuronGLCanvas =new NeuronGLCanvas(canvasPanel, ID_WXNEURONCANVAS);
	//wxNeuronGLCanvas->Reparent( canvasPanel );
	canvasSizer->Add(mNeuronGLCanvas, 1,wxALIGN_CENTER |wxEXPAND | wxALL, 2);
	canvasPanel->SetSizer(canvasSizer);
	
    CreateMenus();
  
    SetStatusBar(WxStatusBarMain);
	SetTitle(wxT("NL Morphology Viewer"));
	SetIcon(wxNullIcon);

    SetSizer(mainbox);
    SetMinSize(wxSize(APP_MIN_WIDTH, APP_MIN_HEIGHT));
	this->SetAutoLayout(true);
	
	Center();
	/*
	  CreateMenus();
	 
	WxSplitterWindowMain   = new wxSplitterWindow(this, ID_WXSPLITTERWINDOWMAIN);
	WxSplitterWindowMain->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Segoe UI")));

    mNeuronComponentsDataPanel = new NlNeuronComponentsInfoPanel(WxSplitterWindowMain);
    wxNeuronGLCanvas =new NeuronGLCanvas(WxSplitterWindowMain, ID_WXNEURONCANVAS);// wxPoint(127,5), wxSize(200,97));
    WxSplitterWindowMain->Initialize(mNeuronComponentsDataPanel);
	WxSplitterWindowMain->SplitVertically(mNeuronComponentsDataPanel,wxNeuronGLCanvas);
   
  
    SetStatusBar(WxStatusBarMain);
	SetTitle(wxT("NL Morphology Viewer"));
	SetIcon(wxNullIcon);
	  SetMinSize(wxSize(640, 480));    
	Center();
	*/

}

void wxNLMorphologyConverterFrm::CreateMenus()
{
    
    WxMenuBar1 = new wxMenuBar();
	wxMenu *IdMenuFile_Mnu_Obj = new wxMenu(0);
	IdMenuFile_Mnu_Obj->Append(ID_MENU_OPENNEURON, wxT("&Open Neuron\tCtrl+O"), wxT("Loads a Neuron data from a file"), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->AppendSeparator();
	IdMenuFile_Mnu_Obj->Append(ID_MNU_SAVEIMAGE, wxT("&Save Image \tCtrl+S"), wxT(""), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->Append(ID_MNU_CLOSEWORKSPACE, wxT("&Close Neuron"), wxT(""), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->AppendSeparator();

	wxMenu *ID_MNU_IMPORTNEURON_Mnu_Obj = new wxMenu(0);
	ID_MNU_IMPORTNEURON_Mnu_Obj->Append(ID_MNU_IMPORTFROMFILE, wxT("Import from file\tCtrl+I"), wxT(""), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->Append(ID_MNU_IMPORTNEURON, wxT("&Import Neuron"), ID_MNU_IMPORTNEURON_Mnu_Obj);

	wxMenu *ID_MNU_EXPORT_Mnu_Obj = new wxMenu(0);
	ID_MNU_EXPORT_Mnu_Obj->Append(ID_MNU_EXPORTORIGINALAS, wxT("Export Original Neuron as"), wxT(""), wxITEM_NORMAL);
	ID_MNU_EXPORT_Mnu_Obj->Append(ID_MNU_EXPORTCURRENTSTATEAS_1030, wxT("Export Current State as"), wxT(""), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->Append(ID_MNU_EXPORT, wxT("&Export Neuron"), ID_MNU_EXPORT_Mnu_Obj);
	IdMenuFile_Mnu_Obj->AppendSeparator();
	IdMenuFile_Mnu_Obj->Append(IdMenuFileExit, wxT("E&xit"), wxT(""), wxITEM_NORMAL);
	WxMenuBar1->Append(IdMenuFile_Mnu_Obj, wxT("&File"));

	wxMenu *IdMenuEdit_Mnu_Obj = new wxMenu(0);
	WxMenuBar1->Append(IdMenuEdit_Mnu_Obj, wxT("&Edit"));
	wxMenu *ID_MENU_VIEW_Mnu_Obj = new wxMenu(0);
	ID_MENU_VIEW_Mnu_Obj->Append(ID_MNU_VIEWTREEDATA, wxT("&Neuron Components Data"), wxT(""), wxITEM_CHECK);
	ID_MENU_VIEW_Mnu_Obj->Check(ID_MNU_VIEWTREEDATA,true);
	ID_MENU_VIEW_Mnu_Obj->AppendSeparator();
	ID_MENU_VIEW_Mnu_Obj->Append(ID_MNU_VIEWLAYOUT3D, wxT("Layout &3D"), wxT(""), wxITEM_RADIO);
	ID_MENU_VIEW_Mnu_Obj->Check(ID_MNU_VIEWLAYOUT3D,false);
	ID_MENU_VIEW_Mnu_Obj->Append(ID_MNU_LAYOUTX_1035, wxT("Layout X"), wxT(""), wxITEM_RADIO);
	ID_MENU_VIEW_Mnu_Obj->Check(ID_MNU_LAYOUTX_1035,false);
	ID_MENU_VIEW_Mnu_Obj->AppendSeparator();
	ID_MENU_VIEW_Mnu_Obj->Append(ID_MNU_STATUSBAR, wxT("Status b&ar"), wxT(""), wxITEM_CHECK);
	ID_MENU_VIEW_Mnu_Obj->Check(ID_MNU_STATUSBAR,false);
	WxMenuBar1->Append(ID_MENU_VIEW_Mnu_Obj, wxT("&View"));

	wxMenu *ID_MNU_ADD_ONS_Mnu_Obj = new wxMenu(0);
	ID_MNU_ADD_ONS_Mnu_Obj->Append(ID_MNU_SCRIPTMANAGER, wxT("Script &Manager"), wxT(""), wxITEM_NORMAL);
	ID_MNU_ADD_ONS_Mnu_Obj->Append(ID_MNU_PLUGINMANAGER, wxT("Plugin Manager"), wxT(""), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_MNU_ADD_ONS_Mnu_Obj, wxT("&Add-ons"));

	wxMenu *ID_MENU_WINDOW_Mnu_Obj = new wxMenu(0);
	WxMenuBar1->Append(ID_MENU_WINDOW_Mnu_Obj, wxT("&Window"));
	wxMenu *ID_MENU_HELP_Mnu_Obj = new wxMenu(0);
	ID_MENU_HELP_Mnu_Obj->Append(ID_MENU_CONTENT, wxT("&Content\tF1"), wxT("Open help file"), wxITEM_NORMAL);
	ID_MENU_HELP_Mnu_Obj->AppendSeparator();
	ID_MENU_HELP_Mnu_Obj->Append(ID_MENU_ABOUT, wxT("&About ...\tShift+F1"), wxT(""), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_MENU_HELP_Mnu_Obj, wxT("&Help"));
	SetMenuBar(WxMenuBar1);

}

void wxNLMorphologyConverterFrm::CreateNeuronTextDataWindow()
{
    	//NeuronSpecific: Create the canvas for the components data
   // 	 mNeuronComponentsDataWindow = new NeuronTxtDataFrm(WxSplitterWindowMain);
//	wxNeuronGLCanvas =new NeuronGLCanvas(WxSplitterWindowMain, ID_WXNEURONCANVAS,
  //  wxPoint(127,5), wxSize(200,97));
    //WxSplitterWindowMain->SplitVertically(WxSWNeuronData,wxNeuronGLCanvas,50);


  //   WxSplitterWindowMain->SetSashGravity(0.8);
  //   	WxSplitterWindowMain->SetMinimumPaneSize(200);
     //	WxSplitterWindowMain->
  //   WxSplitterWindowMain->UpdateSize();
        
}

void wxNLMorphologyConverterFrm::OnClose(wxCloseEvent& event)
{
	Destroy();
}

/*
 * MnuFileExitClick
 */
void wxNLMorphologyConverterFrm::MnuFileExitClick(wxCommandEvent& event)
{
	// Close the application
	Close();
}

/*
 * MenuAboutClick
 */
void wxNLMorphologyConverterFrm::MenuAboutClick(wxCommandEvent& event)
{
	xwNLMorphoConvAbout dlg(this);
   // if ( dlg.ShowModal() == wxID_OK )

}


/*
 * MenuOpenNeuronClick
 */
void wxNLMorphologyConverterFrm::MenuOpenNeuronClick(wxCommandEvent& event)
{
	wxFileDialog WxOpenFileDialog(this, _("Choose a Neuron File"), wxEmptyString, wxEmptyString, 
    wxEmptyString, wxOPEN |wxFILE_MUST_EXIST);

	if(WxOpenFileDialog.ShowModal() == wxID_OK)
	{		
		
		MorphologyDataManager::NeuronID id = g_applicationLogic.LoadNeuron( StringRef( WxOpenFileDialog.GetPath().mb_str() ) );
		this->InitialiseNeuronComponentsGraphicalData(id );
		if(id >= 0)
		{
			NeuronGLCanvas* pCanvas = GetOrCreateNeuronGLCanvas(WxOpenFileDialog.GetFilename());
			assert(pCanvas);
			pCanvas->SetCell(id);
			
			// Temp fake function to loa fke data. it will have to be loaded from the neuron data 
			mNeuronComponentsDataPanel->LoadNeuronItemListData();

		}
	}     
}


bool wxNLMorphologyConverterFrm::ExportDataToFile(const wxString &fileName)
{
	return false;//g_applicationLogic.SaveNeuron( MorphologyHandlerID("NeurolucidaV3"), 0, String(fileName.mb_str()) );
}

/*
 * MenuExportCurrentStateAsClick
 */
void wxNLMorphologyConverterFrm::MenuExportCurrentStateAsClick(wxCommandEvent& event)
{
    wxFileDialog SaveDialog (this, _("Export Data As _?"), wxEmptyString, wxEmptyString,
		kNlNeuronFormats,	wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

	// Creates a Save Dialog with 4 file types
	if (SaveDialog.ShowModal() == wxID_OK) // If the user clicked "OK"
	{
        if (ExportDataToFile(SaveDialog.GetPath()))
		{
    		// Set the Title to reflect the file open
    		SetTitle(wxString(wxT("Edit - ")) << SaveDialog.GetFilename());
        }
        else
        {
             wxMessageBox(SaveDialog.GetPath(), wxT("Error Exporting Neuron to."),
              wxICON_INFORMATION);
        }
	}
}

/*
 * MenuExportOriginalAsClick
 */
void wxNLMorphologyConverterFrm::MenuExportOriginalAsClick(wxCommandEvent& event)
{
    // NlNeuronFormatSelectDlg formatSelect  (this);


	 wxFileDialog SaveDialog (this, _("Export Data As _?"), wxEmptyString, wxEmptyString,
	//	_("Format 1 (*.f1)|*.f1|Format 2 (*.f2)|*.f2"),	
		kNlNeuronFormats,
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

	// Creates a Save Dialog with 4 file types
	if (SaveDialog.ShowModal() == wxID_OK) // If the user clicked "OK"
	{
        if (ExportDataToFile(SaveDialog.GetPath()))
		{
           // SetCurrentPath( SaveDialog.GetPath() );
    		// Set the Title to reflect the file open
    		SetTitle(wxString(wxT("Edit - ")) << SaveDialog.GetFilename());
        }
        else
        {
             wxMessageBox(SaveDialog.GetPath(), wxT("Error Exporting Original Neuron to."),
              wxICON_INFORMATION);
        }
	}
}



/*
 * MnuviewNeuronComponentsViewCheck
 */
void wxNLMorphologyConverterFrm::MnuviewNeuronComponentsViewCheck(wxCommandEvent& event)
{
    static bool panelVisible=true;
    panelVisible= ! GetSizer()->IsShown(mNeuronComponentsDataPanel);
	GetSizer()->Show(mNeuronComponentsDataPanel, panelVisible, true); //show/hide recursively	
	GetSizer()->Layout();
}


void wxNLMorphologyConverterFrm::InitialiseApplicationLogic()
{
	wxStandardPaths stdPaths;
	wxString filePath = stdPaths.GetExecutablePath();

	String pathDir = String(filePath.mbc_str()).beforeLast("bin");
	//root path
	g_applicationLogic.Initialise(pathDir);
}

bool wxNLMorphologyConverterFrm::InitialiseNeuronComponentsGraphicalData(MorphologyDataManager::NeuronID id )
{
	NLNeuronDisplayElements& neuronElementsDisplaySettings=GetNeuronElementsDisplaySettings();
	int numcomponents = 10; //@TODO obtain from the neuron data 
	//for each element in the neuron,initialize its display data.
	for (int i=0; i< numcomponents; i++)
	{
		//initialy the values will be de default ones
		NeuronItemDisplayData elementdata;		
		neuronElementsDisplaySettings[i]=elementdata;
	}
}

NeuronGLCanvas* wxNLMorphologyConverterFrm::GetOrCreateNeuronGLCanvas(wxString name)
{
	return mNeuronGLCanvas;
}