#include "NeuronOperations.h"

#include "NeuronApplicationLogic/ApplicationLogic.h"
//#include "wxDialogs\wxNeuronExportFormatDialog.h"
#include "NeuronMorphologyFormat/MorphologyHandlerID.h"
#include "NeuronMorphologyFormat/MorphologyFormatID.h"
#include "NeuronRepresentation\NeuronComponentDisplayProperties.h"
#include <memory>
#include "Core/OutputFile.h"


/************************************************************************/
/* Exports the given neuron to the given format 
/* @param Neuron to be exported
/* @format to export to
/* @param file name
/* @return true if the neuron was exported correctly, false otherwise
/************************************************************************/
bool ExportNeuronDataToFile(Neuron3D& neuronToExport, BuiltInMorphologyFormat format, const String& fileName, const StyleOptions& styleOptions)
{
	std::auto_ptr<Output> pOut( OutputFile::open(fileName.c_str()) );
	if(pOut.get())
	{
		return ExportMorphologyFormat(format, *pOut, neuronToExport, styleOptions);
	}

	return false;
}

/************************************************************************/
/* Generate A Neuron that is a modified version of the original one
/* with user changes applied 
/*
/************************************************************************/
Neuron3D GenerateModifiedNeuron(MorphologyDataManager::NeuronID originalId,	NLNeuronDisplayElements& neuronDisplayData)
{

	Neuron3D* pNeuron = g_applicationLogic.GetMorphologyDataManager().GetNeuron3D(originalId);
	//make a copy of the original neuron, to apply modifications.
	Neuron3D tempNeuron = *pNeuron;

	const u32 numSomas = tempNeuron.CountSomas();
	const u32 numDends = tempNeuron.CountDendrites();
	const u32 numAxons = tempNeuron.CountAxons();
	const u32 numMarkers = (tempNeuron.CountAllMarkers() > 0 ? 1 : 0);
	const u32 numSpines = (tempNeuron.CountAllSpines() > 0 ? 1 : 0);

	//assert the number of elements is the same one 
	assert ( neuronDisplayData.size() == numSomas + numDends + numAxons + numMarkers + numSpines );

	int contSomas =0;
	int contDendrites=0;
	int elementIndex=0;

	//for each element in the list, if it is mark as "not visible" delete associated data from neuron
	// first chop out anything that isn't wanted
	Neuron3D::SomaIterator sit = tempNeuron.SomaBegin();
	Neuron3D::SomaIterator send = tempNeuron.SomaEnd();
	for (;elementIndex<numSomas && (sit!= send); elementIndex++)
	{
		assert (neuronDisplayData[elementIndex].mType == kNeuronCompSoma);
		if (neuronDisplayData[elementIndex].mIsVisible == false)
		{
			sit = tempNeuron.DeleteSoma(sit);
		}
		else
			sit++;
	}

	//Remove selected dendrites
	Neuron3D::DendriteIterator dit = tempNeuron.DendriteBegin();
	Neuron3D::DendriteIterator dend = tempNeuron.DendriteEnd();
	for (;elementIndex<numSomas+numDends && (dit!= dend); elementIndex++)
	{
		assert(neuronDisplayData[elementIndex].mType == kNeuronCompDendrite);
		if (neuronDisplayData[elementIndex].mIsVisible == false)
		{
			(*dit).clear();
		}
		dit++;		
	}

	//Remove selected Axons
	Neuron3D::AxonIterator ait = tempNeuron.AxonBegin();
	Neuron3D::AxonIterator aend = tempNeuron.AxonEnd();
	for (;elementIndex<numSomas+numDends+numAxons && (ait!= aend); elementIndex++)
	{
		assert (neuronDisplayData[elementIndex].mType == kNeuronCompAxon);
		if (neuronDisplayData[elementIndex].mIsVisible == false)
		{
			(*ait).clear();
		}
		ait++;		
	}
	//all neurons have been marked for deletion

	if(elementIndex < numSomas+numDends+numAxons+numMarkers)
	{
		assert (neuronDisplayData[elementIndex].mType == kNeuronCompMarkers);
		if (neuronDisplayData[elementIndex].mIsVisible == false)
			tempNeuron.ClearAllMarkerData();
		++elementIndex;
	}

	if(elementIndex < numSomas+numDends+numAxons+numMarkers+numSpines)
	{
		assert (neuronDisplayData[elementIndex].mType == kNeuronCompSpines);
		if (neuronDisplayData[elementIndex].mIsVisible == false)
			tempNeuron.ClearAllSpineData();
		++elementIndex;
	}


	// now erase all stump tree
	//add the bit for somas

	dit = tempNeuron.DendriteBegin();
	dend = tempNeuron.DendriteEnd();
	for(;dit!=dend;)
	{
		if( (*dit).root() == Neuron3D::DendriteTree::null() )
		{
			dit = tempNeuron.DeleteDendrite(dit);
		}
		else
		{
			++dit;
		}
	}
	ait = tempNeuron.AxonBegin();
	aend = tempNeuron.AxonEnd();
	for(;ait!=aend;)
	{
		if( (*ait).root() == Neuron3D::AxonTree::null() )
		{
			ait = tempNeuron.DeleteAxon(ait);
		}
		else
		{
			++ait;
		}
	}

	return tempNeuron;
}



