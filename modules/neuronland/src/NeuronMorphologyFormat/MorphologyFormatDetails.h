#ifndef MORPHOLOGYFORMATSDETAILS_HPP_
#define MORPHOLOGYFORMATSDETAILS_HPP_

#include "Core/String.hpp"
#include "Types.h"

/**

	@brief Contains some fundamental information about the nature and limitations of a morphology file format

	@todo - use XML Schema to  C++ code generator

**/

class MorphologyFormatDetails
{
public:

				MorphologyFormatDetails();
				MorphologyFormatDetails(const String& name);
				MorphologyFormatDetails(class Input& rIn);	///< input from XML file

	enum OriginDataSource
	{
		kDataSourceExperimental,
		kDataSourceDerived,
		kDataSourceAlgorithm
	};

	enum DataFileEncoding
	{
		kEncodingAscii,
		kEncodingUTF8,
		kEncodingBinary
	};

	enum DataFileEndOfLine
	{
		kEndOfLineCRLF,
		kEndOfLineCR
	};

	enum SomaSupport
	{
		kSomaUnsupported,
		kSomaPoint,
		kSomaContourOpenSingle,
		kSomaContourClosedSingle,
		kSomaContourOpenMultiple,
		kSomaContourClosedMultiple
	};

	enum AxonSupport
	{
		kAxonUnsupported,
		kAxonAsTree,
		kAxonSingle,
		kAxonMultiple
	};

	enum DendriteSupport
	{
		kDendriteUnsupported,
		kDendriteAsTree,
		kDendriteSingle,
		kDendriteMultiple
	};

	enum GeometryRepresentation
	{
		Geometry2D,
		Geometry3D
	};

	enum TreeBranchingLimit
	{
		kBranchingLimitUnary,
		kBranchingLimitBinary,
		kBranchingLimitUnlimited	
	};


private:
	// properties
	String					m_name;
	String					m_description;
	String					m_software;
	OriginDataSource		m_dataSource;
	String					m_extensions;
	DataFileEncoding		m_dataFileEncoding;				
	SomaSupport				m_somaSupport;					
	AxonSupport				m_axonSupport;					
	DendriteSupport			m_dendriteSupport;				
	GeometryRepresentation	m_geometryRepresentation;			
	bool					m_cellMarkers;
	bool					m_somaMarkers;
	bool					m_peerlessRoot;
	TreeBranchingLimit		m_branchingLimit;
	bool					m_branchExplicitConnectivity;
	bool					m_branchRepeatBranchPointSample;
	u32						m_branchOrderLimit;
	bool					m_branchMarkers;
	u32						m_sampleUnitsPosition;
	u32						m_sampleUnitsDiameter;
	bool					m_visualMarkerColours;
	bool					m_visualBranchColours;

public:

	String					GetName() const											{ return m_name; }
	String					GetDescription() const									{ return m_description; }
	String					GetSoftware() const										{ return m_software; }
	OriginDataSource		GetDataSource() const									{ return m_dataSource; }
	String					GetExtensions() const									{ return m_extensions; }
	DataFileEncoding		GetDataFileEncoding() const								{ return m_dataFileEncoding; }
	SomaSupport				GetSomaSupport() const									{ return m_somaSupport;	}
	AxonSupport				GetAxonSupport() const									{ return m_axonSupport;	}
	DendriteSupport			GetDendriteSupport() const								{ return m_dendriteSupport;	}
	GeometryRepresentation	GetGeometryRepresentation() const						{ return m_geometryRepresentation; }
	bool					GetCellMarkers() const 									{ return m_cellMarkers; }
	bool					GetSomaMarkers() const 									{ return m_somaMarkers; }
	bool					GetPeerlessRoot() const									{ return m_peerlessRoot; }
	TreeBranchingLimit		GetBranchingLimit() const								{ return m_branchingLimit; }
	bool					GetBranchExplicitConnectivity() const					{ return m_branchExplicitConnectivity; }
	bool					GetBranchRepeatBranchPointSample() const				{ return m_branchRepeatBranchPointSample; }
	u32						GetBranchOrderLimit() const								{ return m_branchOrderLimit; }
	bool					GetBranchMarkers() const								{ return m_branchMarkers; }
	u32						GetSampleUnitsPosition() const							{ return m_sampleUnitsPosition; }
	u32						GetSampleUnitsDiameter() const							{ return m_sampleUnitsDiameter; }
	bool					GetVisualMarkerColours() const 							{ return m_visualMarkerColours; }
	bool					GetVisualBranchColours() const 							{ return m_visualBranchColours; }

	void					SetName(String value)									{ m_name = value; }
	void					SetDescription(String value)							{ m_description = value; }
	void					SetSoftware(String value)								{ m_software = value; }
	void					SetDataSource(OriginDataSource value)					{ m_dataSource = value; }
	void 					SetExtensions(String value)								{ m_extensions = value; }
	void 					SetDataFileEncoding(DataFileEncoding value) 			{ m_dataFileEncoding = value; }				
	void 					SetSomaSupport(SomaSupport value) 						{ m_somaSupport = value; }					
	void 					SetAxonSupport(AxonSupport value) 						{ m_axonSupport = value; }					
	void 					SetDendriteSupport(DendriteSupport value)				{ m_dendriteSupport = value; }				
	void 					SetGeometryRepresentation(GeometryRepresentation value) { m_geometryRepresentation = value; }			
	void 					SetCellMarkers(bool value) 								{ m_cellMarkers = value; }					
	void 					SetSomaMarkers(bool value) 								{ m_somaMarkers = value; }					
	void 					SetPeerlessRoot(bool value)								{ m_peerlessRoot = value; }					
	void 					SetBranchingLimit(TreeBranchingLimit value)				{ m_branchingLimit = value; }
	void 					SetBranchExplicitConnectivity(bool value)				{ m_branchExplicitConnectivity = value; }		
	void 					SetBranchRepeatBranchPointSample(bool value)			{ m_branchRepeatBranchPointSample = value; }
	void 					SetBranchMarkers(u32 value)								{ m_branchOrderLimit = value; }
	void 					SetBranchMarkers(bool value)							{ m_branchMarkers = value; }
	void 					SetSampleUnitsPosition(u32 value) 						{ m_sampleUnitsPosition = value; }
	void 					SetSampleUnitsDiameter(u32 value) 						{ m_sampleUnitsDiameter = value; }
	void 					SetVisualMarkerColours(bool value) 						{ m_visualMarkerColours = value; }
	void 					SetVisualBranchColours(bool value) 						{ m_visualBranchColours = value; }

};

#endif // MORPHOLOGYFORMATSDETAILS_HPP_
