#ifndef NEURON3D_HPP_
#define NEURON3D_HPP_

#include <float.h>

#include <vector>
#include <map>
#include <algorithm>

#include "Core/v4f.h"

#include "SamplePoint.h"
#include "MarkerPoint.h"
#include "SpinePoint.h"

#include "ArtificialBranchingConfiguration.h"

#include "TerminalType.h"

#include "NeuronGeneric.h"


struct StructureColour
{
	u8 r;
	u8 g;
	u8 b;
};

struct Branch3D
{
	Branch3D() : terminalType(kTerminal_Normal) {}
	~Branch3D() {}

	//
	StructureColour colour;
	void SetColour(u8 r, u8 g, u8 b) {colour.r = r;colour.g = g;colour.b = b;}
	void GetColour(u8& r, u8& g, u8& b) {r = colour.r;g = colour.g;b = colour.b;}
	//

	typedef std::vector<MarkerPoint>	MarkerSet;

	struct Markers : public std::map<String, MarkerSet>
	{
		bool HasMarkers() const
		{
			for(const_iterator bit = begin();bit != end();++bit)
			{
				if(bit->second.size() > 0)
					return true;
			}

			return false;
		}
	};
	typedef std::vector<SamplePoint>	Samples;
	typedef std::vector<SpinePoint>		Spines;

	Markers m_markers;
	Samples m_samples;
	Spines	m_spines;

	TerminalType terminalType; // only meaningful for child-less branches...

	void clear()
	{
		m_samples.clear();
		m_markers.clear();
		m_spines.clear();
	}

	void SetTerminalType(TerminalType type)
	{
		terminalType = type;
	}

	TerminalType GetTerminalType() const
	{
		return terminalType;
	}

	float GetLength() const
	{
		float length = 0.0f;
		if(m_samples.size() > 1)
		{
			std::vector<SamplePoint>::const_iterator prev = m_samples.begin();
			std::vector<SamplePoint>::const_iterator it = prev; ++it;
			std::vector<SamplePoint>::const_iterator end = m_samples.end();
			
			for(;it!=end;++it,++prev)
				length += (*it).Distance(*prev);
		}

		return length;
	}

	float GetVolume() const
	{
		float volume = 0.0f;

		if(m_samples.size() > 1)
		{
			std::vector<SamplePoint>::const_iterator prev = m_samples.begin();
			std::vector<SamplePoint>::const_iterator it = prev; ++it;
			std::vector<SamplePoint>::const_iterator end = m_samples.end();

			for(;it!=end;++it,++prev)
			{
				// volume of tapering cylinder - assuming linear change in diameter between sample points.
				// /int_0_L {PI r^2 dx}, where  r = rs + x/L (rf - rs).  (rs = start radius, rf = end radius)
				const float length = (*it).Distance(*prev);
				const float startDiam = (*prev).d;
				const float endDiam = (*it).d;
				volume +=  3.1415926f * length * (  (startDiam*startDiam) + (endDiam*endDiam) + (startDiam*endDiam) ) / 12;
			}
		}

		return volume;
	}

	float GetSurfaceArea() const
	{
		float area = 0.0f;

		if(m_samples.size() > 1)
		{
			std::vector<SamplePoint>::const_iterator prev = m_samples.begin();
			std::vector<SamplePoint>::const_iterator it = prev; ++it;
			std::vector<SamplePoint>::const_iterator end = m_samples.end();

			for(;it!=end;++it,++prev)
			{
				// surface area of tapering cylinder - assuming linear change in diameter between sample points.
				// /int_0_L {2 PI r dx}, where  r = rs + x/L (rf - rs).  (rs = start radius, rf = end radius)
				const float length = (*it).Distance(*prev);
				const float startDiam = (*prev).d;
				const float endDiam = (*it).d;
				area += 0.5f * 3.1415926f * length * (startDiam + endDiam);
			}
		}

		return area;
	}

	float GetMinDiam() const;
	float GetMaxDiam() const;

	u32 GetNumSpines() const 
	{
		return (u32) m_spines.size();
	}
	u32 GetNumMarkers() const 
	{
		u32 count = 0;
		for(Markers::const_iterator mit=m_markers.begin();mit!=m_markers.end();++mit)
		{
			count += (u32)mit->second.size();
		}
		return count;
	}

