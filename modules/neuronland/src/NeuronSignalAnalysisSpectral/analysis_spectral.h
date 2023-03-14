#ifndef __ANALYSIS__H__TWIG__
#define __ANALYSIS__H__TWIG__

#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>

#include <list>


#define STANDARD  10
#define TIME_WINDOWED 11

#define ERR_DIFF_FILE_NUM 100	// different number of data files associated with each channel
#define ERR_NO_FILES 101		// no data files associated with a particular channel
#define ERR_FILE_BAD 102		// problem opening a file
#define ERR_TOTAL_SECS 103		// total record lengths of two channels are different
#define ERR_INCOMPAT_SAMP_RATES 104 // sampling rates of two files are incompatible
#define ERR_INCONSIS_SAMP_RATE 105 // two files for same channel have different samp rates
#define ERR_UNSUITABLE_PARAMETERS 106
#define ERR_DATA_SHORT_FOR_SEGMENT 107
#define ERR_INCOMPAT_FREQ_VALS 108
#define ERR_INCOMPAT_NUM_PTS 109
#define ERR_INCOMPAT_SEG_INTERVAL 110
#define ERR_UNDETERMINED_ELECTRODE 111

#define RECT_FULLWAVE 0
#define RECT_HALF_TOP 1
#define RECT_HALF_BOT 2


#define new DEBUG_NEW

namespace  Analysis {


	class anFileInfo { // input data file info
		public:

			int secs; // analogue data file specific info
			int rate;
			int segnum;

			unsigned long numPts; // number of data points, or number of spike times

			float maxVal; // range of y values
			float minVal;

			float *data; // storage for data values
			float *dataX; // x-values when specified
			float *nuConfLim; // array for non-uniform confidence limit data (phase)
			long *spikes; // array of soike times (msecs)

			float lineUpper[4]; // storage for 3 lines on graph
			float lineMiddle[4];
			float lineLower[4];

			anFileInfo();
			~anFileInfo();
			void clear();
	};


	class resInfo {
		public:
		
			operator int() const { return type; };

			CString pathname;
			CString filename;
			long type;
	};

	class phaseBFInfo{
	public:
		int start;
		int stop;
		float delay;
		float del_c95;

		float x0;
		float y0; 
		float x1;
		float y1;
	};
	//typedef std::list<resInfo> resList;
	//typedef resList::const_iterator resCI;
	//typedef resList::iterator resI;
	

	typedef std::list<CString> strList;
	typedef strList::const_iterator strCI;
	typedef strList::iterator strI;


	bool an_load_data(LPCSTR filename, long fileType, anFileInfo &info);

	int DoAnalysisStandard(
		strList &fileList1, strList &fileList2, int elecType1, int elecType2,
		strList &refFileList,
		CString outputPathName, CString outputNameStem, 
		std::list<resInfo> &outputFileInfoList,
		CString elec1name, CString elec2name,
		bool doSpectrum1, bool doSpectrum2, bool doCumulant, bool doCoherence, bool doPhase,
		bool hanning, bool inversion, bool notch,
		bool rectify1, bool rectify2, int rectType, bool useRef1, bool useRef2, 
		int fixedSampRate, bool trendRemove1, bool trendRemove2,
		int freqOut, int segSizePwr, int lagTot, int lagNeg, 
		int smPts1, int smPts2, float sdScale1, float sdScale2 );


	int DoAnalysisCompCoh(const CString &path1, const CString &path2, 
								const CString &outputFilePath, resInfo &res);

	bool AnalysisPhaseBestFitLine(const CString &phzFile, const CString &cohFile,
				int start, int stop, phaseBFInfo &bfInfo);




////////// DAVID's ROUTINES ////////////

// modified for C++ iostreams instead of C i/o
// also, memory allocation now uses new/delete

int an_in_f_d(float **an_dat,int an_pts,ifstream &is, float *ref_data, int an_fac,int sm_pts);

int pp_ft(int *spike_times,int spike_tot,int *spike_start,
            int samp_start,int samp_stop,int seg_size,
            float *dt_r,float *dt_i,int fn_flag);
void ts_ft(float *an_dat,int an_pts,int seg_size,
            float *dt_r,float *dt_i,int fn_flag);
void ts_trend(float *an_dat,int an_pts,int trend_flag);
void rvfft(float *x,int n);
void irvfft(float *x,int n);
void fit(float *y,int ndata,float *a,float *b);
void an_sdscale(float *data,int pts_tot,float sd_new);
void an_rect_a(float *data,int pts_tot,int rect_type);
void han(float *dat,int dat_pts);
void moment(float *data,int pts_tot,float *ave,float *sdev);
void q12_ft(float *f_r,float *f_i,int seg_size,int seg_size_2,
      float *cov,float norm_fac);
float q12_var(float *f11, float *f22,int seg_size,int seg_size_2,long samp_tot);

void an_interpolate(float *an_dat,long an_samp_tot,float **an_dat_1,
          long *an_samp_tot_1,int an_int_tot);
void sm_av_ncls_a(float *dat_in,float *dat_out,int pts_tot,
      int sm_pts,float sm_fac);
float rmagn2(float re,float im);
float frba(float ar,float ai,float br,float bi);
float fiba(float ar,float ai,float br,float bi);
float cab(float ab2,float aa,float bb);

};



struct analysisOptions
{
	analysisOptions();

	bool	m_allElec;
	bool	m_doCoherence;
	bool	m_doCumulant;
	bool	m_doLogPSpec1;
	bool	m_doLogPSpec2;
	bool	m_doPhase;
	bool	m_useRef1;
	bool	m_useRef2;
	bool	m_fixedSamp;
	bool	m_freqOut;
	bool	m_hanning;
	bool	m_lagNeg;
	bool	m_lagTot;
	bool	m_rectify1;
	bool	m_rectify2;
	bool	m_SD1;
	bool	m_SD2;
	bool	m_segSizePwr;
	bool	m_trend1;
	bool	m_trend2;
	CString	m_outputFileStem;
	CString	m_outputPathName;
	UINT	m_fixedSampVal;
	UINT	m_freqOutVal;
	UINT	m_lagNegVal;
	UINT	m_lagTotVal;
	float	m_SD1Val;
	float	m_SD2Val;
	UINT	m_trend1Val;
	UINT	m_trend2Val;
	UINT	m_segSizePwrVal;
	bool	m_notch;
	int m_rectType;

};
	
	

using namespace Analysis;


#endif //__ANALYSIS__H__TWIG__
