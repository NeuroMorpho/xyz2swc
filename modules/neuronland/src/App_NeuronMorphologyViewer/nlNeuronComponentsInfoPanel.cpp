//---------------------------------------------------------------------------
//
// Name:        NlNeuronComponentsInfoPanel.cpp
// Author:      VanessaNessie
// Created:     23/01/2009 11:15:29
// Description: NlNeuronComponentsInfoPanel class implementation
//
//---------------------------------------------------------------------------

#include "nlNeuronComponentsInfoPanel.h"
#include "nlApplicationSettings.h"
#include <wx/colordlg.h>
#include <wx/notebook.h>

#include "NeuronApplicationLogic/ApplicationLogic.h"
#include "NeuronRepresentation\NeuronComponentDisplayProperties.h"

//----------------------------------------------------------------------------
// NlNeuronComponentsInfoPanel
//----------------------------------------------------------------------------
NlNeuronComponentsInfoPanel::NlNeuronComponentsInfoPanel(wxWindow* parent, 
wxWindowID id /*= 1*/,
        const wxPoint& pos /*= wxDefaultPosition*/,
        const wxSize& size /*= wxDefaultSize*/,
        long style/* = wxTAB_TRAVERSAL*/,
        const wxString& name /*= "NlNeuronComponentsInfoPanel"*/):
        wxPanel(parent, id, pos, size,style,name ),
		mNumSomas(0)
{
	CreateGUIControls();
}

NlNeuronComponentsInfoPanel::~NlNeuronComponentsInfoPanel()
{
}



