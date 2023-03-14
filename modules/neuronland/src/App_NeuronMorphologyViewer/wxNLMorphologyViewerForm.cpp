//---------------------------------------------------------------------------
//
// Name:        wxNLMorphologyConverterFrm.cpp
// Author:      Satsumatwig
// Created:     19/01/2009 14:53:24
// Description: wxNLMorphologyConverterFrm class implementation
//
//---------------------------------------------------------------------------

#include <memory>
#include "Core/OutputFile.h"
#include "wxNLMorphologyViewerForm.h"
//NL custom INCLUDES
#include "NeuronGLCanvas.h"
#include "nlNeuronComponentsInfoPanel.h"
#include "nlApplicationSettings.h"

#include "wx/aboutdlg.h"
#include "wx/stdpaths.h"

#include "NeuronApplicationLogic/ApplicationLogic.h"
#include "NeuronMorphologyFormat/MorphologyHandlerID.h"
#include "NeuronMorphologyFormat/MorphologyFormatID.h"
//#include "wxDialogs\wxNeuronExportFormatDialog.h"
#include "NeuronOperations.h"

//help
#include <wx/fs_zip.h>   // ZIP filesystem support
#include <wx/html/helpctrl.h> // html help support
//#include "wx/localize.h"   // localize support
//end help includes
#include <wx/statline.h>

extern wxString g_appname;
wxString GetTitleVersion()
{
	return g_appname + " " + APP_VERSION + " " ;
}

//----------------------------------------------------------------------------
// wxNLMorphologyConverterFrm
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxNLMorphologyConverterFrm,wxFrame)
	EVT_CLOSE(wxNLMorphologyConverterFrm::OnClose)
	EVT_MENU(ID_MENU_OPENNEURON, wxNLMorphologyConverterFrm::MenuOpenNeuronClick)
	EVT_MENU(ID_MNU_EXPORTORIGINALAS, wxNLMorphologyConverterFrm::MenuExportOriginalAsClick)
	EVT_MENU(ID_MNU_EXPORTCURRENTSTATEAS_1030, wxNLMorphologyConverterFrm::MenuExportCurrentStateAsClick)
	EVT_MENU(ID_MNU_CLOSENEURON, wxNLMorphologyConverterFrm::MenuCloseNeuronClick)
	EVT_MENU(IdMenuFileExit, wxNLMorphologyConverterFrm::MnuFileExitClick)
	EVT_MENU(ID_MNU_VIEWTREEDATA, wxNLMorphologyConverterFrm::MnuviewNeuronComponentsViewCheck)	
	EVT_MENU(ID_MENU_ABOUT, wxNLMorphologyConverterFrm::MenuAboutClick)
	EVT_MENU(ID_MENU_CONTENT, wxNLMorphologyConverterFrm::OnHelp)
	EVT_MENU(ID_MNU_VIEWLAYOUTPOINTS, wxNLMorphologyConverterFrm::MnuviewNeuronViewPointsSelect)
	EVT_MENU(ID_MNU_VIEWLAYOUTLINES, wxNLMorphologyConverterFrm::MnuviewNeuronViewLinesSelect)
END_EVENT_TABLE()


wxNLMorphologyConverterFrm::wxNLMorphologyConverterFrm(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style),mNeuronGLCanvas(NULL),   
  mNeuronComponentsDataPanel(NULL),mNeuronId(-1)
{
	CreateGUIControls();

	InitialiseApplicationLogic();
}

wxNLMorphologyConverterFrm::~wxNLMorphologyConverterFrm()
{
	delete mNeuronGLCanvas;
	delete mNeuronComponentsDataPanel;
}