	// TODO: redo
	std::pair<u32, float> CalculateNearestSegment(SamplePoint pt)
	{
		float closestDistance = FLT_MAX;
		u32 closestPoint = 0;

		for(u32 n=1;n<m_samples.size();++n)
		{
			float distance = m_samples[n].Distance(pt); 
			if(distance < closestDistance)
			{
				closestDistance = distance;
				closestPoint = n;
			}
		}

		return std::make_pair(closestPoint, closestDistance);
	}
};

bool operator !=(const Branch3D& lhs, const Branch3D& rhs);


struct Soma3D : public Branch3D
{
	enum SomaType 
	{
		kPoint,
		kSegment,
		kContourClosed,
		kContourOpen
	};

	SomaType somaType;

	Soma3D() : somaType(kPoint) {}
};

class Neuron3D : public NeuronGeneric<Branch3D, Soma3D, Branch3D>
{
public:
			Neuron3D();
			~Neuron3D();

	//Neuron3D& operator=(const Neuron3D& src);

	template<class unaryfunc>
	void	DoForAllBranches(unaryfunc uf);


	u32	CountTotalSamplePoints() const;
	u32	CountAllDendriteSamplePoints() const;
	u32	CountAllAxonSamplePoints() const;
	
	// temp.. probably a better way to calculate this.. (for amria mesh skeleton)
	u32	CountAllDendriteNeighbourPoints() const;
	u32	CountAllAxonNeighbourPoints() const;
	//

	u32	CountDendriteSamplePoints(DendriteTree::const_iterator root) const;
	u32	CountAxonSamplePoints(AxonTree::const_iterator root) const;

	u32 CountAllSpines() const;
	u32 CountDendriteSpines() const;
	u32 CountAxonSpines() const;
	u32 CountGlobalSpines() const;

	u32 CountAllMarkers() const;
	u32 CountGlobalMarkers() const;
	u32 CountSomaMarkers() const;
	u32 CountDendriteMarkers() const;
	u32 CountAxonMarkers() const;

	void MergeDendriteUnaryBranchPoints();
	void MergeAxonUnaryBranchPoints();
	void MergeAllUnaryBranchPoints();

	void EnhanceSingleSampleTrees();

	void AddMarker(const String& name, const MarkerPoint& marker);

	void ClearAllMarkerData();
	void ClearGlobalMarkerData();
	void ClearSomaMarkerData();
	void ClearAxonMarkerData();
	void ClearDendriteMarkerData();

	void AddSpine(const SpinePoint& marker);
	void ClearAllSpineData();
	void ClearGlobalSpineData();
	void ClearDendriteSpineData();
	void ClearAxonSpineData();

	bool HasOnlyBinaryTrees() const;
	bool HasOnlyBinaryAxons() const;
	bool HasOnlyBinaryDends() const;
	bool DoUnconnectedSamplesShareLocation() const;

	bool HasMarkers() const;
	bool HasSpines() const;

	void Rotate(v4f rotation);
	void Translate(v4f translation);
	void Scale(v4f scale);

	v4f CalculateSomaCentre() const;

	// WARNING: FOR USE WITH 1D formats.... 
	// assumes that neuron branches samples are preprocessed into the format  (length, 0, 0, diameter)
	void ApplyArtificialBranchingSubTree(Neuron3D::DendriteTreeIterator it, const SamplePoint startPoint, const float startAngle, const float angleIncrement, float childRangeAngle);
	void ApplyArtificialBranching(const ArtificialBranchingConfiguration& config);
	//
	//

	void ConvertAllMarkersToSpines();
	void ConvertNamedMarkersToSpines(const std::vector<String>& markerNames);
	void ConvertAllSpinesToMarkers(const String& name);

	void MapGlobalSpinesToBranches(float limit);
	void MapGlobalMarkersToBranches(float limit);

	void MakeAllSpinesGlobal();
	void MakeAllMarkersGlobal();

	void ConvertTreeSpinesToBranches();

	void RenameMarkers(const std::map<String, String>& rename);

	void SetGlobalTreeTerminalType(TerminalType type);

	void ReverseChildOrderAllTrees();
	static void ReverseChildOrderSubTree(DendriteTreeIterator it);


