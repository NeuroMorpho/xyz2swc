//---------------------------------------------------------------------------
//
// Name:        NeuronComponentDisplayProperties.h
// Author:      VanessaNessie
// Created:     23/01/2009 13:31:33
// Description: NeuronItemDisplayData class declaration
//
//---------------------------------------------------------------------------

#ifndef __NEURONCOMPONENTDISPLAYDATA_h__
#define __NEURONCOMPONENTDISPLAYDATA_h__

#include <vector>


#define CUSTOMIZE_NEURON_COLOURS


enum NeuronComponentType
{
	kNeuronCompSoma=0,
	kNeuronCompDendrite,
	kNeuronCompAxon,
	kNeuronCompMarkers,
	kNeuronCompSpines
};

/**
* Drawable neuron individual item display settings
*/
struct NeuronComponentDisplayProperties
{  
	NeuronComponentType mType;  
    bool mIsVisible; // indicates whether the item is visible
    bool mShowMarckers;
	//colour
	unsigned char mRed;
	unsigned char mGreen;
	unsigned char mBlue;	
	unsigned char mAlpha;

	
	NeuronComponentDisplayProperties(NeuronComponentType neurontype): mType(neurontype),
							mIsVisible(true), 
							mShowMarckers(false)							
							{
								SetDefaultColour();
							}

	NeuronComponentDisplayProperties(NeuronComponentType neurontype,char red,char green, char blue,char alpha = 0xFF, bool visible = true, bool markers=true):
		mType(neurontype),
		mIsVisible(visible),
		mShowMarckers(markers),
		mRed(red),mGreen(green),
		mBlue(blue), mAlpha(alpha)
		{
		}

		/************************************************************************/
		/* Set default colour for type                                          */
		/************************************************************************/
	void SetDefaultColour() 
		{
			if (mType == kNeuronCompSoma)
			{	mRed=0x00;	
				mGreen=0x00;
				mBlue=0xFF;	
				mAlpha=0xFF;
			}
			if (mType == kNeuronCompDendrite)
			{
				mRed=(0x00);	
				mGreen=(0xFF);
				mBlue=(0x00);	
				mAlpha=(0xFF);
			}
			if (mType == kNeuronCompAxon)
			{
				mRed=(0xFF);	
				mGreen=(0x00);
				mBlue=(0x00);	
				mAlpha=(0xFF);
			}
			if (mType == kNeuronCompMarkers)
			{
				mRed=(0xFF);	
				mGreen=(0x00);
				mBlue=(0xFF);	
				mAlpha=(0xFF);
			}
			if (mType == kNeuronCompSpines)
			{
				mRed=(0x00);	
				mGreen=(0xFF);
				mBlue=(0xFF);	
				mAlpha=(0xFF);
			}
		}

    
};

typedef unsigned int  NLNeuronElementId;
//typedef std::map <NLNeuronElementId, NeuronComponentDisplayProperties> NLNeuronDisplayElements;
typedef std::vector <NeuronComponentDisplayProperties> NLNeuronDisplayElements;
/***
 * @todo well, this is here cos dunno where to put it. it should be somewhere in the opengl /scene or wherever
 */
inline NLNeuronDisplayElements& GetNeuronElementsDisplaySettings()
{
	static NLNeuronDisplayElements gNeuronDisplayElements;
	return gNeuronDisplayElements;
}



#endif