void NlNeuronComponentsInfoPanel::CreateGUIControls()
{

	WxBoxSizerMain = new wxBoxSizer(wxVERTICAL);

	//	WxBoxSizerComponentsList = new wxBoxSizer(wxHORIZONTAL);
	//	WxBoxSizer1->Add(WxBoxSizerComponentsList, 4, wxALIGN_TOP | wxALIGN_CENTER | wxEXPAND | wxALL, 5);

	wxStaticBox* WxStaticBoxSizerComponetsProp_StaticBoxObj = new wxStaticBox(this, wxID_ANY, _("Neuron Components"));
	WxStaticBoxSizerComponetsProp = new wxStaticBoxSizer(WxStaticBoxSizerComponetsProp_StaticBoxObj, wxHORIZONTAL);
	//	WxBoxSizerComponentsList->Add(WxStaticBoxSizerComponetsProp, 1, wxALIGN_CENTER | wxEXPAND | wxALL, 5);
	WxBoxSizerMain->Add(WxStaticBoxSizerComponetsProp, 4, wxALIGN_TOP | wxALIGN_CENTER | wxEXPAND | wxALL, 2);

	WxBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	WxStaticBoxSizerComponetsProp->Add(WxBoxSizer2, 1, wxALIGN_CENTER | wxEXPAND | wxALL, 2);

	wxArrayString arrayStringFor_WxListBoxNeuronComponents;
	//	WxListViewNeuronComponents = new wxListView(this, ID_WXLISTBOXNEURONCOMPONENTS, wxDefaultPosition, wxSize(200, 200), wxLC_LIST);
	WxListViewNeuronComponents = new wxListCtrl(this, ID_WXLISTBOXNEURONCOMPONENTS, 
		wxDefaultPosition,wxDefaultSize/* wxSize(200, 200)*/, wxLC_REPORT);
	WxListViewNeuronComponents->SetBackgroundColour(wxColour(0,0,0));
	//WxListViewNeuronComponents->SetFont(wxFont(10, wxSWISS, wxNORMAL,wxNORMAL, false));
	WxListViewNeuronComponents->InsertColumn(0, _("Type"));
	WxListViewNeuronComponents->SetColumnWidth(0,wxLIST_AUTOSIZE_USEHEADER );
	WxListViewNeuronComponents->InsertColumn(1, _("Visible") );
	WxListViewNeuronComponents->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER );
	//WxListViewNeuronComponents->InsertColumn(2, _("Markers") );
	//WxListViewNeuronComponents->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER );

	WxBoxSizer2->Add(WxListViewNeuronComponents,7, wxALIGN_TOP | wxEXPAND | wxALL,2);

	WxBoxSizerCompButtons = new wxBoxSizer(wxHORIZONTAL);
	WxBoxSizer2->Add(WxBoxSizerCompButtons, 0, wxALIGN_BOTTOM | wxEXPAND |wxALL, 2);

	WxButtonShowComp = new wxButton(this, ID_WXBUTTONSHOWCOMP, _("ON/OFF"), wxDefaultPosition, wxDefaultSize /*wxSize(75,25)*/, 0, wxDefaultValidator, wxT("WxButtonShowComp"));
	//WxButtonShowComp->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxNORMAL));
	WxBoxSizerCompButtons->Add(WxButtonShowComp,0,wxALIGN_CENTER | wxEXPAND | wxALL,2);
	WxButtonShowComp->Enable(false);

	//WxButtonShowMarkers = new wxButton(this, ID_WXBUTTONSHOWMARKERS, _("Markers"), wxDefaultPosition, wxDefaultSize /*wxSize(75,25)*/, 0, wxDefaultValidator, wxT("WxButtonShowMarkers"));
	//WxButtonShowMarkers->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Segoe UI")));
	//WxBoxSizerCompButtons->Add(WxButtonShowMarkers,0,wxALIGN_CENTER | wxALL,2);
	//WxButtonShowMarkers->Enable(false);

	WxButtonCompColour = new wxButton(this, ID_WXBUTTONCOMPCOLOUR, _("Colour"), wxDefaultPosition, wxDefaultSize /*wxSize(75,25)*/, 0, wxDefaultValidator, wxT("WxButtonCompColour"));
	//WxButtonCompColour->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Segoe UI")));
	WxBoxSizerCompButtons->Add(WxButtonCompColour,0, wxALIGN_CENTER | wxALL,2);
	WxButtonCompColour->Enable(false);


	mDataNoteBook = new wxNotebook(this, -1,  wxPoint(-1, -1), wxSize(220, 302));
	WxBoxSizerMain->Add(mDataNoteBook, 0, wxALIGN_BOTTOM | wxALIGN_CENTER | wxEXPAND | wxALL, 5);

	// permanent
	CreateWarningsPane(mDataNoteBook);
	CreateGlobalStatsPanel(mDataNoteBook);
	CreateComponentStatsPanel(mDataNoteBook);

	// variable - start empty
	SetCurrent3rdPage(kPanelEmpty);

	//Register button events.
	RegisterEvents();  
	


	this->SetSizer(WxBoxSizerMain);
	GetSizer()->Layout();
	//GetSizer()->Fit(this);
	Center();	

}

