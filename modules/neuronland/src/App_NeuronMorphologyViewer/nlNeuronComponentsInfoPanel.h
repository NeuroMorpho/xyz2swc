//---------------------------------------------------------------------------
//
// Name:        NeuronTxtDataFrm.h
// Author:      VanessaNessie
// Created:     23/01/2009 11:15:29
// Description: NeuronTxtDataFrm class declaration
//
//---------------------------------------------------------------------------

#ifndef __NEURONCOMPONENTSINFOPANEL_h__
#define __NEURONCOMPONENTSINFOPANEL_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/panel.h>
#else
	#include <wx/wxprec.h>
#endif

#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/notebook.h>
//NL INCLUDES
//#include "NeuronRepresentation/NeuronComponentDisplayProperties.h"
//#include "nlNeuronComponentDisplayProperties.h"
#include "NeuronMorphologyDataManager/MorphologyDataManager.h"

struct NeuronComponentDisplayProperties;

class NlNeuronComponentsInfoPanel : public wxPanel
{

	public:
		NlNeuronComponentsInfoPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
        const wxPoint& pos = wxDefaultPosition, 
        const wxSize& size = wxDefaultSize, 
        long style = wxTAB_TRAVERSAL,
        const wxString& name = wxT("NlNeuronComponentsInfoPanel"));

		virtual ~NlNeuronComponentsInfoPanel();

    	void WxButtonShowCompClick(wxCommandEvent& event);
		//void WxButtonShowMarkersClick(wxCommandEvent& event);
		void WxButtonCompColourClick(wxCommandEvent& event);
	
	/************************************************************************/
	/* Load Neuron Pane Data     
	/* Initializes panel data from neuron information.
	/************************************************************************/
	void LoadNeuronData (MorphologyDataManager::NeuronID neuronId);

	/************************************************************************/
	/* Clears/resets all data
	/************************************************************************/
	void ClearNeuronData ();

	private:
	
		wxTextCtrl *          WxMemoComponentWarnings; //Component statistics text control
		wxStaticBoxSizer *    WxStaticBoxSizerComponentStats;
		//Button controls
		wxButton *            WxButtonCompColour;
		//wxButton *            WxButtonShowMarkers;
		wxButton *            WxButtonShowComp;
		
		//Sizers (maybe can be moved
        wxBoxSizer *WxBoxSizerCompButtons;		
		//wxListView *WxListViewNeuronComponents;
		wxListCtrl *WxListViewNeuronComponents;
		wxBoxSizer *WxBoxSizer2;
		wxStaticBoxSizer *WxStaticBoxSizerComponetsProp;
		wxBoxSizer *WxBoxSizerComponentsList;
		wxBoxSizer *WxBoxSizerMain;
		
