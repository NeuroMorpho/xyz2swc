//
// Neuron3D.cpp
//

#include <algorithm>

#include "Neuron3D.h"

#include "Core/m44f.h"
#include "Core/v4f.h"


bool operator !=(const Branch3D& lhs, const Branch3D& rhs)
{
	// check if sample data is identical
	if( lhs.m_samples.size() == rhs.m_samples.size() )
	{
		for(u32 n=1;n<lhs.m_samples.size();++n)
		{
			const SamplePoint sampleLhs = lhs.m_samples[n];
			const SamplePoint sampleRhs = rhs.m_samples[n];

			if(sampleLhs != sampleRhs)
				return true;
		}
	}
	else
		return true;

	// markers
	if( lhs.m_markers.size() == rhs.m_markers.size() )
	{
		Branch3D::Markers::const_iterator itL = lhs.m_markers.begin();
		Branch3D::Markers::const_iterator itR = rhs.m_markers.begin();
		for(;itL!=lhs.m_markers.end();++itL, ++itR)
		{
			if( itL->first != itR->first || 
				itL->second.size() != itR->second.size() )
				return true;

			for(u32 n=0;n<itL->second.size();++n)
			{
				const SamplePoint markerLhs = itL->second[n];
				const SamplePoint markerRhs = itR->second[n];

				if(markerLhs != markerRhs)
					return true;
			}
		}
	}
	else
		return true;

	// spines
	if( lhs.m_spines.size() == rhs.m_spines.size() )
	{
		for(u32 n=1;n<lhs.m_spines.size();++n)
		{
			const SpinePoint spineLhs = lhs.m_spines[n];
			const SpinePoint spineRhs = rhs.m_spines[n];

			if(spineLhs != spineRhs)
				return true;
		}
	}
	else
		return true;


	return false;
}



float Branch3D::GetMinDiam() const
{
	float min_d = FLT_MAX;
	for(u32 n=0;n<m_samples.size();++n)
		min_d = std::min(min_d, m_samples[n].d);

	return min_d;
}

float Branch3D::GetMaxDiam() const
{
	float max_d = 0.0f;
	for(u32 n=0;n<m_samples.size();++n)
		max_d = std::max(max_d, m_samples[n].d);

	return max_d;
}




Neuron3D::Neuron3D()
{
//	m_ga = 1.0;
//	m_gs = 1.0;
//	m_gm = 1.0;
//	m_cm = 1.0;
}

Neuron3D::~Neuron3D()
{

}

/*Neuron3D& Neuron3D::operator=(const Neuron3D& src)
{
	m_somas = src.m_somas;
	m_axons = src.m_axons;
	m_dendrites = src.m_dendrites;
	m_globalData = src.m_globalData;
}*/


u32 Neuron3D::CountTotalSamplePoints() const
{
	const int rootPoints = CountAxons() + CountDendrites();

	SampleCounter c;
	DoForAllSomas(c);
	DoForAllAxonElements(c);
	DoForAllDendriteElements(c);

	return rootPoints + c.GetCount();
}

u32	Neuron3D::CountAllDendriteSamplePoints() const
{
	SampleCounter c;
	DoForAllDendriteElements(c);
	return c.GetCount() + CountDendrites();
}

u32	Neuron3D::CountAllAxonSamplePoints() const
{
	SampleCounter c;
	DoForAllAxonElements(c);
	return c.GetCount() + CountAxons();
}

u32	Neuron3D::CountAllDendriteNeighbourPoints() const
{
	NeighbourCounter c;
	DoForAllDendriteElements(c);
	return c.GetCount() + CountDendrites();
}

u32	Neuron3D::CountAllAxonNeighbourPoints() const
{
	NeighbourCounter c;
	DoForAllAxonElements(c);
	return c.GetCount() + CountAxons();
}


u32	Neuron3D::CountDendriteSamplePoints(DendriteTreeConstIterator root) const
{
	SampleCounter c;
	DoForDendriteElements(root, c);
	return c.GetCount() + 1;
}

u32	Neuron3D::CountAxonSamplePoints(AxonTreeConstIterator root) const
{
	SampleCounter c;
	DoForAxonElements(root, c);
	return c.GetCount() + 1;
}


u32 Neuron3D::CountAllSpines() const
{
	return CountGlobalSpines() + CountDendriteSpines() + CountAxonSpines();
}

u32 Neuron3D::CountGlobalSpines() const
{
	SpineCounter c;
	c(&m_globalData);
	return c.GetCount();
}

u32 Neuron3D::CountDendriteSpines() const
{
	SpineCounter c;
	DoForAllDendriteElements(c);
	return c.GetCount();
}

