#ifndef NLAPPSETTINGS_H_
#define NLAPPSETTINGS_H_

#include <wx/string.h>
#include <wx/colour.h>

const unsigned int APP_MIN_WIDTH = 800;
const unsigned int APP_MIN_HEIGHT = 600;
const wxString APP_NAME = _T("NLMorphologyViewer");
const wxString APP_VENDOR = _T("NeuronLand");
const wxString APP_VERSION = _T("0.4.0");
const wxString APP_MAINT = _T("James Ogden");
const wxString APP_MAIN2 = _T("Vanessa Ibanez Llanos");
const wxString APP_COPYRIGTH = _T("(C) 2009-2012 Neuronland");
const wxString APP_WEBSITE = _T("http://www.neuronland.org/");
//const wxString APP_WEBSITE_EXACT = _T("index.php?page=demo.html");
const wxString APP_MAIL = _T("support@neuronland.org");

const wxColour kNlNeuronComponentDefaultEnabledColour (0,0,255);
const wxColour kNlNeuronComponentDisabledTxtColour (127,127,127);
const wxColour kNlNeuronComponentDisabledBGColour (0,0,0);

#endif 