void wxNLMorphologyConverterFrm::CreateGUIControls()
{ 
 
	//wxPanel* mainpanel = new wxPanel(this, -1);//, wxPoint(0,0), wxSize(524,417));

	WxStatusBarMain = new wxStatusBar(this, ID_WXSTATUSBARMAIN);
	int widths[3] = {-3,-2,-1};
	WxStatusBarMain->SetFieldsCount(3,widths );
	wxBoxSizer *mainbox = new wxBoxSizer(wxHORIZONTAL);

	mNeuronComponentsDataPanel = new NlNeuronComponentsInfoPanel(this,ID_WXNEURONCOMPSDATA,  wxPoint(0,0), wxSize(280,350));
   	wxPanel* canvasPanel = new wxPanel(this);

	mainbox->Add(mNeuronComponentsDataPanel, 0, wxEXPAND | wxALL | wxFIXED_MINSIZE, 0);
    mainbox->Add(canvasPanel, 1, wxEXPAND | wxALL, 0);
	
	//Create Sizer for the NeuronCanvasGL
	wxBoxSizer *canvasSizer = new wxBoxSizer(wxHORIZONTAL);
	mNeuronGLCanvas =new NeuronGLCanvas(canvasPanel, ID_WXNEURONCANVAS);
	//wxNeuronGLCanvas->Reparent( canvasPanel );
	canvasSizer->Add(mNeuronGLCanvas, 1,wxALIGN_CENTER |wxEXPAND | wxALL, 2);
	canvasPanel->SetSizer(canvasSizer);
	
    CreateMenus();
  
    SetStatusBar(WxStatusBarMain);
	SetTitle(GetTitleVersion());
	SetIcon(wxNullIcon);

    SetSizer(mainbox);
    SetMinSize(wxSize(APP_MIN_WIDTH, APP_MIN_HEIGHT));
	SetSize (DetermineFrameSize ());

	WxStatusBarMain->SetStatusText(_("Ready"));
	this->SetAutoLayout(true);
//	GetSizer()->Layout();
	Center();


}

