#ifndef NEURONOPERATIONS_H_
#define NEURONOPERATIONS_H_


#include "NeuronMorphologyFormat/MorphologyFormatID.h"
#include "NeuronMorphologyFormatImportExportBuiltIn\BuiltInMorphologyFormat.h"
#include "NeuronMorphologyDataManager\MorphologyDataManager.h"
#include "NeuronRepresentation\NeuronComponentDisplayProperties.h"


/************************************************************************/
/* Exports the given neuron to the given format 
/* @param Neuron to be exported
/* @param file name
/* @format to export to
/* @return true if the neuron was exported correctly, false otherwise
/************************************************************************/
bool ExportNeuronDataToFile(Neuron3D& neuronToExport,BuiltInMorphologyFormat format, const String& fileName, const StyleOptions& styleOptions);

/************************************************************************/
/* Generate A Neuron that is a modified version of the original one
/* with user changes applied 
/*
/************************************************************************/
Neuron3D GenerateModifiedNeuron(MorphologyDataManager::NeuronID originalId,	NLNeuronDisplayElements& neuronDisplayData);


/************************************************************************/
/*  Generate a vector with the list of formats that a Neuron can be exported to
/* @param Neuron3D
/* @param vector to set with the exportable formats
/* @return true if there are formats to be exported to, false otherwise
/************************************************************************/
bool GetExportableMorphologyFormats(Neuron3D& neuronToExport, std::vector <BuiltInMorphologyFormat>& exportableFormats);


/*
bool ExportModifiedNeuronDataAs(MorphologyDataManager::NeuronID originalId,
								BuiltInMorphologyFormat format, 
								NLNeuronDisplayElements& neuronDisplayData,
								const String& fileName);

void ExportModifiedNeuronDataAs(MorphologyDataManager::NeuronID originalId, BuiltInMorphologyFormat format,const NLNeuronDisplayElements& neuronDisplayData);
*/
#endif