void NlNeuronComponentsInfoPanel::CreateWarningsPane(wxNotebook* book)
{
	WxMemoComponentWarnings = new wxTextCtrl(book, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, wxT("WxMemoComponentWarnings"));
	WxMemoComponentWarnings->SetInsertionPointEnd();
	WxMemoComponentWarnings->SetFont(wxFont(9, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Segoe UI")));
	WxMemoComponentWarnings->SetEditable(false);
	//WxStaticBoxSizerComponentStats->Add(WxMemoComponentWarnings,1,wxALIGN_CENTER | wxEXPAND | wxALL,5);
	book->AddPage(WxMemoComponentWarnings, _("Warnings"));
}


void NlNeuronComponentsInfoPanel::RegisterEvents()
{
   Connect(ID_WXBUTTONSHOWCOMP, wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(NlNeuronComponentsInfoPanel::WxButtonShowCompClick));
    //Connect(ID_WXBUTTONSHOWMARKERS, wxEVT_COMMAND_BUTTON_CLICKED,
      //wxCommandEventHandler(NlNeuronComponentsInfoPanel::WxButtonShowMarkersClick));
    Connect(ID_WXBUTTONCOMPCOLOUR, wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(NlNeuronComponentsInfoPanel::WxButtonCompColourClick));
    Connect(ID_WXLISTBOXNEURONCOMPONENTS, wxEVT_COMMAND_LIST_ITEM_FOCUSED, 
    wxListEventHandler(NlNeuronComponentsInfoPanel::WxListNeuronComponentsItemFocused));
    Connect(ID_WXLISTBOXNEURONCOMPONENTS, wxEVT_COMMAND_LIST_ITEM_SELECTED,
    wxListEventHandler(NlNeuronComponentsInfoPanel::WxListNeuronComponentsItemSelected));
    Connect(ID_WXLISTBOXNEURONCOMPONENTS, wxEVT_COMMAND_LIST_ITEM_DESELECTED,
    wxListEventHandler(NlNeuronComponentsInfoPanel::WxListNeuronComponentsItemDeSelected));


}
    
void NlNeuronComponentsInfoPanel::OnClose(wxCloseEvent& event)
{
	Destroy();
}

/*
 * WxButtonCompColourClick
 */
void NlNeuronComponentsInfoPanel::WxButtonCompColourClick(wxCommandEvent& event)
{
    //only available if there are neuron components selected

	wxColour  selectedCol = ::wxGetColourFromUser(this);
	if (selectedCol.IsOk())
	{
		for (long i = WxListViewNeuronComponents->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		i != -1; i=WxListViewNeuronComponents->GetNextItem(i, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)  )
		{
			if (GetNeuronElementsDisplaySettings()[i].mIsVisible)
			{			
				//mNeuronElements[i].mColour = selectedCol;
				GetNeuronElementsDisplaySettings()[i].mRed = selectedCol.Red();
				GetNeuronElementsDisplaySettings()[i].mGreen = selectedCol.Green();
				GetNeuronElementsDisplaySettings()[i].mBlue = selectedCol.Blue();

				UpdateListElement(i,GetNeuronElementsDisplaySettings()[i]);
			}
		}
	}
	/*
    wxColourDialog colourDlg(this);
    if (colourDlg.ShowModal()== wxID_OK)
    {              
        wxColourData  colData =colourDlg.GetColourData ();
        wxColour  selectedCol= colData.GetColour();        
        //Set the selected items colour value to the chose one.
        for (long i = WxListViewNeuronComponents->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
         i != -1; i=WxListViewNeuronComponents->GetNextItem(i, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)  )
         {
            //mNeuronElements[i].mColour = selectedCol;
			 GetNeuronElementsDisplaySettings()[i].mRed = selectedCol.Red();
			 GetNeuronElementsDisplaySettings()[i].mGreen = selectedCol.Green();
			 GetNeuronElementsDisplaySettings()[i].mBlue = selectedCol.Blue();

            UpdateListElement(i,GetNeuronElementsDisplaySettings()[i]);
         }
    }*/
}

/*
 * WxButtonShowCompClick
 */
void NlNeuronComponentsInfoPanel::WxButtonShowCompClick(wxCommandEvent& event)
{ 
    for (long i = WxListViewNeuronComponents->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
         i != -1; i=WxListViewNeuronComponents->GetNextItem(i, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)  )
    {        
		//update the ComponentElement
        GetNeuronElementsDisplaySettings()[i].mIsVisible = !GetNeuronElementsDisplaySettings()[i].mIsVisible;
		GetNeuronElementsDisplaySettings()[i].mAlpha=GetNeuronElementsDisplaySettings()[i].mIsVisible?0xff:0x00;
		UpdateListElement(i, GetNeuronElementsDisplaySettings()[i]);
    }    
}

void NlNeuronComponentsInfoPanel::WxListNeuronComponentsItemFocused(wxListEvent& event)
{
   // WxMemoComponentWarnings->Clear();
    long focusedItem = WxListViewNeuronComponents->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
   // if(focusedItem> -1)   	
	SetComponentStatisticsPaneValues(focusedItem);		
	mDataNoteBook->/*GetPage(2)->*/Refresh();
}

void NlNeuronComponentsInfoPanel::WxListNeuronComponentsItemSelected(wxListEvent& event)
{
    long selected = WxListViewNeuronComponents->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(selected> -1)
    {
        WxButtonShowComp->Enable(true);
        WxButtonCompColour->Enable(true);
       // WxButtonShowMarkers->Enable(true);
	#ifdef __WXMAC__
		if (selected <= mNumSomas)
		{  
			focusedItem = -1;       
		}  
		SetComponentStatisticsPaneValues(focusedItem);		
		mDataNoteBook->/*GetPage(2)->*/Refresh();
	#endif //ifdef __WXMAC__

    }
}

        /*
        * Updates the state of the buttons.
        * If no components are selected, Disable all the buttons.
        */
void NlNeuronComponentsInfoPanel::WxListNeuronComponentsItemDeSelected(wxListEvent& event)
{
    long selected = WxListViewNeuronComponents->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(selected== -1)
    {
        //there are no items selected, disable buttons
        WxButtonShowComp->Enable(false);
        WxButtonCompColour->Enable(false);
        //WxButtonShowMarkers->Enable(false);
        
        //Clear Stats Data Or show neuron data
        SetComponentStatisticsPaneValues(-1);
    }
}

void NlNeuronComponentsInfoPanel::LoadNeuronData (MorphologyDataManager::NeuronID neuronId)
{
	
	mNeuronId=neuronId;
	// Loads Neuron Data into list
	LoadNeuronItemListData();
	//load stats and faults into data members and feed the panes
	g_applicationLogic.GetStatisticsAndFaults(mNeuronId,mNeuronStatistics, mNeuronFaults);
	SetComponentStatisticsPaneValues(-1); //load the neuron general stats
	SetGlobalStatisticsPaneValues();
	SetFaultsPaneValues(-1);
}


void NlNeuronComponentsInfoPanel::SetComponentStatisticsPaneValues (int aTreeIndex)
{
	const u32 numSomas = mNeuronStatistics.numSomas;
	const u32 numTrees = mNeuronStatistics.numDendrites + mNeuronStatistics.numAxons;
	const u32 numMarkers = (mNeuronStatistics.numGlobalMarkers + mNeuronStatistics.combinedTreeStats.numMarkers + mNeuronStatistics.combinedSomaStats.numMarkers > 0) ? 1 : 0;
	const u32 numSpines = (mNeuronStatistics.numGlobalSpines + mNeuronStatistics.combinedTreeStats.numSpines + mNeuronStatistics.combinedSomaStats.numSpines > 0) ? 1 : 0;


	if(aTreeIndex == -1)
	{
		SetCurrent3rdPage(kPanelEmpty);
	}
	else if(aTreeIndex < numSomas)
	{
		SetCurrent3rdPage(kPanelSoma);

		const SomaStatisticsBasic& statsSoma = mNeuronStatistics.perSomaStats[aTreeIndex];

		somaStatTxt[kSNumSamples]->SetLabel(wxString::Format(wxT("%d"),	statsSoma.numSamples));
		somaStatTxt[kSNumSpines]->SetLabel(wxString::Format(wxT("%d"),	statsSoma.numSpines));
		somaStatTxt[kSNumMarkers]->SetLabel(wxString::Format(wxT("%d"),	statsSoma.numMarkers));
		somaStatTxt[kSMaxDiameter]->SetLabel(wxString::Format(wxT("%f"),	statsSoma.maxDiameter));
		somaStatTxt[kSMinDiameter]->SetLabel(wxString::Format(wxT("%f"),	statsSoma.minDiameter));
		somaStatTxt[kSTotalLength]->SetLabel(wxString::Format(wxT("%f"),	statsSoma.totalLength));

	}
	else if(aTreeIndex < numSomas + numTrees)
	{
		SetCurrent3rdPage(kPanelTree);

		const TreeStatisticsBasic& statsTree = mNeuronStatistics.perTreeStats[aTreeIndex-numSomas];
				
		treeStatTxt[kTNumSamples]->SetLabel(wxString::Format(wxT("%d"),	statsTree.numSamples));
		treeStatTxt[kTNumSpines]->SetLabel(wxString::Format(wxT("%d"),	statsTree.numSpines));
		treeStatTxt[kTNumMarkers]->SetLabel(wxString::Format(wxT("%d"),	statsTree.numMarkers));
		treeStatTxt[kTNumBranches]->SetLabel(wxString::Format(wxT("%d"),		statsTree.numBranches));
		treeStatTxt[kTNumBranchPoints]->SetLabel(wxString::Format(wxT("%d"),	statsTree.numBranchPoints));
		treeStatTxt[kTNumTreeTerminals]->SetLabel(wxString::Format(wxT("%d"),	statsTree.numTerminals));
		treeStatTxt[kTMaxBranchingOrder]->SetLabel(wxString::Format(wxT("%d"),statsTree.branchingOrder));
		treeStatTxt[kTMaxPathLength]->SetLabel(wxString::Format(wxT("%f"),statsTree.maxTerminalDistancePath));
		treeStatTxt[kTMaxPhysLength]->SetLabel(wxString::Format(wxT("%f"),statsTree.maxTerminalDistancePhys));
		treeStatTxt[kTMaxDiameter]->SetLabel(wxString::Format(wxT("%f"),	statsTree.maxDiameter));
		treeStatTxt[kTMinDiameter]->SetLabel(wxString::Format(wxT("%f"),	statsTree.minDiameter));
		treeStatTxt[kTBranchLength]->SetLabel(wxString::Format(wxT("%f"),	statsTree.totalLength));
		treeStatTxt[kTSurfaceArea]->SetLabel(wxString::Format(wxT("%f"),	statsTree.totalSurface));
		treeStatTxt[kTVolume]->SetLabel(wxString::Format(wxT("%f"),		statsTree.totalVolume));

	}
	else if(aTreeIndex < numSomas + numTrees + numMarkers)
	{
		SetCurrent3rdPage(kPanelMarkers);

		const u32 numGlobal = mNeuronStatistics.numGlobalMarkers;
		const u32 numTree = mNeuronStatistics.combinedTreeStats.numMarkers;
		const u32 numSoma = mNeuronStatistics.combinedSomaStats.numMarkers;
		const u32 numAxon = mNeuronStatistics.combinedDendStats.numMarkers;
		const u32 numDend = mNeuronStatistics.combinedAxonStats.numMarkers;
		const u32 numTypes = 0;//mNeuronStatistics.numMarkerTypes;

		markersStatTxt[kTotalMarkers]->SetLabel(wxString::Format(wxT("%d"),	numGlobal + numTree + numSoma));
		markersStatTxt[kGlobalMarkers]->SetLabel(wxString::Format(wxT("%d"), numGlobal));
		markersStatTxt[kSomaMarkers]->SetLabel(wxString::Format(wxT("%d"),	numSoma));
		markersStatTxt[kTreeMarkers]->SetLabel(wxString::Format(wxT("%d"),	numTree));
		//markersStatTxt[kDendMarkers]->SetLabel(wxString::Format(wxT("%d"),	numDend));
		//markersStatTxt[kAxonMarkers]->SetLabel(wxString::Format(wxT("%d"),	numAxon));
		markersStatTxt[kNumMarkerTypes]->SetLabel(wxString::Format(wxT("%d"), numTypes));
	}
	else if(aTreeIndex < numSomas + numTrees + numMarkers + numSpines)
	{
		SetCurrent3rdPage(kPanelSpines);

		const u32 numGlobalSp = mNeuronStatistics.numGlobalSpines;
		const u32 numTreeSp = mNeuronStatistics.combinedTreeStats.numSpines;

		spinesStatTxt[kTotalSpines]->SetLabel(wxString::Format(wxT("%d"),	numGlobalSp + numTreeSp));
		spinesStatTxt[kGlobalSpines]->SetLabel(wxString::Format(wxT("%d"), numGlobalSp));
		spinesStatTxt[kTreeSpines]->SetLabel(wxString::Format(wxT("%d"),	numTreeSp));
	}
}

void NlNeuronComponentsInfoPanel::SetGlobalStatisticsPaneValues()
{
	const TreeStatisticsBasic& statsTree = mNeuronStatistics.combinedTreeStats;
	const SomaStatisticsBasic& statsSoma = mNeuronStatistics.combinedSomaStats;

	globalStatTxt[kGNumSomas]->SetLabel(wxString::Format(wxT("%d"),			mNeuronStatistics.numSomas));
	globalStatTxt[kGNumDendrites]->SetLabel(wxString::Format(wxT("%d"),		mNeuronStatistics.numDendrites));
	globalStatTxt[kGNumAxons]->SetLabel(wxString::Format(wxT("%d"),			mNeuronStatistics.numAxons));
	globalStatTxt[kGTotalNumSamples]->SetLabel(wxString::Format(wxT("%d"),	statsTree.numSamples + statsSoma.numSamples));
	globalStatTxt[kGTotalNumSpines]->SetLabel(wxString::Format(wxT("%d"),	statsTree.numSpines + statsSoma.numSpines + mNeuronStatistics.numGlobalSpines));
	globalStatTxt[kGTotalNumMarkers]->SetLabel(wxString::Format(wxT("%d"),	statsTree.numMarkers + statsSoma.numMarkers + mNeuronStatistics.numGlobalMarkers));
	globalStatTxt[kGTreeBranches]->SetLabel(wxString::Format(wxT("%d"),		statsTree.numBranches));
	globalStatTxt[kGTreeBranchPoints]->SetLabel(wxString::Format(wxT("%d"),	statsTree.numBranchPoints));
	globalStatTxt[kGTreeTerminals]->SetLabel(wxString::Format(wxT("%d"),	statsTree.numTerminals));
	globalStatTxt[kGTreeMaxBranchingOrder]->SetLabel(wxString::Format(wxT("%d"),statsTree.branchingOrder));
	globalStatTxt[kGTreeMaxPathLength]->SetLabel(wxString::Format(wxT("%f"),statsTree.maxTerminalDistancePath));
	globalStatTxt[kGTreeMaxPhysLength]->SetLabel(wxString::Format(wxT("%f"),statsTree.maxTerminalDistancePhys));
	globalStatTxt[kGTreeMaxDiameter]->SetLabel(wxString::Format(wxT("%f"),	statsTree.maxDiameter));
	globalStatTxt[kGTreeMinDiameter]->SetLabel(wxString::Format(wxT("%f"),	statsTree.minDiameter));
	globalStatTxt[kGTreeBranchLength]->SetLabel(wxString::Format(wxT("%f"),	statsTree.totalLength));
	globalStatTxt[kGTreeSurfaceArea]->SetLabel(wxString::Format(wxT("%f"),	statsTree.totalSurface));
	globalStatTxt[kGTreeVolume]->SetLabel(wxString::Format(wxT("%f"),		statsTree.totalVolume));
}


void NlNeuronComponentsInfoPanel::SetFaultsPaneValues (int aTreeIndex)
{
	//NeuronFaultData nWarnings;
	
	//g_applicationLogic.GetNeuronWarnings(mNeuronId,mNeuronFaults);	
	if (mNeuronFaults.m_faults.size()> 0)
	{	
		wxString warnigsTxt ; 	
		warnigsTxt<<"Original Neuron Warnings:\n";	
		for(u32 n=0;n<mNeuronFaults.m_faults.size();++n)
			warnigsTxt<< mNeuronFaults.FaultToStringVisual(mNeuronFaults.m_faults[n]) << "\n";
		WxMemoComponentWarnings->AppendText(warnigsTxt);
	}

}

void NlNeuronComponentsInfoPanel::ClearStatisticsPanes()
{
	for(u32 n=0;n<kNumGlobalStatistic;++n)
		globalStatTxt[n]->SetLabel(wxEmptyString);

	ConfigureComponentStatsPanel(0, 0);
}



void NlNeuronComponentsInfoPanel::LoadNeuronItemListData()
{
	NLNeuronDisplayElements& mNeuronElementsDisplaySettings = GetNeuronElementsDisplaySettings();
	int ind =0;
	mNumSomas =0;
	int mNumDendrites=0;
	int mNumAx=0;
	for (NLNeuronDisplayElements::iterator itr = mNeuronElementsDisplaySettings.begin() ; 
		 itr!=mNeuronElementsDisplaySettings.end(); ++itr)
	{
		//mNeuronElementsDisplaySettings[i]=elementdata;
		wxListItem NewItem;
        NewItem.SetMask(wxLIST_MASK_TEXT);
		NewItem.SetId(ind++);
		wxString auxname;
		switch (itr->mType)
		{
		case kNeuronCompSoma:
			auxname << _("Soma ") << ++mNumSomas;
			break;
		case kNeuronCompDendrite:
			auxname << _("Dendrite ") << ++mNumDendrites; ;
			break;
		case kNeuronCompAxon:
			auxname << _("Axon ") << ++mNumAx;
			break;
		case kNeuronCompMarkers:
			auxname << _("Markers");
			break;
		case kNeuronCompSpines:
			auxname << _("Spines");
			break;
		}	
		NewItem.SetText(auxname);   
		NewItem.SetTextColour(wxColour(itr->mRed,itr->mGreen,itr->mBlue));
		NewItem.SetBackgroundColour( wxColour(0,0,0) );//kNlNeuronComponentDisabledBGColour);
		//check this res because it should be the index in which i enter them.
		long res = WxListViewNeuronComponents->InsertItem(NewItem);      
		UpdateListElement (res, *itr);
	}

}

void NlNeuronComponentsInfoPanel::UpdateListElement (long itemIndex, NeuronComponentDisplayProperties& neuronItemData)
{
    //wxString auxname ("Item " ); 	auxname << itemIndex;
    //wxMessageBox(auxname,wxT("WxButtonCompColourClick."),     wxICON_INFORMATION);
	
    WxListViewNeuronComponents->SetItem(itemIndex, 1, neuronItemData.mIsVisible?_("Y"):_("N")); //want this for col. 2
    //WxListViewNeuronComponents->SetItem(itemIndex, 2, neuronItemData.mShowMarckers? _("Y"):_("N")); //col. 3
	// WxListViewNeuronComponents->SetItemTextColour(itemIndex,neuronItemData.mColour );
    WxListViewNeuronComponents->SetItemTextColour(itemIndex,neuronItemData.mIsVisible?
												  wxColour(neuronItemData.mRed,neuronItemData.mGreen,neuronItemData.mBlue):
												  kNlNeuronComponentDisabledTxtColour);
  //  WxListViewNeuronComponents->SetItemBackgroundColour(itemIndex,neuronItemData.mIsVisible? wxColour(255,255,255): kNlNeuronComponentDisabledBGColour);
	
}


void NlNeuronComponentsInfoPanel::ClearNeuronData ()
{
	mNeuronFaults.Clear();
	mNeuronStatistics.Clear();
	WxListViewNeuronComponents->DeleteAllItems();
	this->ClearStatisticsPanes();
	WxMemoComponentWarnings->Clear();
}



//////////////////////////////////////////////////////////////////////////////////
// Stats panels: 
//

const char* globalStatName[NlNeuronComponentsInfoPanel::kNumGlobalStatistic] = 
{
	"Num Somas",
	"Num Dendrites",
	"Num Axons",
	"Total Num Samples",
	"Total Num Spines",
	"Total Num Markers",
	"Tree Branches",
	"Tree Branch Points",
	"Tree Terminals",
	"Tree Max Branching Order",
	"Tree Max Path Length",
	"Tree Max Phys Length",
	"Tree Min Diameter",
	"Tree Max Diameter",
	"Tree Branch Length",
	"Tree Surface Area",
	"Tree Volume"
};

const char* treeStatName[NlNeuronComponentsInfoPanel::kNumTreeStatistic] = 
{
	"Num Samples",
	"Num Spines",
	"Num Markers",
	"Num Branches",
	"Num Branch Points",
	"Num Terminals",
	"Max Branching Order",
	"Max Path Length",
	"Max Phys Length",
	"Min Diameter",
	"Max Diameter",
	"Total Branch Length",
	"Surface Area",
	"Volume"
};

const char* somaStatName[NlNeuronComponentsInfoPanel::kNumSomaStatistic] = 
{
	"Num Samples",
	"Num Spines",
	"Num Markers",
	"Min Diameter",
	"Max Diameter",
	"Total Length"
};

const char* spinesStatName[NlNeuronComponentsInfoPanel::kNumSpinesStatistic] = 
{
	"Total Num Spines",
	"Num Global Spines",
	"Num Tree Spines"
};

const char* markersStatName[NlNeuronComponentsInfoPanel::kNumMarkersStatistic] = 
{
	"Total Num Markers",
	"Num Global Markers",
	"Num Soma Markers",
	"Num Tree Markers",
	//"Num Dendrite Markers",
	//"Num Axon Markers",
	"Num Marker Types"
};

const char* statsPanelName[NlNeuronComponentsInfoPanel::kNumPanels] = 
{
	"Global Stats.",
	"Component (Not Set)",
	"Component (Soma)",
	"Component (Tree)",
	"Component (Markers)",
	"Component (Spines)"
};

void NlNeuronComponentsInfoPanel::CreateGlobalStatsPanel(wxNotebook* book)
{
	wxPanel* pPanel = new wxPanel(book);
	CreateStatsPanel(book, pPanel, kNumGlobalStatistic, globalStatName, globalStatTxt, 0);
	book->AddPage(pPanel, statsPanelName[kPanelGlobal], false);
}

void NlNeuronComponentsInfoPanel::CreateComponentStatsPanel(wxNotebook* book)
{
	wxPanel* pPanel = new wxPanel(book);
	CreateStatsPanel(book, pPanel, kNumTreeStatistic, treeStatName, treeStatTxt, componentLabelTxt);
	book->AddPage(pPanel, statsPanelName[kPanelEmpty], false);
}

void NlNeuronComponentsInfoPanel::CreateStatsPanel(wxNotebook* book, wxPanel* pan, u32 numStats, const char** statNames, wxStaticText** pWxStaticTextArray, wxStaticText** pWxStoreLabels)
{
	wxBoxSizer* vsizer=new wxBoxSizer(wxVERTICAL);
	vsizer->AddSpacer(5);

	wxStaticText* pText;
	wxBoxSizer* hsizer;

	for(u32 n=0;n<numStats;++n)
	{
		hsizer=new wxBoxSizer(wxHORIZONTAL);
		hsizer->AddSpacer(5);
		pText = new wxStaticText(pan, -1, _( statNames[n] ));
		hsizer->Add(pText, 1, wxALIGN_LEFT);

		if(pWxStoreLabels)
			pWxStoreLabels[n] = pText;

		pText = new wxStaticText(pan, -1, wxEmptyString);
		hsizer->Add(pText,1,wxALIGN_RIGHT);
		vsizer->Add(hsizer,0,wxEXPAND);
		vsizer->AddSpacer(3);

		pWxStaticTextArray[n] = pText;
	}

	pan->SetSizer(vsizer);
	pan->Layout();
}

void NlNeuronComponentsInfoPanel::ConfigureComponentStatsPanel(u32 numStats, const char** statNames)
{
	for(u32 n=0;n<kNumTreeStatistic;++n)
	{
		if(n < numStats)
			componentLabelTxt[n]->SetLabel( _( statNames[n] ) );
		else
		{
			componentLabelTxt[n]->SetLabel( wxEmptyString );
			treeStatTxt[n]->SetLabel( wxEmptyString );
		}
	}
}


void NlNeuronComponentsInfoPanel::SetCurrent3rdPage(StatisticsPanel panel)
{
	switch(panel)
	{
	case kPanelSoma:
		ConfigureComponentStatsPanel(kNumSomaStatistic, somaStatName);
		break;
	case kPanelTree:
		ConfigureComponentStatsPanel(kNumTreeStatistic, treeStatName);
		break;
	case kPanelSpines:
		ConfigureComponentStatsPanel(kNumSpinesStatistic, spinesStatName);
		break;
	case kPanelMarkers:
		ConfigureComponentStatsPanel(kNumMarkersStatistic, markersStatName);
		break;
	case kPanelEmpty:
		ConfigureComponentStatsPanel(0, 0);
		break;
	}

	mDataNoteBook->SetPageText(2, _( statsPanelName[panel] ) );
}


/*
* WxNeuronElementsPropertiesListCtrlRightClick
*/
void NlNeuronComponentsInfoPanel::WxNeuronComponentListRightClick(wxListEvent& event)
{
	// insert your code here

}