void wxNLMorphologyConverterFrm::CreateMenus()
{
    
    WxMenuBar1 = new wxMenuBar();
	wxMenu *IdMenuFile_Mnu_Obj = new wxMenu(0);
	IdMenuFile_Mnu_Obj->Append(ID_MENU_OPENNEURON, _("&Import Neuron\tCtrl+O"), _("Loads Neuron Data from a File"), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->AppendSeparator();

	wxMenu *ID_MNU_EXPORT_Mnu_Obj = new wxMenu(0);
	ID_MNU_EXPORT_Mnu_Obj->Append(ID_MNU_EXPORTORIGINALAS, _("Export Original Neuron As"), _("Export Original Neuron Data to File"), wxITEM_NORMAL);
	ID_MNU_EXPORT_Mnu_Obj->Append(ID_MNU_EXPORTCURRENTSTATEAS_1030, _("Export Visible Neuron As"), _("Export Modified Neuron Data to File"), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->Append(ID_MNU_EXPORT, _("&Export Neuron"), ID_MNU_EXPORT_Mnu_Obj);
	
	IdMenuFile_Mnu_Obj->AppendSeparator();
	IdMenuFile_Mnu_Obj->Append(ID_MNU_SAVEIMAGE, _("&Save Image \tCtrl+S"), _("Save Neuron Image As"), wxITEM_NORMAL);
		IdMenuFile_Mnu_Obj->Append(ID_MNU_CLOSENEURON, _("&Close Neuron"), _("Close Neuron"), wxITEM_NORMAL);
	IdMenuFile_Mnu_Obj->AppendSeparator();

	IdMenuFile_Mnu_Obj->Append(IdMenuFileExit, _("E&xit"), _("Exit"), wxITEM_NORMAL);
	WxMenuBar1->Append(IdMenuFile_Mnu_Obj, _("&File"));

	//wxMenu *IdMenuEdit_Mnu_Obj = new wxMenu(0);
	//WxMenuBar1->Append(IdMenuEdit_Mnu_Obj, _("&Edit"));

	wxMenu *ID_MENU_VIEW_Mnu_Obj = new wxMenu(0);
	ID_MENU_VIEW_Mnu_Obj->Append(ID_MNU_VIEWTREEDATA, _("&Neuron Components Data"), _("Show/Hide Neuron Components Text Data Panel"), wxITEM_CHECK);
	ID_MENU_VIEW_Mnu_Obj->Check(ID_MNU_VIEWTREEDATA,true);
	ID_MENU_VIEW_Mnu_Obj->AppendSeparator();
	ID_MENU_VIEW_Mnu_Obj->Append(ID_MNU_VIEWLAYOUTPOINTS, _("Neuron Sample Points"), _("View Original Neuron Sample Points"), wxITEM_RADIO);
	ID_MENU_VIEW_Mnu_Obj->Check(ID_MNU_VIEWLAYOUTPOINTS, false);
	ID_MENU_VIEW_Mnu_Obj->Append(ID_MNU_VIEWLAYOUTLINES, _("Neuron Line Segments"), _("View Neuron Data Connectivity Layout"), wxITEM_RADIO);
	ID_MENU_VIEW_Mnu_Obj->Check(ID_MNU_VIEWLAYOUTLINES,true);
	//ID_MENU_VIEW_Mnu_Obj->AppendSeparator();
	//ID_MENU_VIEW_Mnu_Obj->Append(ID_MNU_STATUSBAR, wxT("Status b&ar"), wxT(""), wxITEM_CHECK);
	//ID_MENU_VIEW_Mnu_Obj->Check(ID_MNU_STATUSBAR,false);
	WxMenuBar1->Append(ID_MENU_VIEW_Mnu_Obj, _("&View"));

	//wxMenu *ID_MNU_ADD_ONS_Mnu_Obj = new wxMenu(0);
	//ID_MNU_ADD_ONS_Mnu_Obj->Append(ID_MNU_SCRIPTMANAGER, _("Script &Manager"), _(""), wxITEM_NORMAL);
	//ID_MNU_ADD_ONS_Mnu_Obj->Append(ID_MNU_PLUGINMANAGER, _("Plugin Manager"), _(""), wxITEM_NORMAL);
	//WxMenuBar1->Append(ID_MNU_ADD_ONS_Mnu_Obj, _("&Add-ons"));

	//wxMenu *ID_MENU_WINDOW_Mnu_Obj = new wxMenu(0);
	//WxMenuBar1->Append(ID_MENU_WINDOW_Mnu_Obj, _("&Window"));
	
	wxMenu *ID_MENU_HELP_Mnu_Obj = new wxMenu(0);
	ID_MENU_HELP_Mnu_Obj->Append(ID_MENU_CONTENT, _("&View User Manual\tF1"), _("Open help file"), wxITEM_NORMAL);
	ID_MENU_HELP_Mnu_Obj->AppendSeparator();
	ID_MENU_HELP_Mnu_Obj->Append(ID_MENU_ABOUT, _("&About ...\tShift+F1"), wxT("View About Information"), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_MENU_HELP_Mnu_Obj, _("&Help"));
	SetMenuBar(WxMenuBar1);

	//Disable not implemented options.
	WxMenuBar1->Enable(ID_MNU_SAVEIMAGE, false);
	//WxMenuBar1->Enable(ID_MNU_SCRIPTMANAGER, false);
	//WxMenuBar1->Enable(ID_MNU_PLUGINMANAGER, false);
	//WxMenuBar1->Enable(ID_MNU_VIEWLAYOUT3D, false);	

	UpdateMenus();

}

wxRect wxNLMorphologyConverterFrm::DetermineFrameSize () 
{
	wxSize scr = wxGetDisplaySize();

	// determine default frame position/size
	wxRect normal;
	if (scr.x <= 640) {
		normal.x = 40 / 2;
		normal.width = scr.x - 40;
	}else{
		normal.x = (scr.x - 640) / 2;
		normal.width = 640;
	}
	if (scr.y <= 480) {
		normal.y = 80 / 2;
		normal.height = scr.y - 80;
	}else{
		normal.y = (scr.y - 400) / 2;
		normal.height = 400;
	}

	return normal;
}



void wxNLMorphologyConverterFrm::OnClose(wxCloseEvent& event)
{
	CloseNeuron();
	Destroy();
}

/*
 * MnuFileExitClick
 */
void wxNLMorphologyConverterFrm::MnuFileExitClick(wxCommandEvent& event)
{
	// CLose whatever needs to be done with the app logic
		CloseNeuron();
	Close();
}

/*
 * MenuAboutClick
 */
void wxNLMorphologyConverterFrm::MenuAboutClick(wxCommandEvent& event)
{
	wxAboutDialogInfo info;
	info.SetName(g_appname);
	info.SetVersion(APP_VERSION);
	info.SetDescription(_("This program is a simple neuron morphology viewer\nand data exporter which uses the import/export\ntechnology developed for the NLMorphologyConverter."));
	info.SetCopyright(APP_COPYRIGTH);	
	wxArrayString developers;
	developers.Add(APP_MAINT);
	developers.Add(APP_MAIN2);
	info.SetDevelopers(developers);
	//info.SetDocWriters(developers);
	info.SetWebSite(APP_WEBSITE);
	//info.SetTranslators(developers);
	wxAboutBox(info);

	//xwNLMorphoConvAbout dlg(this);
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
		CloseNeuron(); //reset any opened neuron data		
		//set status bar message 
		WxStatusBarMain->SetStatusText(_("Loading Neuron ")+ WxOpenFileDialog.GetFilename());
		::wxBeginBusyCursor();
		wxWindow::Update();

		MorphologyDataManager::NeuronID id = g_applicationLogic.LoadNeuron( StringRef( WxOpenFileDialog.GetPath().mb_str() ) );
	
		if(id >= 0)
		{
			//neuron was loaded
			g_applicationLogic.CreateNeuronDisplayData(id);
			NeuronGLCanvas* pCanvas = GetOrCreateNeuronGLCanvas(WxOpenFileDialog.GetFilename());
			assert(pCanvas);
			pCanvas->SetCell(id);
			mNeuronId = id;
			
			// Temp fake function to loa fke data. it will have to be loaded from the neuron data 
			mNeuronComponentsDataPanel->LoadNeuronData(id);
			WxStatusBarMain->SetStatusText(_("Neuron Loaded"));
			WxStatusBarMain->SetStatusText(_("Neuron: ")+WxOpenFileDialog.GetFilename(),1);
			WxStatusBarMain->SetStatusText((string)(g_applicationLogic.GetLastNeuronLoadedFormatString()),2);
			SetTitle (GetTitleVersion() + _T("  [") + WxOpenFileDialog.GetFilename() + "]");
		}
		else 
		{
			wxMessageBox("An unknown error occurred while trying to load Neuron from file \n" + 
				WxOpenFileDialog.GetFilename() , _("Error Importing  Neuron "), wxICON_ERROR);
		}

		::wxEndBusyCursor();
		UpdateMenus () ;
	}     
}


/*bool wxNLMorphologyConverterFrm::ExportDataToFile(const wxString &fileName)
{
	std::auto_ptr<Output> pOut( OutputFile::open(fileName.c_str()) );
	if(pOut.get())
	{
		StyleOptions styleOptions;
		return ExportMorphologyFormat(kMorphologyFormatNeuroZoom, *pOut,
			*g_applicationLogic.GetMorphologyDataManager().GetNeuron3D(mNeuronId), styleOptions);
	}

	return false;
}*/
/*
void wxNLMorphologyConverterFrm::MenuExportCurrentStateAsClick(wxCommandEvent& event)
{
	wxNeuronExportFormatDialog exportDialog(this, wxID_ANY, "test", wxDefaultPosition, wxDefaultSize, 0);

	if(exportDialog.ShowModal() == wxID_OK)
	{
		wxFileDialog fileDialog(this, _("Save neuron in chosen format"), 
			wxT("C://NeuronResearch//Neuron Data Files"), wxEmptyString, 
			wxFileSelectorDefaultWildcardStr, 
			wxFD_SAVE);
		if(fileDialog.ShowModal())
		{
			//ExportMorphologyFormat(eOptions.outputFormat, *pOut, *( (*it).second)) )
			//g_applicationLogic.SaveNeuron(exportDialog.GetMorphologyHandlerID(), 0, String(fileDialog.GetPath().mb_str()) );
		}
	}
}
*/

/************************************************************************/
/*  for a given format, generate a vector with the list of formats
/* it can be exported to and generate the wildstring                    */
/************************************************************************/
 
wxString GetWildCarForExportableMorphologyFormats(std::vector <BuiltInMorphologyFormat>& exportableFormats)
{
	wxString strformats;
	if (exportableFormats.size()>0)
	{
		int i=0;
		strformats << GetBuiltInMorphologyFormatDetails(exportableFormats[i]).GetName()  ;
		strformats << GetBuiltInMorphologyFormatDetails(exportableFormats[i]).GetName()  ;
		strformats <<" (*.*) | *.* ";

		for (i++; i< exportableFormats.size() ;i++)
		{
			strformats << "| "<< GetBuiltInMorphologyFormatDetails(exportableFormats[i]).GetName()  ;
			String extensions = GetBuiltInMorphologyFormatDetails(exportableFormats[i]).GetExtensions ();
			if (extensions.length ()==0 )
				strformats <<" (*.*) | *.*";
			else
				strformats <<" (" << extensions << ") | " << extensions ;
		}
	}
	return strformats;
}
/*
 * MenuExportCurrentStateAsClick
 */
void wxNLMorphologyConverterFrm::MenuExportCurrentStateAsClick(wxCommandEvent& event)
{
	//calculate modified neuron data
	Neuron3D neuron3d=	GenerateModifiedNeuron(mNeuronId, GetNeuronElementsDisplaySettings());
	vector<BuiltInMorphologyFormat> exportableformats;

	if (!GetExportableMorphologyFormats(neuron3d,exportableformats))
	{
		wxMessageBox(_("There are no available formats to export the modified Neuron to."), _("Export Modified Neuron..."),
			wxICON_INFORMATION);
		return;
	}

	//tjere are available formats to extract to.
	wxFileDialog SaveDialog (this, _("Export Modified Neuron As "), wxEmptyString, wxEmptyString,
		GetWildCarForExportableMorphologyFormats(exportableformats),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

	// Creates a Save Dialog with 4 file types
	if (SaveDialog.ShowModal() == wxID_OK) // If the user clicked "OK"
	{
		//set status bar message 
		WxStatusBarMain->SetStatusText(_("Exporting Original Neuron to")+ SaveDialog.GetFilename());
		::wxBeginBusyCursor();
		wxWindow::Update();

		StyleOptions styleOptions;
		styleOptions.appName = APP_NAME;
		styleOptions.appVersion = APP_VERSION;
		styleOptions.fileName = SaveDialog.GetFilename();

		if ( ExportNeuronDataToFile(neuron3d, exportableformats[SaveDialog.GetFilterIndex()], 
			SaveDialog.GetPath().c_str(), styleOptions ) )
		{
			// SetCurrentPath( SaveDialog.GetPath() );
			wxMessageBox(_T("Modified Neuron exported to file ") +SaveDialog.GetPath(), _("Export Modified Neuron..."), wxICON_INFORMATION);;
		}
		else
		{
			wxMessageBox(_T("Error saving Modified Neuron to file ") +SaveDialog.GetPath(), _("Export Modified Neuron to"),
				wxICON_ERROR);
		}
		::wxEndBusyCursor();
		WxStatusBarMain->SetStatusText(_("Ready"));
	}
}


/*
 * MenuExportOriginalAsClick
 */
void wxNLMorphologyConverterFrm::MenuExportOriginalAsClick(wxCommandEvent& event)
{
    // NlNeuronFormatSelectDlg formatSelect  (this);
	Neuron3D& neuron3d=*g_applicationLogic.GetMorphologyDataManager().GetNeuron3D(mNeuronId);
	vector<BuiltInMorphologyFormat> exportableformats;
	
	if (!GetExportableMorphologyFormats(neuron3d,exportableformats))
	{
		wxMessageBox(_("There are no available formats to export the Neuron to."), _("Export Neuron..."),
			wxICON_INFORMATION);
		return;
	}

	//tjere are available formats to extract to.
	wxFileDialog SaveDialog (this, _("Export Data As _?"), wxEmptyString, wxEmptyString,
		GetWildCarForExportableMorphologyFormats(exportableformats),
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

	// Creates a Save Dialog with 4 file types
	if (SaveDialog.ShowModal() == wxID_OK) // If the user clicked "OK"
	{	
		WxStatusBarMain->SetStatusText(_("Exporting Modified Neuron to")+ SaveDialog.GetFilename());
		::wxBeginBusyCursor();
		wxWindow::Update();

		StyleOptions styleOptions;
		styleOptions.appName = APP_NAME;
		styleOptions.appVersion = APP_VERSION;
		styleOptions.fileName = SaveDialog.GetFilename();


		if ( ExportNeuronDataToFile(neuron3d, exportableformats[SaveDialog.GetFilterIndex()], 
				SaveDialog.GetPath().c_str(), styleOptions ))
		{
           // SetCurrentPath( SaveDialog.GetPath() );
			wxMessageBox(_T(" Neuron exported to file ") +SaveDialog.GetPath(), _("Export Neuron..."), wxICON_INFORMATION);;
        }
        else
        {
             wxMessageBox(_T("Error saving Original Neuron to file ") +SaveDialog.GetPath(), _("Export Original Neuron to"),
              wxICON_ERROR);
        }
		WxStatusBarMain->SetStatusText(_("Ready"));
		::wxEndBusyCursor();
	}
}

void wxNLMorphologyConverterFrm::MenuCloseNeuronClick(wxCommandEvent& event)
{
	CloseNeuron();
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


/*
* MnuviewNeuronComponentsViewCheck
*/
void wxNLMorphologyConverterFrm::MnuviewNeuronViewPointsSelect(wxCommandEvent& event)
{

	mNeuronGLCanvas->SetNeuronViewMode(kNeuronModePoints);
	
}

/*
* MnuviewNeuronComponentsViewCheck
*/
void wxNLMorphologyConverterFrm::MnuviewNeuronViewLinesSelect(wxCommandEvent& event)
{
	mNeuronGLCanvas->SetNeuronViewMode(kNeuronModeLine);
}

void wxNLMorphologyConverterFrm::InitialiseApplicationLogic()
{
	wxStandardPaths stdPaths;
	wxString filePath = stdPaths.GetExecutablePath();
	String pathDir = String(filePath.mbc_str()).beforeLast("bin");
	//root path
	g_applicationLogic.Initialise(pathDir);
}


NeuronGLCanvas* wxNLMorphologyConverterFrm::GetOrCreateNeuronGLCanvas(wxString name)
{
	return mNeuronGLCanvas;
}


/************************************************************************/
/* Reset current neuron data
/* Clear neuron panels, reset neuron associated logic/display data
/************************************************************************/
void wxNLMorphologyConverterFrm::CloseNeuron ()
{
	mNeuronGLCanvas->Clear();
	mNeuronComponentsDataPanel->ClearNeuronData();
	GetNeuronElementsDisplaySettings().clear();
	g_applicationLogic.GetMorphologyDataManager().Remove(mNeuronId);
	mNeuronId =-1;
	UpdateTitle();
	UpdateMenus () ;
	WxStatusBarMain->SetStatusText(_("Ready"));
	WxStatusBarMain->SetStatusText(wxEmptyString,1);
	WxStatusBarMain->SetStatusText(wxEmptyString,2);
}




void wxNLMorphologyConverterFrm::UpdateTitle () 
{
	wxString title = GetTitleVersion();
	if (title != GetTitle()) 
		SetTitle (title);
}

void  wxNLMorphologyConverterFrm::UpdateMenus () 
{
	//Enable/disable menu options according to the state.
	//If there is no neuron loaded, disable export /save  options
	WxMenuBar1->Enable(ID_MNU_EXPORT, NeuronLoaded());
	WxMenuBar1->Enable(ID_MNU_CLOSENEURON, NeuronLoaded());

}

extern wxString g_appDirectory;
void wxNLMorphologyConverterFrm::OnHelp (wxCommandEvent &WXUNUSED(event))
{
	wxWindow *active = wxGetActiveWindow();
	wxString helptext;
	while (active && helptext.IsEmpty()) {
		helptext = active->GetHelpText();
		active = GetParent();
	}
	g_help->DisplayContents();
	/*
	wxBoxSizer *topsizer;
	wxHtmlWindow *html;
	wxDialog dlg(this, wxID_ANY, _("Help Contents"),wxDefaultPosition);
	topsizer = new wxBoxSizer(wxVERTICAL);
	html = new wxHtmlWindow(&dlg, wxID_ANY, wxDefaultPosition,wxSize(APP_MIN_WIDTH,APP_MIN_HEIGHT), wxHW_SCROLLBAR_AUTO);
	html->SetBorders(1);
	wxString helpfile =  g_appDirectory +"/wxNlMorphologyViewer/docs/NlMorphologyViewer.html";
	html->LoadPage(helpfile);
	// Fit the HTML window to the size of its contents
	//html->SetSize(html->GetInternalRepresentation()->GetWidth(),html->GetInternalRepresentation()->GetHeight());
	topsizer->Add(html, 1, wxALL, 10);
	topsizer->Add(new wxStaticLine(&dlg, wxID_ANY), 0, wxEXPAND | wxLEFT |
			wxRIGHT, 10);
	wxButton *but = new wxButton(&dlg, wxID_OK, _("OK"));
	but->SetDefault();
	topsizer->Add(but, 0, wxALL | wxALIGN_RIGHT, 15);
	
	dlg.SetSizer(topsizer);
	topsizer->Fit(&dlg);
	dlg.ShowModal();	
	*/
}


//WxMenuBar1->Enable(ID_MNU_VIEWLAYOUT3D, false);	