u32 Neuron3D::CountAxonSpines() const
{
	SpineCounter c;
	DoForAllAxonElements(c);
	return c.GetCount();
}

u32 Neuron3D::CountAllMarkers() const
{
	return CountGlobalMarkers() + CountSomaMarkers() + CountDendriteMarkers() + CountAxonMarkers();
}

u32 Neuron3D::CountGlobalMarkers() const
{
	MarkerCounter c;
	c(&m_globalData);
	return c.GetCount();
}

u32 Neuron3D::CountSomaMarkers() const
{
	MarkerCounter c;
	DoForAllSomas(c);
	return c.GetCount();
}

u32 Neuron3D::CountDendriteMarkers() const
{
	MarkerCounter c;
	DoForAllDendriteElements(c);
	return c.GetCount();
}

u32 Neuron3D::CountAxonMarkers() const
{
	MarkerCounter c;
	DoForAllAxonElements(c);
	return c.GetCount();
}


struct UnaryBranchPointMergerer
{
	struct BranchCombiner
	{
		void operator() (Neuron3D::DendriteTreeIterator itDest, Neuron3D::DendriteTreeIterator itSrc)
		{
			const u32 numOrigDestSamples = (*itDest).m_samples.size();
			const u32 numorigDestSpines = (*itDest).m_spines.size();

			// samples
			// simple insert (ignore src most proximal sample)
			(*itDest).m_samples.insert((*itDest).m_samples.end(), (*itSrc).m_samples.begin() + 1, (*itSrc).m_samples.end());

			// spines
			// insert, update associated segments
			(*itDest).m_spines.insert((*itDest).m_spines.end(), (*itSrc).m_spines.begin(), (*itSrc).m_spines.end());
			for(u32 n=numorigDestSpines;n<(*itDest).m_spines.size();++n)
			{
				(*itDest).m_spines[n].associatedSegment += (numOrigDestSamples-1);
			}

			// markers
			for(Branch3D::Markers::const_iterator it = (*itSrc).m_markers.begin();it!=(*itSrc).m_markers.end();++it)
			{
				Branch3D::MarkerSet& ms = (*itDest).m_markers[it->first];
				ms.insert(ms.end(), it->second.begin(), it->second.end());
			}

		}
	};

	void operator() (Neuron3D::DendriteIterator it)
	{
		BranchCombiner c;
		Neuron3D::DendriteTree::merge_unary_branching((*it).root(), c);
	}
};

void Neuron3D::MergeDendriteUnaryBranchPoints()
{
	UnaryBranchPointMergerer m;
	DoForAllDendrites(m);
}

void Neuron3D::MergeAxonUnaryBranchPoints()
{
	UnaryBranchPointMergerer m;
	DoForAllAxons(m);
}

void Neuron3D::MergeAllUnaryBranchPoints()
{
	MergeDendriteUnaryBranchPoints();
	MergeAxonUnaryBranchPoints();
}

// ugh for now...
class TestBinary
{
public:
	bool binary;

	TestBinary()
	{
		binary = true;
	}

	void operator()(Neuron3D::DendriteConstIterator it)
	{
		binary = binary && Neuron3D::DendriteTree::is_binary( (*it).root() );
	}
};

bool Neuron3D::HasOnlyBinaryTrees() const
{
	TestBinary testFunctor;

	DoForAllDendrites(testFunctor);
	DoForAllAxons(testFunctor);

	return testFunctor.binary;
}

bool Neuron3D::HasOnlyBinaryAxons() const
{
	TestBinary testFunctor;

	DoForAllAxons(testFunctor);

	return testFunctor.binary;
}

bool Neuron3D::HasOnlyBinaryDends() const
{
	TestBinary testFunctor;

	DoForAllDendrites(testFunctor);

	return testFunctor.binary;
}


class TestMarkers
{
public:
	bool markers;

	TestMarkers() : markers(false)
	{
	}

	template<typename T>
	void operator()(T it)
	{
		if(markers)
			return;

		if(it->m_markers.HasMarkers())
			markers = true;
	}
};

bool Neuron3D::HasMarkers() const
{
	TestMarkers testFunctor;

	if (m_globalData.m_markers.HasMarkers())
		return true;

	// really need alternative version which stops testing as soon as we hit true...
	DoForAllTreeElements(testFunctor);

	return testFunctor.markers;
}

class TestSpines
{
public:
	bool spines;

	TestSpines() : spines(false)
	{
	}

	template<typename T>
	void operator()(T it)
	{
		spines = spines || ((*it).m_spines.size() > 0);
	}
};

bool Neuron3D::HasSpines() const
{
	TestSpines testFunctor;

	if (m_globalData.m_spines.size() > 0)
		return true;

	DoForAllTreeElements(testFunctor);

	return testFunctor.spines;
}