/************************************************************************/
/*  Generate a vector with the list of formats that a Neuron can be exported to
/* @param Neuron3D
/* @param vector to set with the exportable formats
/* @return true if there are formats to be exported to, false otherwise
/************************************************************************/
bool GetExportableMorphologyFormats(Neuron3D& neuronToExport, std::vector <BuiltInMorphologyFormat>& exportableFormats)
{	
	for (int i= kMorphologyFormatFirst;  i< (int)kMorphologyFormatBannister ;i++)
	{
		if( neuronToExport.HasOnlyBinaryTrees() || !IsFormatBinaryOnly((BuiltInMorphologyFormat)i ))
		{
			// need better check for suitable export...(look at test program...)
			//if(neuronToExport.CountAxons() > 0 && !DoesFormatSupportAxons((BuiltInMorphologyFormat)i ))
			//{
			//	if(neuronToExport.CountDendrites() > 0)
			//	{
					//printf("Original file (after any modifications) has axon data. Selected format (%s) does not support this. Axons will be ignored\n", (const char*)GetBuiltInMorphologyFormatDetails(eOptions.outputFormat).GetName());
				
			//	}
			//	else
			//	{
					/*
					printf("Original file (after any modifications) only has axon data. 
						Selected format (%s) does not support this.\n", (const char*)GetBuiltInMorphologyFormatDetails(eOptions.outputFormat).GetName());
					*/
			//		break;
			//	}
			//}
			exportableFormats.push_back((BuiltInMorphologyFormat)i);
		}
		//do not add to the exportable formats.
	}
	return exportableFormats.size() > 0;

}



//void ExportModifiedNeuronDataAs(MorphologyDataManager::NeuronID originalId, BuiltInMorphologyFormat format,const NLNeuronDisplayElements& neuronDisplayData)
//{}

/************************************************************************/
/* Generate a duplicate from the original neuron and exports it to the 
/* selected		format													 */
/************************************************************************/
#if 0
bool ExportModifiedNeuronDataAs(MorphologyDataManager::NeuronID originalId,
								BuiltInMorphologyFormat format, 
								NLNeuronDisplayElements& neuronDisplayData,
								const String& fileName, const StyleOptions& styleOptions)
{

	Neuron3D* pNeuron = g_applicationLogic.GetMorphologyDataManager().GetNeuron3D(originalId);
	//make a copy of the original neuron, to apply modifications.
	Neuron3D tempNeuron = *pNeuron;

	//assert the number of elements is the same one 
	assert ( neuronDisplayData.size() == tempNeuron.CountSomas()+ tempNeuron.CountDendrites()+tempNeuron.CountAxons());

	int contSomas =0;
	int contDendrites=0;
	int elementIndex=0;

	//for each element in the list, if it is mark as "not visible" delete asociated dta from neuron
	// first chop out anything that isn't wanted
	Neuron3D::SomaIterator sit = tempNeuron.SomaBegin();
	Neuron3D::SomaIterator send = tempNeuron.SomaEnd();
	for (;(elementIndex<tempNeuron.CountSomas()) && (sit!= send); elementIndex++)
	{
		if (neuronDisplayData[elementIndex].mType != kNeuronCompSoma)
		{
			return false;
		}
		else
		{
			if (neuronDisplayData[elementIndex].mIsVisible == false)
			{
				//(*sit).clear();??
				//tempNeuron.DeleteSoma();??
			}
			sit++;
		}
	}
	//Remove selected dendrites
	Neuron3D::DendriteIterator dit = tempNeuron.DendriteBegin();
	Neuron3D::DendriteIterator dend = tempNeuron.DendriteEnd();
	for (;(elementIndex<tempNeuron.CountSomas()+tempNeuron.CountDendrites()) && (dit!= dend); elementIndex++)
	{
		if (neuronDisplayData[elementIndex].mType != kNeuronCompDendrite)
		{
			return false;
		}
		else
		{
			if (neuronDisplayData[elementIndex].mIsVisible == false)
			{
				(*dit).clear();
				//tempNeuron.DeleteDendrite();
			}
			dit++;
		}
	}
	//Remove selected Axons
	Neuron3D::AxonIterator ait = tempNeuron.AxonBegin();
	Neuron3D::AxonIterator aend = tempNeuron.AxonEnd();
	for (;(elementIndex<neuronDisplayData.size()) && (ait!= aend); elementIndex++)
	{
		if (neuronDisplayData[elementIndex].mType != kNeuronCompAxon)
		{
			return false;
		}
		else
		{
			if (neuronDisplayData[elementIndex].mIsVisible == false)
			{
				(*dit).clear();
				//tempNeuron.DeleteDendrite();
			}
			dit++;
		}
	}
	//all neurons have been marked for deletion

	// now erase all stump tree

	dit = tempNeuron.DendriteBegin();
	dend = tempNeuron.DendriteEnd();
	for(;dit!=dend;)
	{
		if( (*dit).root() == Neuron3D::DendriteTree::null() )
		{
			dit = tempNeuron.DeleteDendrite(dit);
		}
		else
		{
			++dit;
		}
	}
	ait = tempNeuron.AxonBegin();
	aend = tempNeuron.AxonEnd();
	for(;ait!=aend;)
	{
		if( (*ait).root() == Neuron3D::AxonTree::null() )
		{
			ait = tempNeuron.DeleteAxon(ait);
		}
		else
		{
			++ait;
		}
	}

	bool exported = ExportNeuronDataToFile(tempNeuron,format,fileName, styleOptions);
	return exported;

}
#endif