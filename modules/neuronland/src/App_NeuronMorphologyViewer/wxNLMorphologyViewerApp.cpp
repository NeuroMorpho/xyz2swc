//---------------------------------------------------------------------------
//
// Name:        wxNLMorphologyConverterApp.cpp
// Author:      Vanessa Ibanez
// Created:     19/01/2009 14:53:24
// Description: 
//
//---------------------------------------------------------------------------

#include "wxNLMorphologyViewerApp.h"
#include "wxNLMorphologyViewerForm.h"

#include "nlApplicationSettings.h"
#include "wx/stdpaths.h"
#include <wx/filename.h>
#include <wx/filesys.h>
//help
#include <wx/fs_zip.h>   // ZIP filesystem support
#include <wx/html/helpctrl.h> // html help support
//#include "wx/localize.h"   // localize support
//end help includes

IMPLEMENT_APP(wxNLMorphologyConverterFrmApp)

wxString g_appname;
wxString g_appDirectory;
//! global help provider
wxHtmlHelpController *g_help = NULL;
/************************************************************************/
/*                                                                      */
/************************************************************************/
void wxNLMorphologyConverterFrmApp::InitHelp()
{	

	wxFileSystem::AddHandler (new wxZipFSHandler);
	g_help = new wxHtmlHelpController;
	//wxString helpfile =  g_appDirectory + "/" +g_appname + _T(".htb");
	wxString helpfile =  g_appDirectory + wxFileName::GetPathSeparator() + "HelpNlMorphologyViewer.htb";
	if (wxFileExists (helpfile)) 
	{
		g_help->AddBook (helpfile);
	}
}



bool wxNLMorphologyConverterFrmApp::OnInit()
{
	if (g_appDirectory.IsEmpty()) 
		g_appDirectory = wxFileName(argv[0]).GetPath (wxPATH_GET_VOLUME);
	 g_appname = "NLMorphologyViewer";//wxFileName(argv[0]).GetName();

#ifdef __WXMAC__
	appPath += wxFileName::GetPathSeparator() + appName + _T(".app/Contents/SharedSupport"));
#endif
	SetAppName (APP_NAME);
	SetVendorName (APP_VENDOR);
//	g_appname.Append (APP_NAME);

    InitLanguageSupport();
	InitHelp();

    //Create main window
    wxNLMorphologyConverterFrm* frame = new wxNLMorphologyConverterFrm(NULL);
    SetTopWindow(frame);
    frame->Show();
    return true;
}
 
int wxNLMorphologyConverterFrmApp::OnExit()
{
	if (g_help) delete g_help;

	return 0;
}


 
void wxNLMorphologyConverterFrmApp::InitLanguageSupport()
{
   /* language =  wxLANGUAGE_DEFAULT;
 
    // fake functions, use proper implementation
    if( userWantsAnotherLanguageThanDefault() )
        language = getUsersFavoriteLanguage();
       
    // load language if possible, fall back to english otherwise
    if(wxLocale::IsAvailable(language))
    {
	m_locale = new wxLocale( language, wxLOCALE_CONV_ENCODING );
 
        #ifdef __WXGTK__
        // add locale search paths
        m_locale->AddCatalogLookupPathPrefix(wxT("/usr"));
        m_locale->AddCatalogLookupPathPrefix(wxT("/usr/local"));
        wxStandardPaths* paths = (wxStandardPaths*) &wxStandardPaths::Get();
        wxString prefix = paths->GetInstallPrefix();
        m_locale->AddCatalogLookupPathPrefix( prefix );
        #endif
 
        m_locale->AddCatalog(wxT("app_name"));
 
	    if(! m_locale->IsOk() )
	    {
		    std::cerr << "selected language is wrong" << std::endl;
		    delete m_locale;
		    m_locale = new wxLocale( wxLANGUAGE_ENGLISH );
                    language = wxLANGUAGE_ENGLISH;
	    }
    }
    else
    {
        std::cout << "The selected language is not supported by your system."
                  << "Try installing support for this language." << std::endl;
    	m_locale = new wxLocale( wxLANGUAGE_ENGLISH );
        m_locale = wxLANGUAGE_ENGLISH;
    }
    
    */
    /**
    wxString appPath = g_appDirectory; // gets the configured application directory
    if (appPath.IsEmpty()) appPath = wxFileName(argv[0]).GetPath (wxPATH_GET_VOLUME);
    wxString appName = wxFileName(argv[0]).GetName();
#ifdef __WXMAC__
    appPath += wxFileName::GetPathSeparator() + appName + _T(".app/Contents/SharedSupport"));
#endif

    ...
    m_locale.Init();
    m_locale.AddCatalogLookupPathPrefix (appPath);
    m_locale.AddCatalog (appName);
    */
}