bool Neuron3D::DoUnconnectedSamplesShareLocation() const
{
	return false;
}

struct ExpandSingleSampleTree
{
	void operator()(Neuron3D::DendriteIterator it)
	{
		// some dendrites/axons can have only one sample - lets make sure they have at least 2...
		if(	(!(*it).root().child()) &&
			(*it).root()->m_samples.size() == 1)
		{
			(*it).root()->m_samples.push_back((*it).root()->m_samples[0]);
		}
	}
};

struct SampleTranslator
{
	v4f mTranslation;

	SampleTranslator(v4f translation) : mTranslation(translation) {}

	template<typename T>
	void operator()(T it)
	{
		//for_each((*it).m_samples.begin(), (*it).m_samples.end(), std::bind2nd(plus<SamplePoint>(), mTranslation)  );

		for( u32 n=0;n<(*it).m_samples.size();++n)
		{
			(*it).m_samples[n].x += mTranslation.GetX();
			(*it).m_samples[n].y += mTranslation.GetY();
			(*it).m_samples[n].z += mTranslation.GetZ();
		}

		for(Branch3D::Markers::iterator mit=(*it).m_markers.begin();mit!=(*it).m_markers.end();++mit)
		{
			for( u32 n=0;n<(*mit).second.size();++n)
			{
				MarkerPoint& pt = (*mit).second[n];
				pt.x += mTranslation.GetX();
				pt.y += mTranslation.GetY();
				pt.z += mTranslation.GetZ();
			}
		}

		for( u32 n=0;n<(*it).m_spines.size();++n)
		{
			(*it).m_spines[n].x += mTranslation.GetX();
			(*it).m_spines[n].y += mTranslation.GetY();
			(*it).m_spines[n].z += mTranslation.GetZ();
		}
	}
};

struct SampleScaler
{
	v4f m_scale;

	SampleScaler(v4f scale) : m_scale(scale) {}

	template<typename T>
	void operator()(T it)
	{
		for( u32 n=0;n<(*it).m_samples.size();++n)
		{
			(*it).m_samples[n].x *= m_scale.GetX();
			(*it).m_samples[n].y *= m_scale.GetY();
			(*it).m_samples[n].z *= m_scale.GetZ();
			(*it).m_samples[n].d *= m_scale.GetW();
		}

		for(Branch3D::Markers::iterator mit=(*it).m_markers.begin();mit!=(*it).m_markers.end();++mit)
		{
			for( u32 n=0;n<(*mit).second.size();++n)
			{
				MarkerPoint& pt = (*mit).second[n];
				pt.x *= m_scale.GetX();
				pt.y *= m_scale.GetY();
				pt.z *= m_scale.GetZ();
				pt.d *= m_scale.GetW();
			}
		}


		for( u32 n=0;n<(*it).m_spines.size();++n)
		{
			(*it).m_spines[n].x *= m_scale.GetX();
			(*it).m_spines[n].y *= m_scale.GetY();
			(*it).m_spines[n].z *= m_scale.GetZ();
			(*it).m_spines[n].d *= m_scale.GetW();
		}
	}
};

struct SampleRotator
{
	m44f mTranslationform;

	SampleRotator(v4f dirAng)
	{
		mTranslationform = m44f(dirAng, dirAng.GetW());
	}

	template<typename T>
	void operator()(T it)
	{
		for( u32 n=0;n<(*it).m_samples.size();++n)
		{
			v4f curPos((*it).m_samples[n].x, (*it).m_samples[n].y, (*it).m_samples[n].z, (*it).m_samples[n].d);
			v4f newPos = mTranslationform * curPos;
			(*it).m_samples[n].x = newPos.GetX();
			(*it).m_samples[n].y = newPos.GetY();
			(*it).m_samples[n].z = newPos.GetZ();
		}


		for(Branch3D::Markers::iterator mit=(*it).m_markers.begin();mit!=(*it).m_markers.end();++mit)
		{
			for( u32 n=0;n<(*mit).second.size();++n)
			{
				MarkerPoint& pt = (*mit).second[n];
				v4f curPos(pt.x, pt.y, pt.z, pt.d);
				v4f newPos = mTranslationform * curPos;
				pt.x = newPos.GetX();
				pt.y = newPos.GetY();
				pt.z = newPos.GetZ();
			}
		}

		for( u32 n=0;n<(*it).m_spines.size();++n)
		{
			v4f curPos((*it).m_spines[n].x, (*it).m_spines[n].y, (*it).m_spines[n].z, (*it).m_spines[n].d);
			v4f newPos = mTranslationform * curPos;
			(*it).m_spines[n].x = newPos.GetX();
			(*it).m_spines[n].y = newPos.GetY();
			(*it).m_spines[n].z = newPos.GetZ();
		}

	}
};