	std::pair<DendriteTreeIterator, u32> FindNearestBranchSamplePoint(SamplePoint pt, float limit);


	struct LengthAccumulator : DendriteTree::SingleItemAccumulator
	{
		void operator()(DendriteTreeConstIterator it)
		{
			Add( it->GetLength() );
		}
	};

	struct SampleCounter : DendriteTree::SingleItemCounter
	{
		template<typename T>
		void operator()(T it)
		{
			Add( (u32)it->m_samples.size() - 1);
		}
	};

	struct NeighbourCounter : DendriteTree::SingleItemCounter
	{
		template<typename T>
		void operator()(T it)
		{
			assert(it->m_samples.size()>=2);

			const u32 neighboursInternal = ( (u32)it->m_samples.size() - 2 ) * 2;
			const u32 neighboursEnd = 1 + DendriteTree::countchildren(it);

			Add( neighboursInternal + neighboursEnd );
		}
	};

	struct MaxDistancePathAccumulator : DendriteTree::MaxAccumulater<float>
	{
		MaxDistancePathAccumulator() : DendriteTree::MaxAccumulater<float>(0.0f) {}

		void operator()(DendriteTreeConstIterator it)
		{
			if(!it.child())
			{
				DendriteTreeConstIterator rootSeeker = it;
				float maxLength = 0.0f;
				do
				{
					maxLength += rootSeeker->GetLength();
					rootSeeker = rootSeeker.parent();
				}
				while(rootSeeker);

				DendriteTree::MaxAccumulater<float>::MaxWith(maxLength);
			}
		}
	};

	struct MaxDistancePhysAccumulator : DendriteTree::MaxAccumulater<float>
	{
		MaxDistancePhysAccumulator() : DendriteTree::MaxAccumulater<float>(0.0f) {}

		void operator()(DendriteTreeConstIterator it)
		{
			if(!it.child())
			{
				DendriteTreeConstIterator rootSeeker = it;

				while(rootSeeker.parent())
					rootSeeker = rootSeeker.parent();

				const float physDistance = it->m_samples[it->m_samples.size()-1].Distance(rootSeeker->m_samples[0]);

				DendriteTree::MaxAccumulater<float>::MaxWith(physDistance);
			}
		}
	};

	struct MaxDiamAccumulator : DendriteTree::MaxAccumulater<float>
	{
		MaxDiamAccumulator() : DendriteTree::MaxAccumulater<float>(0.0f) {}

		void operator()(DendriteTreeConstIterator it)
		{
			DendriteTree::MaxAccumulater<float>::MaxWith( it->GetMaxDiam() );
		}
	};

	struct MinDiamAccumulator : DendriteTree::MinAccumulater<float>
	{
		MinDiamAccumulator() : DendriteTree::MinAccumulater<float>(FLT_MAX) {}

		void operator()(DendriteTreeConstIterator it)
		{
			DendriteTree::MinAccumulater<float>::MinWith( it->GetMinDiam() );
		}
	};

	struct VolumeAccumulator : DendriteTree::ValueAccumulater<float>
	{
		VolumeAccumulator() : DendriteTree::ValueAccumulater<float>(0.0f) {}

		void operator()(DendriteTreeConstIterator it)
		{
			Add( it->GetVolume() );
		}
	};

	struct SurfaceAreaAccumulator : DendriteTree::ValueAccumulater<float>
	{
		SurfaceAreaAccumulator() : DendriteTree::ValueAccumulater<float>(0.0f) {}

		void operator()(DendriteTreeConstIterator it)
		{
			Add( it->GetSurfaceArea() );
		}
	};

	struct SpineCounter : DendriteTree::SingleItemCounter
	{
		template<typename T>
		void operator()(T it)
		{
			Add( it->GetNumSpines() );
		}
	};

	struct MarkerCounter : DendriteTree::SingleItemCounter
	{
		template<typename T>
		void operator()(T it)
		{
			Add( it->GetNumMarkers() );
		}
	};

private:
	//float m_cm, m_ga, m_gm, m_gs;
	String m_header;

public:
	// convenient to store global markers and spines here
	Branch3D m_globalData;
};


bool operator!= (const Neuron3D& nrnLhs, const Neuron3D& nrnRhs);


#endif // NEURON3D_HPP_