public:
		enum StatisticsPanel
		{
			kPanelGlobal,
			kPanelEmpty,
			kPanelSoma,
			kPanelTree,
			kPanelMarkers,
			kPanelSpines,

			kNumPanels,
		};

		enum GlobalStatistic
		{
			kGNumSomas,
			kGNumDendrites,
			kGNumAxons,
			kGTotalNumSamples,
			kGTotalNumSpines,
			kGTotalNumMarkers,
			kGTreeBranches,
			kGTreeBranchPoints,
			kGTreeTerminals,
			kGTreeMaxBranchingOrder,
			kGTreeMaxPathLength,
			kGTreeMaxPhysLength,
			kGTreeMinDiameter,
			kGTreeMaxDiameter,
			kGTreeBranchLength,
			kGTreeSurfaceArea,
			kGTreeVolume,

			kNumGlobalStatistic
		};

		enum TreeStatistic
		{
			kTNumSamples,
			kTNumSpines,
			kTNumMarkers,
			kTNumBranches,
			kTNumBranchPoints,
			kTNumTreeTerminals,
			kTMaxBranchingOrder,
			kTMaxPathLength,
			kTMaxPhysLength,
			kTMinDiameter,
			kTMaxDiameter,
			kTBranchLength,
			kTSurfaceArea,
			kTVolume,

			kNumTreeStatistic
		};

		enum SomaStatistic
		{
			kSNumSamples,
			kSNumSpines,
			kSNumMarkers,
			kSMinDiameter,
			kSMaxDiameter,
			kSTotalLength,

			kNumSomaStatistic
		};

		enum SpinesStatistic 
		{
			kTotalSpines,
			kGlobalSpines,
			kTreeSpines,

			kNumSpinesStatistic
		};

		enum MarkerStatistic 
		{
			kTotalMarkers,
			kGlobalMarkers,
			kSomaMarkers,
			kTreeMarkers,
			//kAxonMarkers,
			kNumMarkerTypes,

			kNumMarkersStatistic
		};

		wxStaticText *globalStatTxt[kNumGlobalStatistic];

		wxStaticText *componentLabelTxt[kNumTreeStatistic];
		union
		{
			wxStaticText *somaStatTxt[kNumSomaStatistic];
			wxStaticText *treeStatTxt[kNumTreeStatistic];
			wxStaticText *spinesStatTxt[kNumSpinesStatistic];
			wxStaticText *markersStatTxt[kNumMarkersStatistic];
		};
		//

		wxGridSizer *wxGridSizerStats;
		wxNotebook* mDataNoteBook;

	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
		
			ID_WXBUTTONCOMPCOLOUR = 1034,
			//ID_WXBUTTONSHOWMARKERS = 1033,
			ID_WXBUTTONSHOWCOMP = 1032,
			ID_WXLISTBOXNEURONCOMPONENTS = 1031,
			
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};

	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
		
		/***************************
		* Initializes the list data
		*/
		void LoadNeuronItemListData();
		

       /******Events***************************************************************/
        /**
        */
        void WxNeuronComponentListRightClick(wxListEvent& event);
      
        /**
        * Updates the state of the buttons.
        * If there is 1 or more components selected, all the buttons are available.
        * If only 1 item is selected,update the buttons according to the item state:
            Item "enabled"/disabled => button text change to "disable"/"enable" 
            Item has markers => 
        */
        void WxListNeuronComponentsItemSelected(wxListEvent& event);
        /*      
        * Updates the state of the buttons.
        * If no components are selected, Disable all the buttons.
        */
        void WxListNeuronComponentsItemDeSelected(wxListEvent& event);

		void WxListNeuronComponentsItemFocused(wxListEvent& event);

        
        /**
            Register button events.
        */
        void RegisterEvents();
        
		/******************************************************************************/
        
        void UpdateListElement (long itemIndex, NeuronComponentDisplayProperties& neuronItemData);
        
		/************************************************************************/
		/* Obtain Neuron statistics  data and feed statistics pane	  
		/* @param tree Index of the tree to display statistics, if -1 ,display neuron stats*/
		/************************************************************************/
		void SetComponentStatisticsPaneValues (int aTreeIndex);
		void SetGlobalStatisticsPaneValues ();

		/************************************************************************/
		/* Obtain Neuron WARNINGS  data and feed WARNINGS pane	  
		/* @param tree Index of the tree to display statistics, if -1 ,display neuron stats*/
		/************************************************************************/
		void SetFaultsPaneValues (int aTreeIndex);

		bool SetNeuronElementsDisplayData(MorphologyDataManager::NeuronID id );

        /**
        *Empty statistics data
        */
        void ClearStatisticsPanes();

		void CreateWarningsPane(wxNotebook* book);
		void CreateGlobalStatsPanel(wxNotebook* book);
		void CreateComponentStatsPanel(wxNotebook* book);
		void CreateStatsPanel(wxNotebook* book, wxPanel* pan, u32 numStats, const char** statNames, wxStaticText** pWxStaticTextArray, wxStaticText** pWxStoreLabels);
		void ConfigureComponentStatsPanel(u32 numStats, const char** statNames);

		void SetCurrent3rdPage(StatisticsPanel panel);

private:

		NeuronStatisticsBasic mNeuronStatistics;
		MorphologyDataFaults mNeuronFaults;
		MorphologyDataManager::NeuronID mNeuronId ;

		unsigned int mNumSomas; //required to calculate the index of the statistics data.
        // Member Data
//        NLNeuronDisplayElements      mNeuronElements; //fake data types...
};

#endif