struct MarkerDeleter
{
	MarkerDeleter()
	{
	}

	template<typename T>
	void operator()(T it)
	{
		(*it).m_markers.clear();
	}
};

struct SpineDeleter
{
	SpineDeleter()
	{
	}

	template<typename T>
	void operator()(T it)
	{
		(*it).m_spines.clear();
	}
};

void Neuron3D::EnhanceSingleSampleTrees()
{
	ExpandSingleSampleTree expander;
	DoForAllAxons(expander);
	DoForAllDendrites(expander);
}

void Neuron3D::AddMarker(const String& name, const MarkerPoint& marker)
{
	m_globalData.m_markers[name].push_back(marker);
}

void Neuron3D::ClearAllMarkerData()
{
	ClearGlobalMarkerData();

	// per-element markers
	MarkerDeleter deleter;
	DoForAllSomas(deleter);
	DoForAllTreeElements(deleter);
}

void Neuron3D::ClearGlobalMarkerData()
{
	// global markers
	m_globalData.m_markers.clear();
}

void Neuron3D::ClearSomaMarkerData()
{
	MarkerDeleter deleter;
	DoForAllSomas(deleter);
}

void Neuron3D::ClearAxonMarkerData()
{
	MarkerDeleter deleter;
	DoForAllAxonElements(deleter);
}

void Neuron3D::ClearDendriteMarkerData()
{
	MarkerDeleter deleter;
	DoForAllDendriteElements(deleter);
}


void Neuron3D::AddSpine(const SpinePoint& spine)
{
	m_globalData.m_spines.push_back(spine);
}

void Neuron3D::ClearAllSpineData()
{
	ClearGlobalSpineData();

	SpineDeleter deleter;
	DoForAllTreeElements(deleter);
}

void Neuron3D::ClearGlobalSpineData()
{
	m_globalData.m_spines.clear();
}

void Neuron3D::ClearDendriteSpineData()
{
	SpineDeleter deleter;
	DoForAllDendriteElements(deleter);
}

void Neuron3D::ClearAxonSpineData()
{
	SpineDeleter deleter;
	DoForAllAxonElements(deleter);
}

void Neuron3D::Rotate(v4f direction_angle)
{
	SampleRotator rotator(direction_angle);

	rotator(&m_globalData);

	DoForAllSomas(rotator);
	DoForAllTreeElements(rotator);
}

void Neuron3D::Translate(v4f translation)
{
	SampleTranslator translator(translation);

	translator(&m_globalData);
	
	DoForAllSomas(translator);
	DoForAllTreeElements(translator);
}

void Neuron3D::Scale(v4f scale)
{
	SampleScaler scaler(scale);

	scaler(&m_globalData);
	
	DoForAllSomas(scaler);
	DoForAllTreeElements(scaler);
}

v4f Neuron3D::CalculateSomaCentre() const
{
	v4f accumulatedPosition(0.0f,0.0f,0.0f,0.0f);
	u32 numPositions = 0;

	Neuron3D::SomaConstIterator sit = SomaBegin();
	Neuron3D::SomaConstIterator send = SomaEnd();
	for(;sit!=send;++sit)
	{
		for(u32 n=0;n<sit->m_samples.size();++n)
		{
			accumulatedPosition += v4f(sit->m_samples[n].x, sit->m_samples[n].y, sit->m_samples[n].z, sit->m_samples[n].d);
			++numPositions;
		}
	}

	if(numPositions == 0)
	{
		Neuron3D::AxonConstIterator ait = AxonBegin();
		Neuron3D::AxonConstIterator aend = AxonEnd();
		for(;ait!=aend;++ait)
		{
			const SamplePoint pt = ait->root()->m_samples[0];
			accumulatedPosition += v4f(pt.x, pt.y, pt.z, pt.d);
			++numPositions;
		}

		Neuron3D::DendriteConstIterator dit = DendriteBegin();
		Neuron3D::DendriteConstIterator dend = DendriteEnd();
		for(;dit!=dend;++dit)
		{
			const SamplePoint pt = dit->root()->m_samples[0];
			accumulatedPosition += v4f(pt.x, pt.y, pt.z, pt.d);
			++numPositions;
		}
	}

	if(numPositions == 0)
		return v4f(0,0,0,0);
	else
		return accumulatedPosition/ static_cast<float>( numPositions );
}


struct Marker2SpineConverter
{
	bool all;
	const std::vector<String>* names;

	Marker2SpineConverter() : all(true), names(0) {}
	Marker2SpineConverter(const std::vector<String>* markerNames) : all(false), names(markerNames) {}

