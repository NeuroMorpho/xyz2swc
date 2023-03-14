//---------------------------------------------------------------------------
//
// Name:        wxNLMorphologyConverterApp.h
// Author:      Satsumatwig
// Created:     19/01/2009 14:53:24
// Description: 
//
//---------------------------------------------------------------------------

#ifndef __WXNLMORPHOLOGYCONVERTERFRMApp_h__
#define __WXNLMORPHOLOGYCONVERTERFRMApp_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#else
	#include <wx/wxprec.h>
#endif

class wxNLMorphologyConverterFrmApp : public wxApp
{
	public:
		bool OnInit();
		int OnExit();


	private:
            void InitLanguageSupport();
			void InitHelp();
    		wxLocale m_locale;
};

#endif