	template<typename T>
	void operator()(T pit)
	{
		for(Branch3D::Markers::iterator it = pit->m_markers.begin();it!=pit->m_markers.end();)
		{
			Branch3D::MarkerSet& markerset = it->second;

			if(all || (names && find(names->begin(), names->end(), it->first) != names->end()) )
			{
				for(u32 n=0;n<markerset.size();++n)
				{
					const MarkerPoint& m = markerset[n];
					u32 associatedSegment = pit->CalculateNearestSegment(m).first;
					SpinePoint p(m, kSpineNoDetail, associatedSegment);
					pit->m_spines.push_back(p);
				}

				it = pit->m_markers.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
};

struct Spines2MarkersConverter
{
	const String& spinename;

	Spines2MarkersConverter(const String& name) : spinename(name) {}

	template<typename T>
	void operator()(T it)
	{
		for(u32 n=0;n<it->m_spines.size();++n)
		{
			const SpinePoint& s = it->m_spines[n];
			MarkerPoint m(s.x, s.y, s.z, s.d);
			it->m_markers[spinename].push_back(m);
		}

		it->m_spines.clear();
	}
};


void Neuron3D::ConvertAllMarkersToSpines()
{
	Marker2SpineConverter spiner;

	spiner(&m_globalData);

	DoForAllTreeElements(spiner);
}

void Neuron3D::ConvertNamedMarkersToSpines(const std::vector<String>& markerNames)
{
	Marker2SpineConverter spiner(&markerNames);

	spiner(&m_globalData);

	DoForAllTreeElements(spiner);
}

void Neuron3D::ConvertAllSpinesToMarkers(const String& name)
{
	Spines2MarkersConverter markifier(name);

	markifier(&m_globalData);

	DoForAllTreeElements(markifier);
}

struct SampleLookup
{
	SamplePoint pt;
	float closestDistance;
	std::pair<Neuron3D::DendriteTreeIterator, u32> result;

	SampleLookup(SamplePoint _pt, float _limit) : pt(_pt), closestDistance(_limit) 
	{
		result.first = Neuron3D::DendriteTree::null();
		result.second = 0;
	}

	template<typename T>
	void operator()(T it)
	{
		std::pair<u32, float> branchResult = it->CalculateNearestSegment(pt);

		if(branchResult.second < closestDistance)
		{
			closestDistance = branchResult.second;
			result.second = branchResult.first;
			result.first = it;
		}
	}

};

std::pair<Neuron3D::DendriteTreeIterator, u32> Neuron3D::FindNearestBranchSamplePoint(SamplePoint pt, float limit)
{
	SampleLookup lookup(pt, limit);

	DoForAllTreeElements( lookup );

	return lookup.result;
}

void Neuron3D::MapGlobalSpinesToBranches(float limit)
{
	for(std::vector<SpinePoint>::iterator sit = m_globalData.m_spines.begin();sit != m_globalData.m_spines.end();)
	{
		std::pair<Neuron3D::DendriteTreeIterator, u32> result = 
			FindNearestBranchSamplePoint(*sit, limit);

		if(result.first)
		{
			SpinePoint pt = *sit;
			pt.associatedSegment = result.second;
			result.first->m_spines.push_back(pt);

			sit = m_globalData.m_spines.erase(sit);
		}
		else
			++sit;
	}
}

void Neuron3D::MapGlobalMarkersToBranches(float limit)
{
	for(Branch3D::Markers::iterator mit = m_globalData.m_markers.begin();mit!=m_globalData.m_markers.end();)
	{
		Branch3D::MarkerSet& markerset = mit->second;

		for(Branch3D::MarkerSet::iterator msit=markerset.begin();msit!=markerset.end();/**/)
		{
			std::pair<Neuron3D::DendriteTreeIterator, u32> result = 
				FindNearestBranchSamplePoint( *msit, limit );

			if(result.first)
			{
				result.first->m_markers[mit->first].push_back(*msit);
				msit = markerset.erase(msit);
			}
			else
				++msit;
		}

		if(mit->second.size() == 0)
		{
			mit = m_globalData.m_markers.erase(mit);
		}
		else
		{
			++mit;
		}
	}
}


struct SpineGlobalizer
{
	Neuron3D& nrn;
	SpineGlobalizer(Neuron3D& n) : nrn(n) {}

	template<typename T>
	void operator()(T it)
	{
		for(u32 n=0;n<it->m_spines.size();++n)
		{
			SpinePoint s = it->m_spines[n];
			s.associatedSegment = 0;
			nrn.AddSpine(s);
		}

		it->m_spines.clear();
	}
};

struct MarkerGlobalizer
{
	Neuron3D& nrn;
	MarkerGlobalizer(Neuron3D& n) : nrn(n) {}

	template<typename T>
	void operator()(T pit)
	{
		for(Branch3D::Markers::iterator it = pit->m_markers.begin();it!=pit->m_markers.end();++it)
		{
			Branch3D::MarkerSet& markerset = it->second;

			for(u32 n=0;n<markerset.size();++n)
				nrn.AddMarker(it->first, markerset[n]);
		}

		pit->m_markers.clear();
	}
};

void Neuron3D::MakeAllSpinesGlobal()
{
	SpineGlobalizer spineglober(*this);

	DoForAllTreeElements(spineglober);
}

void Neuron3D::MakeAllMarkersGlobal()
{
	MarkerGlobalizer markerglober(*this);

	DoForAllSomas(markerglober);
	DoForAllTreeElements(markerglober);
}


struct SpineBranchifier
{
	void operator()(Neuron3D::DendriteTreeIterator it)
	{
		// copy of spines - easier than working with the original, as we are going to be branch splitting..
		Branch3D::Spines spineList = it->m_spines;

		// Note: assume spines are ascending order. (MorphologyBuilder does guarantee this... but should be more careful..)

		// clear the original
		it->m_spines.clear();

		// create new branches for the spines, gradually splitting
		Neuron3D::DendriteTreeIterator current = it;
		u32 spineOffset = 0;
		for(u32 n=0;n<spineList.size();++n)
		{
			SpinePoint& spine = spineList[n];

			// special case... To be reorganized
			if(spine.type == kSpineContourRoot)
				continue;
			// 

			const u32 associatedSegment = spine.associatedSegment;

			const u32 localOffset = associatedSegment - spineOffset;

			if(localOffset == 0)
			{
				if(current.parent())
				{
					Neuron3D::DendriteTreeIterator spineBran = Neuron3D::DendriteTree::insert_child_last(current.parent(), Neuron3D::DendriteElement() );

					(*spineBran).m_samples.push_back( (*current).m_samples[(*current).m_samples.size()-1] );
					(*spineBran).m_samples.push_back( (const SamplePoint&)spine );
				}
				else
				{
					// make spine a new dendritic tree! :/
					// for now..we just lose these spines..!! not good..
					assert(0);
				}
			}
			else
			{
				// BAD: code duplication with MorphologyBuilder::Split()... do something about that! 

				Neuron3D::DendriteTreeIterator lowerBran = current;
				Neuron3D::DendriteTreeIterator upperBran = Neuron3D::DendriteTree::insert(current, Neuron3D::DendriteElement());

				std::vector<SamplePoint>::iterator samp_it = (*lowerBran).m_samples.begin() + localOffset;

				// copy upper elements
				(*upperBran).m_samples.insert( (*upperBran).m_samples.end(), samp_it, (*lowerBran).m_samples.end() );

				// erase them from lower
				(*lowerBran).m_samples.erase(samp_it+1, (*lowerBran).m_samples.end());

				
				Neuron3D::DendriteTreeIterator spineBran = Neuron3D::DendriteTree::insert_child_last(lowerBran, Neuron3D::DendriteElement());

				(*spineBran).m_samples.push_back( (*lowerBran).m_samples[(*lowerBran).m_samples.size()-1] );
				(*spineBran).m_samples.push_back( (const SamplePoint&)spine );

				current = upperBran;
				spineOffset = associatedSegment;
			}
		}
	}
};

void Neuron3D::ConvertTreeSpinesToBranches()
{
	SpineBranchifier branchifier;

	// no...needs to be reversed..

	for(DendriteConstIterator it = DendriteBegin();it != DendriteEnd();++it)
		DendriteTree::recurse_reverse( (*it).root(), branchifier);

	for(AxonConstIterator it = AxonBegin();it != AxonEnd();++it)
		AxonTree::recurse_reverse( (*it).root(), branchifier);


	//DoForAllTreeElements(branchifier);
}


struct MarkerRenamer
{
	String oldname;
	String newname;

	MarkerRenamer(const String& prev, const String& now) : oldname(prev), newname(now) {}

	template<typename T>
	void operator()(T pit)
	{
		for(Branch3D::Markers::iterator it = pit->m_markers.begin();it!=pit->m_markers.end();++it)
		{
			Branch3D::MarkerSet& markerset = it->second;
			if(it->first == oldname)
			{
				pit->m_markers[newname] = markerset;
				pit->m_markers.erase(oldname);
				break;
			}
		}
	}
};

void Neuron3D::RenameMarkers(const std::map<String,String>& rename)
{
	for(std::map<String, String>::const_iterator it=rename.begin();it!=rename.end();++it)
	{
		MarkerRenamer renamer(it->first, it->second);

		renamer(&m_globalData);
		DoForAllSomas(renamer);
		DoForAllTreeElements(renamer);
	}
}


struct TerminalRetyper
{
	TerminalType mType;
	TerminalRetyper(TerminalType type) : mType(type) {}

	void operator()(Neuron3D::DendriteTreeIterator it)
	{
		if(! it.child() )
			it->SetTerminalType(mType);
	}
};

void Neuron3D::SetGlobalTreeTerminalType(TerminalType type)
{
	TerminalRetyper retyper(type);

	DoForAllTreeElements(retyper);
}


static bool NotEqualBranchRecursive(Neuron3D::DendriteTreeConstIterator itLhs, Neuron3D::DendriteTreeConstIterator itRhs)
{
	if( *itLhs != *itRhs )
		return true;

	// check if connectivity structure is identical
	if(itLhs.child() && itRhs.child())
	{
		if( NotEqualBranchRecursive(itLhs.child(), itRhs.child()) )
			return true;
	}
	else if(itLhs.child() != itRhs.child())
		return true;

	if(itLhs.peer() && itRhs.peer())
	{
		if( NotEqualBranchRecursive(itLhs.peer(), itRhs.peer()) )
			return true;
	}
	else if(itLhs.peer() != itRhs.peer())
		return true;

	return false;
}


bool operator!= (const Neuron3D& nrnLhs, const Neuron3D& nrnRhs)
{
	// global data
	if( nrnLhs.m_globalData != nrnRhs.m_globalData)
		return true;

	// soma
	if(nrnLhs.CountSomas() != nrnRhs.CountSomas())
		return true;

	for(Neuron3D::SomaConstIterator l=nrnLhs.SomaBegin(), r=nrnRhs.SomaBegin();l!=nrnLhs.SomaEnd();++l, ++r)
	{
		if( (*l) != (*r) )
			return true;
	}

	// trees
	if(	nrnLhs.CountAxons() != nrnRhs.CountAxons() ||
		nrnLhs.CountDendrites() != nrnRhs.CountDendrites())
		return true;

	for(Neuron3D::AxonConstIterator l=nrnLhs.AxonBegin(), r=nrnRhs.AxonBegin();l!=nrnLhs.AxonEnd();++l, ++r)
	{
		if( NotEqualBranchRecursive( (*l).root(), (*r).root() ) )
			return true;
	}

	for(Neuron3D::DendriteConstIterator l=nrnLhs.DendriteBegin(), r =nrnRhs.DendriteBegin();l!=nrnLhs.DendriteEnd();++l, ++r)
	{
		if( NotEqualBranchRecursive( (*l).root(), (*r).root() ) )
			return true;
	}

	return false;
}


/** 
	TODO - move into cptree..
**/
void Neuron3D::ReverseChildOrderSubTree(Neuron3D::DendriteTreeIterator it)
{
	if(it.child())
		ReverseChildOrderSubTree(it.child());

	if(it.peer())
		ReverseChildOrderSubTree(it.peer());

	if(it.child())
	{
		Neuron3D::DendriteTree::reverse_child_order(it);
	}
}

void Neuron3D::ReverseChildOrderAllTrees()
{
	for(Neuron3D::AxonIterator a = AxonBegin(); a!=AxonEnd(); ++a)
		ReverseChildOrderSubTree( (*a).root() );

	for(Neuron3D::DendriteIterator d = DendriteBegin(); d!=DendriteEnd(); ++d)
		ReverseChildOrderSubTree( (*d).root() );
}

/**
	Artificial branching..
*/

float DegreesToRadians(float degrees)
{
	return (degrees * 3.1415926f) / 180.0f;
}
void CalculateAngleStartAndIncrement(float centerAngle, float angleRange, float numChildren, float& startAngle, float& angleIncrement)
{
	if(numChildren > 1)
	{
		startAngle = centerAngle + (angleRange/2.0f);
		angleIncrement = angleRange / (float)(numChildren-1);
	}
	else
	{
		startAngle = centerAngle;
		angleIncrement = 0.0f;
	}
}

void Neuron3D::ApplyArtificialBranchingSubTree(Neuron3D::DendriteTreeIterator it, const SamplePoint startPoint, float startAngle, float angleIncrement, float childRangeAngle)
{
	const u32 numSamples = (*it).m_samples.size();
	assert( numSamples > 1);
	(*it).m_samples[0].x = startPoint.x;
	(*it).m_samples[0].y = startPoint.y;
	(*it).m_samples[0].z = startPoint.z;
	// diam ?

	const float cosStartAngle = cosf(startAngle);
	const float sinStartAngle = sinf(startAngle);

	// fix up all samples..
	for(u32 n=1;n<numSamples;++n)
	{
		const float length = (*it).m_samples[n].x;

		float dX = length * cosStartAngle;
		float dY = length * sinStartAngle;
		float dZ = 0.0f;

		(*it).m_samples[n].x = (*it).m_samples[n-1].x + dX;
		(*it).m_samples[n].y = (*it).m_samples[n-1].y + dY;
		(*it).m_samples[n].z = (*it).m_samples[n-1].z + dZ;
		// diam ?
	}

	if(it.child())
	{
		const SamplePoint endPoint = (*it).m_samples[numSamples-1];

		const u32 numChildren = Neuron3D::DendriteTree::countchildren(it);

		float childStartAngle;
		float childAngleIncrement;

		CalculateAngleStartAndIncrement(startAngle, childRangeAngle, numChildren, childStartAngle, childAngleIncrement);

		ApplyArtificialBranchingSubTree(it.child(), endPoint, childStartAngle, childAngleIncrement, childRangeAngle);
	}

	if(it.peer())
		ApplyArtificialBranchingSubTree(it.peer(), startPoint, startAngle - angleIncrement, angleIncrement, childRangeAngle);
}


void Neuron3D::ApplyArtificialBranching(const ArtificialBranchingConfiguration& config)
{
	const SamplePoint somaPoint = (SomaBegin() != SomaEnd()) ? SomaBegin()->m_samples[0] : SamplePoint(0.0f,0.0f,0.0f,0.0f);
	const float somaRadius = somaPoint.d/2.0f;

	u32 numApicalDendrites = 0;
	u32 numNonApicalDendrites = 0;

	for(Neuron3D::DendriteIterator d = DendriteBegin(); d!=DendriteEnd(); ++d)
	{
		if( (*d).GetTreeType() == Neuron3D::DendriteTree::Apical)
			++numApicalDendrites;
		else
			++numNonApicalDendrites;
	}

	// non-apical
	{
		float startAngle;
		float angleIncrement;

		CalculateAngleStartAndIncrement(DegreesToRadians(config.rootCenterAngle), DegreesToRadians(config.rootRangeAngle), numNonApicalDendrites, startAngle, angleIncrement);

		for(Neuron3D::DendriteIterator d = DendriteBegin(); d!=DendriteEnd(); ++d)
		{
			if( (*d).GetTreeType() == Neuron3D::DendriteTree::Apical)
				continue;

			SamplePoint startPoint;
			startPoint.x = somaRadius * cosf(startAngle);
			startPoint.y = somaRadius * sinf(startAngle);
			startPoint.z = 0.0f;
			startPoint.d = 0.0f; // not used

			ApplyArtificialBranchingSubTree( (*d).root(), startPoint, startAngle, angleIncrement, DegreesToRadians(config.childBranchRangeAngle) );

			startAngle -= angleIncrement;
		}
	}

	// apical
	{
		float startAngle;
		float angleIncrement;

		CalculateAngleStartAndIncrement(DegreesToRadians(config.rootCenterAngleApical), DegreesToRadians(config.rootRangeAngleApical), numNonApicalDendrites, startAngle, angleIncrement);

		for(Neuron3D::DendriteIterator d = DendriteBegin(); d!=DendriteEnd(); ++d)
		{
			if( (*d).GetTreeType() != Neuron3D::DendriteTree::Apical)
				continue;

			SamplePoint startPoint;
			startPoint.x = somaRadius * cosf(startAngle);
			startPoint.y = somaRadius * sinf(startAngle);
			startPoint.z = 0.0f;
			startPoint.d = 0.0f; // not used

			ApplyArtificialBranchingSubTree( (*d).root(), startPoint, startAngle, angleIncrement, DegreesToRadians(config.childBranchRangeAngleApical) );
			startAngle -= angleIncrement;
		}
	}

	//axons
	{
		float startAngle;
		float angleIncrement;

		CalculateAngleStartAndIncrement(DegreesToRadians(config.rootCenterAngleAxon), DegreesToRadians(config.rootRangeAngleAxon), CountAxons(), startAngle, angleIncrement);

		for(Neuron3D::AxonIterator a = AxonBegin(); a!=AxonEnd(); ++a, startAngle -= angleIncrement)
		{
			SamplePoint startPoint;
			startPoint.x = somaRadius * cosf(startAngle);
			startPoint.y = somaRadius * sinf(startAngle);
			startPoint.z = 0.0f;
			startPoint.d = 0.0f; // not used

			ApplyArtificialBranchingSubTree( (*a).root(), startPoint, startAngle, angleIncrement, DegreesToRadians(config.childBranchRangeAngleAxon) );
		}
	}
}
