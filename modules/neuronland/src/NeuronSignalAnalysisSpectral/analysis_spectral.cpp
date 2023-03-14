#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nr_1dmem.h"

#include "analysis.h"
#include "filetypes.h"


#define pi        3.1415926
#define freq_default     75
#define lag_default     250
#define seg_pwr_default  10
#define seg_pwr_low       6
#define seg_pwr_high     16
#define sm_pts_min       11

#define sampPP			1000






analysisOptions::analysisOptions() :
m_allElec(FALSE), m_doCoherence(TRUE), m_doCumulant(TRUE), m_doLogPSpec1(TRUE), m_doLogPSpec2(TRUE),
m_doPhase(TRUE), m_useRef1(FALSE), m_useRef2(FALSE), m_fixedSamp(FALSE),  m_freqOut(FALSE),m_hanning(FALSE),
m_lagNeg(FALSE), m_lagTot(FALSE), m_rectify1(FALSE), m_rectify2(FALSE), m_SD1(FALSE),
m_SD2(FALSE), m_segSizePwr(FALSE), m_trend1(FALSE), m_trend2(FALSE), 
m_outputFileStem(CString("")), m_outputPathName(CString("")), m_fixedSampVal(0),
m_freqOutVal(0), m_lagNegVal(0), m_lagTotVal(0), m_SD1Val(0.0), m_SD2Val(0.0), 
m_trend1Val(0), m_trend2Val(0), m_segSizePwrVal(0), m_notch(TRUE), m_rectType(-1)
{
}

/////////

Analysis::anFileInfo::anFileInfo() : rate(0), secs(0), segnum(0), numPts(0), 
						data(NULL), dataX(NULL), spikes(NULL), nuConfLim(NULL)
{
}


Analysis::anFileInfo::~anFileInfo()
{
	clear();
}

void Analysis::anFileInfo::clear()
{
	
	rate = 0;
	secs = 0;
	numPts = 0;
	segnum = 0;
	if(data != NULL) delete [] data;
	data = NULL;
	if(dataX != NULL) delete [] dataX;
	dataX = NULL;
	if(spikes != NULL) delete [] spikes;
	spikes = NULL;
	if(nuConfLim != NULL) delete [] nuConfLim;
	nuConfLim = NULL;
}



///////////

double invtanh(double val)
{
	return -0.5 * log( (1-val)/(1+val) );
}


bool Analysis::an_load_data(LPCSTR filename, long fileType, anFileInfo &info)
{

	ifstream is;
	int i;
	char temp[32];
	int chanNum;

	switch(fileType){

	case FILE_DATA:

		float deltat;

		is.open(filename, ios::binary | ios::in | ios::nocreate, filebuf::sh_none);
		if(is.bad()) return FALSE;
		is.read( (char *) &(info.secs), sizeof(int));
		is.read( (char *) &(info.rate), sizeof(int));


		TRACE("File record length : %d secs.  Sample rate : %d per sec\n", 
															info.secs, info.rate);


		if(info.secs < 1 || info.secs > 100000){
			TRACE("This is almost certainly not a suitable data file\n");
			is.close();
			return FALSE;
		}

		deltat=1000.0/info.rate; // rate is no. per sec,  deltat is in ms, 

		info.numPts = ((unsigned long)info.secs) * ((unsigned long)info.rate);

		TRACE("Number of data samples : %d\n", info.numPts);

		an_in_f_d(&(info.data), info.numPts, is, NULL, 1.0, 0);

		is.close();

		break; //FILE_DATA

	case FILE_DATA_SPIKE:


		is.open(filename, ios::in | ios::nocreate, filebuf::sh_none);
		if(is.bad()) return FALSE;

		is >> chanNum;
		is >> info.secs;
		is >> info.rate;
		is >> info.numPts;
		
		info.spikes = new long[info.numPts];
		
		for(i=0;i<info.numPts;i++) is >> info.spikes[i];
			 
		break;

	case FILE_RES_SPECTRA:

		
	
		is.open(filename,  ios::in | ios::nocreate, filebuf::sh_none);
		if(is.bad()) return FALSE;

		is >> temp;
		is >> info.rate;
		is >> info.secs;
		is >> info.numPts;

		
		TRACE("Verify file type : %s\n", temp);
		TRACE("Number of data points  - %d\n", info.numPts);

		info.data = new float[info.numPts];
		info.dataX = new float[info.numPts];

		//load in all spectra data points
		for(i=0; i<info.numPts;i++){
			is >> info.dataX[i] >> info.data[i];
		}

		is >> info.lineMiddle[0] >> info.lineMiddle[1] >> info.lineMiddle[2] >> info.lineMiddle[3];

		is.close();

		break;

	case FILE_RES_COHERENCE:

		
		is.open(filename,  ios::in | ios::nocreate, filebuf::sh_none);
		if(is.bad()) return FALSE;

		is >> temp;
		is >> info.rate;
		is >> info.secs;
		is >> info.segnum;
		is >> info.numPts;

		TRACE("Verify file type : %s\n", temp);
		TRACE("Number of data points  - %d\n", info.numPts);

		info.data = new float[info.numPts];
		info.dataX = new float[info.numPts];

		//load in all coherence data points
		for(i=0; i<info.numPts;i++){
			is >> info.dataX[i] >> info.data[i];
		}

		// single coherence confidence line
		is >> info.lineUpper[0] >> info.lineUpper[1] >> 
					info.lineUpper[2] >> info.lineUpper[3];
		is.close();
		break;

	case FILE_RES_CUMULANT:

		is.open(filename,  ios::in | ios::nocreate, filebuf::sh_none);
		if(is.bad()) return FALSE;

		is >> temp;
		is >> info.rate;
		is >> info.secs;
		is >> info.numPts;

		TRACE("Verify file type : %s\n", temp);
		TRACE("Number of data points  - %d\n", info.numPts);

		info.data = new float[info.numPts];
		info.dataX = new float[info.numPts];

		//load in all coherence data points
		for(i=0; i<info.numPts;i++){
			is >> info.dataX[i] >> info.data[i];
		}

		// single coherence confidence line
		is >> info.lineMiddle[0] >> info.lineMiddle[1] >> 
					info.lineMiddle[2] >> info.lineMiddle[3];
		is >> info.lineUpper[0] >> info.lineUpper[1] >> 
					info.lineUpper[2] >> info.lineUpper[3];
		is >> info.lineLower[0] >> info.lineLower[1] >> 
					info.lineLower[2] >> info.lineLower[3];
		
		is.close();

		break;

	case FILE_RES_PHASE:

		is.open(filename,  ios::in | ios::nocreate, filebuf::sh_none);
		if(is.bad()) return FALSE;

		is >> temp;
		is >> info.rate;
		is >> info.secs;
		is >> info.numPts;

		info.data = new float[info.numPts];
		info.dataX = new float[info.numPts];

		TRACE("Verify file type : %s\n", temp);
		TRACE("Number of data points  - %d\n", info.numPts);

		//load in all coherence data points
		for(i=0; i<info.numPts;i++){
			is >> info.dataX[i] >> info.data[i];
		}

		// single coherence confidence line
		is >> info.lineMiddle[0] >> info.lineMiddle[1] >> 
					info.lineMiddle[2] >> info.lineMiddle[3];

		
// non uniform confidence limit data

		if(is.good())
		{
			info.nuConfLim = new float[info.numPts];
			char conf[32];
			is >> conf;
			if(CString("CONF") == CString(conf)) 
			{
				for(i=0; i<info.numPts;i++){
					is >> info.nuConfLim[i] >> info.nuConfLim[i];
				}
			}
		}


		is.close();

		break;
	case FILE_DIFF_COH:

		is.open(filename,  ios::in | ios::nocreate, filebuf::sh_none);
		if(is.bad()) return FALSE;

		is >> temp;
		is >> info.rate;
		is >> info.secs;
		is >> info.segnum;
		is >> info.numPts;

		info.data = new float[info.numPts];
		info.dataX = new float[info.numPts];

		TRACE("Verify file type : %s\n", temp);
		TRACE("Number of data points  - %d\n", info.numPts);

		//load in all data points
		for(i=0; i<info.numPts;i++){
			is >> info.dataX[i] >> info.data[i];
		}

		// confidence lines
		is >> info.lineUpper[0] >> info.lineUpper[1] >> 
					info.lineUpper[2] >> info.lineUpper[3];
		is >> info.lineMiddle[0] >> info.lineMiddle[1] >> 
					info.lineMiddle[2] >> info.lineMiddle[3];
		is >> info.lineLower[0] >> info.lineLower[1] >> 
					info.lineLower[2] >> info.lineLower[3];

		is.close();

		break;
	default:
		TRACE("Can't open this file type...\n");
		return FALSE;
		break;

	}


	if(fileType != FILE_DATA_SPIKE)
	{
		for(i=0;i<info.numPts;i++){if(!(i%10000)) TRACE("%f\n", info.data[i]);}

		// get maximum data value
		float max;
		float min;

		max = min = info.data[0];

		for(i=1;i<info.numPts;i++){
			if(info.data[i] < min) min = info.data[i];
			else if(info.data[i] > max) max = info.data[i];
		}

		info.maxVal = max;
		info.minVal = min;
	}

	return TRUE;
}



//
//  Generate a best fit line for a portion of a phase graph.
//  Requires both a phase and the associated coherence file.
//
// Original function header----->
/*
 File: sp2_pdel.c, 13/09/94, DMH
 Program for estimating delay from phase plot using weighted least
 squares regression scheme.
*/

bool Analysis::AnalysisPhaseBestFitLine(const CString &phzFile, const CString &cohFile, int startVal, int stopVal, phaseBFInfo &bfInfo)
{
  int   seg_no,samp_rate;
  int   i;
  int   freq_out=freq_default;
  int   unres_flag=0,ind_beg=0,err_flag=0;
  int   i_start=0,i_stop=0,del_pts;
  int   out_fil_no=0, inv_flag=0;
  float *ch21,*ph21;
  float ph_var,deltaf,seg_fac;
  float num,den,omega,wi,del,del_var,del_c95,sigma;

  anFileInfo info_coh;
  anFileInfo info_phz;

/*--------------------------------------------------------------------------*/
/* Input phase and coherence estimates */

	if(!an_load_data(cohFile, FILE_RES_COHERENCE, info_coh)) return FALSE;
	if(!an_load_data(phzFile, FILE_RES_PHASE, info_phz)) return FALSE;

	i_start = 0;
	for(i=0;i<info_phz.numPts;i++){
		if(info_phz.dataX[i] >= startVal)
		{
			i_start = i;
			break;
		}
	}

	i_stop = 0;
	for(i=i_start;i<info_phz.numPts;i++){
		if(info_phz.dataX[i] >= stopVal)
		{
			i_stop = i;
			break;
		}
	}

	if(i_stop ==0) i_stop = info_phz.numPts-1;
	

TRACE("Found start (%d), and stop (%d) indices\n", i_start, i_stop);

  seg_no = info_coh.segnum;
  samp_rate = info_coh.rate;
  ch21=info_coh.data;
  ph21=info_phz.data;
  deltaf=info_coh.dataX[1] - info_coh.dataX[0];
  
  float deltafTest = info_phz.dataX[1] - info_phz.dataX[0];

  if( fabs(deltafTest - deltaf) > 0.001){
	  TRACE("Coherence and phase are incompatible!!\n");
	  return FALSE;
  }

  seg_fac=1.0/(float)(2*seg_no);       /* 1/2L                             */
  num=0.0;                             /*   numerator of (10.7)            */
  den=0.0;                             /* denominator of (10.7)            */
  for (i=i_start;i<=i_stop;i++) {      /* LOOP to estimate delay           */
    ph_var=seg_fac*(1.0/ch21[i]-1.0);  /* Estimate of variance (10.4)      */
    wi=1.0/ph_var;                     /* Estimate of weight w_i (10.10)   */
    omega=(float)i*deltaf*2.0*pi;      /* Frequency in rad/sec (2*pi*f_i)  */
    num+=wi*ph21[i]*omega;             /* Summation (10.7) numerator       */
    den+=wi*omega*omega;               /* Summation (10.7) denominator     */
  }                                    /* Finish LOOP                      */
  del=num/den;                         /* estimate of delay (10.7), Beta   */

  sigma=0.0;                           /* Estimate of sigma (10.9)         */
  den=0.0;                             /* denominator of (10.8)            */
  del_pts=i_stop-i_start+1;            /* Number of points, n              */
  for (i=i_start;i<=i_stop;i++) {      /* LOOP to estimate variance        */
    ph_var=seg_fac*(1.0/ch21[i]-1.0);  /* Estimate of variance (10.4)      */
    wi=1.0/ph_var;                     /* Estimate of weight w_i (10.10)   */
    omega=(float)i*deltaf*2.0*pi;      /* Frequency in rad/sec (2*pi*f)    */
    sigma+=wi*(ph21[i]-del*omega)*(ph21[i]-del*omega); /* Summation (10.9) */
    den+=wi*omega*omega;               /* Summation, denominator (10.8)    */
  }                                    /* Finish LOOP                      */
  sigma/=(float)(del_pts-1);           /* Complete estimate of (10.9)      */
  del_var=sigma/den;                   /* Estimate of variance (10.8)      */
  del_c95=1.96*sqrt(del_var);          /* Estimate 95% confidence limit    */

  TRACE("\nPts: %d   Delay: %.2f msec +/- %.2f msec\n",del_pts,
          -del*1000.0,del_c95*1000.0);


// fill in bf line info structure
  bfInfo.start = i_start;
  bfInfo.stop = i_stop;
  bfInfo.delay = del*1000;
  bfInfo.del_c95 = del_c95*1000;
  bfInfo.x0 = (float)i_start*deltaf;
  bfInfo.y0 = 2.0*pi*(float)i_start*deltaf*del;
  bfInfo.x1 = (float)i_stop*deltaf;
  bfInfo.y1 = 2.0*pi*(float)i_stop*deltaf*del;


  TRACE("start index %d, stop index %d, delay %f, del conf %f\n",
	  bfInfo.start, bfInfo.stop, bfInfo.delay, bfInfo.del_c95);

  TRACE("Best fit line: x0 %f. y0 %f. x1 %f. y1 %f\n", 
	  bfInfo.x0, bfInfo.y0, bfInfo.x1, bfInfo.y1); 

  return TRUE;
}



/////////////////////////////////////////////////////
//
// Compare coherence data contained in two coherence files
// Checks that paths are valid, and data is coherence type
///////////////////////////////////////////////////////////
int Analysis::DoAnalysisCompCoh(const CString &path1, const CString &path2, 
								const CString &outputFilePath, resInfo &res)
{
	ifstream is1;
	ifstream is2;
	ofstream os;
	int pts;
	
	anFileInfo info1;
	anFileInfo info2;
	float thr = (float)0.001;

	an_load_data(path1, FILE_RES_COHERENCE, info1);
	an_load_data(path2, FILE_RES_COHERENCE, info2);

// must have same underlying sample rate
if(info1.rate != info2.rate) return ERR_INCOMPAT_SAMP_RATES;
if(info1.numPts != info2.numPts)
{
		TRACE("Number of points doesn't match\n");
		return ERR_INCOMPAT_NUM_PTS;
}
if(info1.segnum != info2.segnum)
{
	TRACE("Incompatible segment interval (L val)\n");
	return ERR_INCOMPAT_SEG_INTERVAL;
}

	pts = info1.numPts < info2.numPts ? info1.numPts : info2.numPts;

	os.open(outputFilePath, ios::out, filebuf::sh_none);
	if(os.bad()) return ERR_FILE_BAD;

	os << "difference-of-coherence" << '\n';
	os << info1.rate << '\n';
	os << info1.secs << '\n';
	os << info1.segnum << '\n';
	os << pts << '\n';
	os << setiosflags( ios::left ) << setiosflags(ios::fixed);

		for(int i=0; i<pts;i++)
		{
			if( fabs(info1.dataX[i] - info2.dataX[i]) > thr)
			{
				TRACE("Frequency values of points are inconsistent...terminating\n");
				return ERR_INCOMPAT_FREQ_VALS;
			}
			double val1 = invtanh( (float)(sqrt(info1.data[i])) );
			double val2 = invtanh( (float)(sqrt(info2.data[i])) );
			float diff = info1.data[i] - info2.data[i]; // calc tanh-1 diff here
			os << setprecision(6) << info1.dataX[i] << " " << val1-val2 << '\n';
		}
		// output confidence limits

		TRACE("Number of data points  - %d\n", pts);


		// single coherence confidence line

	float Lval = info1.segnum; // millisecs/segsize = length of segment
	float var = 1.96/sqrt(Lval);
		
	// upper line
	os << setprecision(6) << 0.0 << " " << + var << '\n' 
						<<  info1.dataX[pts-1] << " " << +var << '\n'; 
	// middle line
	os << setprecision(6) << 0.0 << " " << 0.0 << '\n' 
						<< info1.dataX[pts-1] << " " << 0.0 << '\n'; 
	// upper line
	os << setprecision(6) << 0.0 << " " << - var << '\n' 
						<<  info1.dataX[pts-1] << " " << - var << '\n'; 

	os.close();


		CFile tempFile(outputFilePath, CFile::modeRead | CFile::shareDenyNone);
//
		res.pathname = tempFile.GetFilePath();
		res.filename = tempFile.GetFileName();
		res.type = FILE_DIFF_COH;
//


// memory used to store coherence info is automatically destroyed in anFileInfo class

return 1;
}

/////////////////////////////////////////////////
// Do analysis for two data sets
// 
//  Ver 1.0   combined spike/analogue data analysis.  Electrodes 1 and 2 may
//			  be any combination of spike and analogue.
//			  Lots of checks are done to make sure that data types are each treated
//			  correctly.
//
//
// Original Function info ------>
/*
 File: sp2a2_a.c, 03/09/96, DMH. (Copied from z_sp2a2.c, Ver 2.10)
 Spectral Analysis Program for: 2 analogue channels.

 sp2a2_a:
 Ver 2.20  03/09/96  Includes option to deal with unequal sampling rates.
              "      Option to force analysis at specified rate: [-s rate]

 Addition  18/06/98   Addition of trend removal, using smoothed version of
        Ver: 2.30     original data to estimate trend. Specified by two
                      new optional parameters: [sm pts 1] [sm pts 2]

 Addition  09/02/99   Addition of scaling of data to have a pre-specified
        Ver: 2.40     value of Standard Deviation. Specified by two
                      new optional parameters: [sd scale 1] [sm scale 2]

 Modification 28/04/99 Simplification of data input section.
        Ver: 2.41
*/
int Analysis::DoAnalysisStandard(
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
		int smPts1, int smPts2, float sdScale1, float sdScale2
		)

{
/*--------------------------------------------------------------------------*/
/* Variables*/

	int   *spike_tot_1, *spike_tot_2, spike_start_1, spike_start_2;
  int   samp_rate_1,samp_rate_2, samp_rate_ref, samp_rate;
  int   sec_tot_1,sec_tot_2, an_sec_tot_ref;
  int   samp_fac_1=1,samp_fac_2=1;
  int   rate_f1,rate_f2,sec_tot_f;
  int   fil_no_1=0,fil_no_2=0, an_fil_no_ref=0;
  int   seg_size_pwr=0,seg_size,seg_size_2;
  int   seg_no,seg_count,seg_tot_f,i,han_flag=0;
  int   coh_flag=0,new_coh_fil=0;
  int   inv_flag=0,rect_chan_1=0,rect_chan_2=0,err_flag=0;
  int   out_fil_no=0,out_pts;
  int   freq_out=freq_default,freq_nyquist;
  int   lag_tot=2*lag_default,lag_neg=lag_default;
  int   lag_bin_tot,lag_bin_neg,q_scale_fac;
  int   fil_flag=0,fil_tot=1,fil_count,trend_chan_1=0,trend_chan_2=0;
  int   notch_flag=0,mains_ind,rect_type=0,no_plot_flag=0;
  int   samp_flag=0,samp_rate_fix;
  int   seg_start,seg_stop;
  int   *an_samp_tot_1, *an_samp_tot_2,an_samp_tot_f; 
	int   **spike_times_1, **spike_times_2, spike_tot_f1=0, spike_tot_f2=0;
  int   sm_pts_1=0,sm_pts_2=0;
  float *d1t_r,*d1t_i,*d2t_r,*d2t_i;
  float *f11,*f22,*f21_r,*f21_i,*q21;
  float *fi,*fo,**an_dat_1,**an_dat_2;
  float freq,deltaf,deltat,t_out;
  float two_pi_t_fac,var_mul=1.0;
  float fx21,ch21,ph21,f1l,f2l,fx_max,fx_mid;
  float f_var,f_sd,f_c95; 
	float Np1,log_p1,p1_u,p1_l; float Np2,log_p2,p2_u,p2_l;
  float ch21_pwr,ch21_c95,q_scale,q_max;
  float q21_fac,q_var,q21_c95,lag_start,lag_stop;
  float sd_scale_1=0.0,sd_scale_2=0.0;


  div_t samp_rate_div;


  bool phaseConf = true;

  bool spline_resampling = false;

/*---------------------------------------------------------------------------*/
/* Options/Parameters */

    if(hanning) 
	{
		han_flag++;
		TRACE("Hanning filter selected\n");
	}
    if(inversion)
	{
		inv_flag++;
		TRACE("Inverted electrode selection\n");
	}
    if(notch)
	{
		notch_flag++;
		TRACE("Notch flag selected\n");
	}

	if (rectify1 && elecType1 == 0)
	{
		rect_chan_1++;
		TRACE("Channel 1 set for rectification\n");
	}
	if (rectify2 && elecType2 == 0)
	{
		rect_chan_2++;
		TRACE("Channel 2 set for rectification\n");
	}
    
    if(fixedSampRate)
	{
		samp_flag++;
		samp_rate_fix = fixedSampRate;
		TRACE("Sampling rate fixed at %d\n", samp_rate_fix);
	}

    if (trendRemove1 && elecType1 == 0)
	{
		trend_chan_1++;
		TRACE("Trend removal selected for channel 1\n");
	}
	if (trendRemove2 && elecType2 == 0)
	{
		trend_chan_2++;
		TRACE("Trend removal selected for channel 2\n");
	}


	rect_type = rectType;
    if ( (rectify1 || rectify2) && rect_type != RECT_FULLWAVE && 
		rect_type != RECT_HALF_TOP && 
		rect_type != RECT_HALF_BOT) err_flag++;


// number of electrode data files
	fil_no_1=fileList1.size();
    fil_no_2=fileList2.size();


	if(fil_no_1 < 1 || fil_no_2 < 1)
	{
		TRACE("No data files for one of both channels\n");
		return ERR_NO_FILES;
	}
	TRACE("Electrode 1 has %d files\n", fil_no_1);
	TRACE("Electrode 2 has %d files\n", fil_no_2);

	if(fil_no_1 != fil_no_2)
	{
		TRACE("Warning, different number of files for each channel...\n"); 
		return ERR_DIFF_FILE_NUM;
	}

	TRACE("Each channel electrode formed from %d files\n", fil_no_1);

	fil_tot = fil_no_1; // = file_tot_2


// electrode data type
	if(elecType1 == 0) TRACE("Electrode 1 contains analogue data\n");
	else if(elecType1 == 1) TRACE("Electrode 1 contains point process data\n");
	else{
		TRACE("Electrode 1 contains undetermined data type..terminating\n");
		return ERR_UNDETERMINED_ELECTRODE;
	}

	if(elecType2 == 0) TRACE("Electrode 2 contains analogue data\n");
	else if(elecType2 == 1) TRACE("Electrode 2 contains point process data\n");
	else
	{
		TRACE("Electrode 2 contains undetermined data type..terminating\n");
		return ERR_UNDETERMINED_ELECTRODE;
	}

// make sure reference is used only with analogue electrodes
	if(elecType1 == 1 && useRef1)
	{
		TRACE("Reference electrode requested but won't be used with spike data electrode 1\n");
		useRef1 = FALSE;
	}
	if(elecType2 == 1 && useRef2)
	{
		TRACE("Reference electrode requested but won't be used with spike data electrode 2\n");
		useRef2 = FALSE;
	}

	if(useRef1 || useRef2)
	{
		an_fil_no_ref=refFileList.size();
		TRACE("Reference electrode specified...has %d files\n", an_fil_no_ref);
	}
	else
	{
		TRACE("Not using reference electrode\n");
	}

	if(useRef1 && an_fil_no_ref != fil_no_1)
	{
		TRACE("Warning, reference channel has different number of files than channel 1\n"); 
		return ERR_DIFF_FILE_NUM;
	}

	if(useRef2 && an_fil_no_ref != fil_no_2)
	{
		TRACE("Warning, reference channel has different number of files than channel 2\n"); 
		return ERR_DIFF_FILE_NUM;
	}


	if(elecType1 == 0 && elecType2 == 0) TRACE("	Two analogue files\n");
	else if(elecType1 == 1 && elecType2 == 1) TRACE("	Two point process files\n");
	else TRACE("	One analgue and one point process file\n");
	

	if(fil_no_1 > 1) fil_flag++;



// Set remaining parameters, if not accepting defaults.  Making sure that 
// inappropriate parameters selected for spike channels are cancelled

// params relevant to both analogue and spike data
	if(freqOut) freq_out=freqOut;
	TRACE("Output frequency %d\n", freq_out);

    if(segSizePwr) seg_size_pwr=segSizePwr;
	TRACE("Seg Size Power %d\n", seg_size_pwr);

	if(lagTot) lag_tot=lagTot;
	TRACE("Lag Tot %d\n", lag_tot);

	if(lagNeg) lag_neg=lagNeg;
	else lag_neg=lag_tot/2;
	TRACE("Lag Neg %d\n", lag_neg);

// analogue only trend removal
    if(smPts1 && elecType1 == 0) sm_pts_1=smPts1;
	TRACE("smPts1 %d\n", sm_pts_1);

    if(smPts2 && elecType2 == 0) sm_pts_2=smPts2;
	TRACE("smPts2 %d\n", sm_pts_2);

// analogue only standard deviation scaling
	sd_scale_1=sdScale1;
	TRACE("sdScale1 %d\n", sd_scale_1);
	if(elecType1 == 1){
		sdScale1 = 0.0;
		TRACE("Electrode 1 is Point process, ignoring SD scaling\n");
	}

    sd_scale_2=sdScale2;
	TRACE("sdScale2 %d\n", sd_scale_2);
	if(elecType2 == 1){
		sdScale2 = 0.0;
		TRACE("Electrode 2 is Point process, ignoring SD scaling\n");
	}
	

	if (err_flag) ERR_UNSUITABLE_PARAMETERS;


/*--------------------------------------------------------------------------*/
/* Check Trend removal smoothing */

// only valid for analogue channel data
  if (sm_pts_1>1 && sm_pts_1<(int)sm_pts_min) {
    sm_pts_1=(int)sm_pts_min;
    TRACE("\nWarning - Ch 1 trend removal smoothing set to minimum: %d\n\n",
           sm_pts_1);
  }
  if (sm_pts_2>1 && sm_pts_2<(int)sm_pts_min) {
    sm_pts_2=(int)sm_pts_min;
    TRACE("\nWarning - Ch 2 trend removal smoothing set to minimum: %d\n\n",
           sm_pts_2);
  }

/*--------------------------------------------------------------------------*/
/* Storage for Data */

  if(elecType1 == 0)
  {
	  TRACE("Allocating storage for electrode 1, analogue\n");
		an_samp_tot_1=ivector(0,fil_tot-1);
		an_dat_1=pvector(0,fil_tot-1);
  }
  else
  {
	  spike_tot_1=ivector(0,fil_tot-1);
	  spike_times_1=ipvector(0,fil_tot-1);

  }
  if(elecType2 == 0)
  {
	  TRACE("Allocating storage for electrode 2, analogue\n");
		an_samp_tot_2=ivector(0,fil_tot-1);
		an_dat_2=pvector(0,fil_tot-1);
  }
  else
  {
	  spike_tot_2=ivector(0,fil_tot-1);
	  spike_times_2=ipvector(0,fil_tot-1);
  }

/*--------------------------------------------------------------------------*/
/* Input Files */

 
	
    fil_count = 0;

	sec_tot_f=0;

	strCI p1 = fileList1.begin();
	strCI p2 = fileList2.begin();
	strCI pref = refFileList.begin();

	ifstream is1;
	ifstream is2;
	ifstream is_ref;
	float *ref_data = NULL;
	
	int totalSecs1 = 0;
	int totalSecs2 = 0;

	int chNumDummy;


	while(p1 != fileList1.end() && p2 != fileList2.end()) 
	{

// First get header info from each file (total record length and sampling rates)
		if(elecType1 == 0) // analogue (binary)
		{
			is1.open(*p1, ios::binary | ios::in | ios::nocreate, filebuf::sh_none);
			if(is1.bad()) return ERR_FILE_BAD;
			is1.read( (char *) &sec_tot_1, sizeof(int));
			is1.read( (char *) &samp_rate_1, sizeof(int));
		}
		else // point process (text)
		{
			is1.open(*p1, ios::in | ios::nocreate, filebuf::sh_none);
			if(is1.bad()) return ERR_FILE_BAD;
			is1 >> chNumDummy;
			is1 >> sec_tot_1;
			is1 >> samp_rate_1;
			// temp
			samp_rate_1 = 1000;
		}
		

		if(elecType2 == 0) // analogue (binary)
		{
			is2.open(*p2, ios::binary | ios::in | ios::nocreate, filebuf::sh_none);
			if(is2.bad()) return ERR_FILE_BAD;
			is2.read( (char *) &sec_tot_2, sizeof(int));
			is2.read( (char *) &samp_rate_2, sizeof(int));
		}
		else // point process (text)
		{
			is2.open(*p2, ios::in | ios::nocreate, filebuf::sh_none);
			if(is2.bad()) return ERR_FILE_BAD;
			is2 >> chNumDummy;
			is2 >> sec_tot_2;
			is2 >> samp_rate_2;
			// temp
			samp_rate_2 = 1000;
		}

		totalSecs1 += sec_tot_1;
		totalSecs2 += sec_tot_2;
		
// load in reference data if required (only if requested and there are any analogue channels)
		if(useRef1 || useRef2)
		{
			is_ref.open(*pref, ios::binary | ios::in | ios::nocreate, filebuf::sh_none);
			if(is_ref.bad()) return ERR_FILE_BAD;

			is_ref.read( (char *) &an_sec_tot_ref, sizeof(int));
			is_ref.read( (char *) &samp_rate_ref, sizeof(int));

			if(useRef1 && samp_rate_1 != samp_rate_ref)
			{
			TRACE("Can't use reference with channel 1 unless original sampling rates are identical");
				return ERR_INCOMPAT_SAMP_RATES;
			}
			if(useRef2 && samp_rate_2 != samp_rate_ref)
			{
			TRACE("Can't use reference with channel 2 unless original sampling rates are identical");
				return ERR_INCOMPAT_SAMP_RATES;
			}
			ref_data = new float[an_sec_tot_ref*samp_rate_ref];

			is_ref.read( (char *)ref_data, sizeof(float)*(size_t)(an_sec_tot_ref*samp_rate_ref) );
		}


  
	TRACE("Using David's routines for setting sampling rates\n");

	
		TRACE("		David's sampling for 2 analogue electrodes\n");
// Check consistency of sampling rates


		if(elecType1 == 1 && samp_rate_1 != 1000)
		{
			TRACE("Warning...point process electrode 1 doesn't have a 1000Hz samp rate\n");
		}

		if(elecType2 == 1 && samp_rate_2 != 1000)
		{
			TRACE("Warning...point process electrode 2 doesn't have a 1000Hz samp rate\n");
		}


		if (!samp_flag) { // Do our best to use the original sampling rates

		  if (samp_rate_1==samp_rate_2) {
			samp_rate=samp_rate_1;
			freq_nyquist=samp_rate/2;
		  }
		  else {
			if (samp_rate_1>samp_rate_2) {
				if(elecType2 == 1) TRACE("Problem...PP elec 2 has lower samp rate\n");
			  samp_rate=samp_rate_1;
			  samp_rate_div=div(samp_rate_1,samp_rate_2);
			  samp_fac_2=samp_rate_div.quot;
			  freq_nyquist=samp_rate_2/2;
			}
			else {
				if(elecType1 == 1) TRACE("Problem...PP elec 1 has lower samp rate\n");
			  samp_rate=samp_rate_2;
			  samp_rate_div=div(samp_rate_2,samp_rate_1);
			  samp_fac_1=samp_rate_div.quot;
			  freq_nyquist=samp_rate_1/2;
			}
			if (!(((samp_rate_div.quot==2) || (samp_rate_div.quot==4) ||
				  (samp_rate_div.quot==5)) && (samp_rate_div.rem==0))) {
			  TRACE("\nFiles: %s, %s",*p1,*p2);
			  TRACE("\n Incompatible sampling rates: %d, %d\n	Will try spline sampling\n",
				  samp_rate_1,samp_rate_2);

			  spline_resampling = true; // incompatible samp rates...try and use splines
			}
		  }
		}

		else { // Try and use the given sampling frequency
			if( (elecType1 == 1 || elecType2 == 1) && samp_rate_fix != sampPP )
			{
				TRACE("Warning...can't use this fixed frequency with PP electrode\n");
				TRACE("Fixing sampling frequency at 1000 Hz\n");
				samp_rate_fix = sampPP;
			}

		  samp_rate_div=div(samp_rate_fix,samp_rate_1);
		  samp_fac_1=samp_rate_div.quot;
		  if (!(((samp_rate_div.quot==1) ||(samp_rate_div.quot==2) ||
				(samp_rate_div.quot==4) || (samp_rate_div.quot==5)) &&
				(samp_rate_div.rem==0))) {
			TRACE("\nFile: %s",*p1);
			TRACE("\n Rate: %d, Incompatible with fixed rate: %d\n\n",
				samp_rate_1,samp_rate_fix);
			spline_resampling = true;  // incompatible samp rates...try and use splines
		  }
		  samp_rate_div=div(samp_rate_fix,samp_rate_2);
		  samp_fac_2=samp_rate_div.quot;
		  if (!(((samp_rate_div.quot==1) ||(samp_rate_div.quot==2) ||
				(samp_rate_div.quot==4) || (samp_rate_div.quot==5))
				&& (samp_rate_div.rem==0))) {
			TRACE("\nFile: %s",*p2);
			TRACE("\n Rate: %d, Incompatible with fixed rate: %d\n\n",
				samp_rate_2,samp_rate_fix);
			spline_resampling = true;  // incompatible samp rates...try and use splines
		  }
		  samp_rate=samp_rate_fix;
		  if (samp_rate_1>samp_rate_2)
			freq_nyquist=samp_rate_2/2;
		  else
			freq_nyquist=samp_rate_1/2;
		}


		if (fil_count==0) {
		  rate_f1=samp_rate_1;
		  rate_f2=samp_rate_2;
		}
		else {
		  if (samp_rate_1!=rate_f1) {
			TRACE("\nFile: %s",*p1);
			TRACE("\n Analogue sampling rate inconsistent - ");
			TRACE("Previous: %d   Present %d\n\n",rate_f1,samp_rate_1);
			return ERR_INCONSIS_SAMP_RATE;
		  }
		  if (samp_rate_2!=rate_f2) {
			TRACE("\nFile: %s",*p2);
			TRACE("\n Analogue sampling rate inconsistent - ");
			TRACE("Previous: %d   Present %d\n\n",rate_f2,samp_rate_2);
			return ERR_INCONSIS_SAMP_RATE;
		  }
		}
	
	

// Load in electrode 1 info
		TRACE("Now loading in electrode data...\n");
		if(elecType1 == 0) // Analogue
		{
			if(useRef1)
			{
				
				if(!spline_resampling) // Just use David's rountine...
				{
					an_samp_tot_1[fil_count]=an_in_f_d(&an_dat_1[fil_count],
						sec_tot_1*samp_rate_1, is1, ref_data, samp_fac_1,sm_pts_1);
				}
				else
				{

				}
			}
			else
			{
				if(!spline_resampling) // Just use David's rountine
				{
					an_samp_tot_1[fil_count]=an_in_f_d(&an_dat_1[fil_count],
						sec_tot_1*samp_rate_1, is1, NULL, samp_fac_1,sm_pts_1);	
				}
				else
				{

				}
				
			}
		}
		else // Point process
		{
			is1 >> spike_tot_1[fil_count];
			spike_times_1[fil_count] = ivector(0, spike_tot_1[fil_count]-1);
			for(i=0;i<spike_tot_1[fil_count];i++) is1 >> spike_times_1[fil_count][i];
		}
		is1.close();

		TRACE("Successfully loaded electrode 1 data\n");

// Load in electrode 2 info
		if(elecType2 == 0) // Analogue
		{
			if(useRef2){
				if(!spline_resampling) // Just use David's rountine
				{
					an_samp_tot_2[fil_count]=an_in_f_d(&an_dat_2[fil_count],
						sec_tot_2*samp_rate_2, is2, ref_data, samp_fac_2,sm_pts_2);
				}
				else
				{

				}
			}
			else
			{	
				if(!spline_resampling) // Just use David's routine
				{
					an_samp_tot_2[fil_count]=an_in_f_d(&an_dat_2[fil_count],
						sec_tot_2*samp_rate_2, is2, NULL, samp_fac_2,sm_pts_2);
				}
				else
				{

				}
			}
		}
		else // Point process
		{
			is2 >> spike_tot_2[fil_count];
			spike_times_2[fil_count] = ivector(0, spike_tot_2[fil_count]-1);
			for(i=0;i<spike_tot_2[fil_count];i++) is2 >> spike_times_2[fil_count][i];
		}
		is2.close();

		TRACE("Successfully loaded electrode 2 data\n");



// Update counters and iterators
		fil_count++;

		sec_tot_f+=sec_tot_1;
		
		++p1;
		++p2;
		++pref;
		if(useRef1 || useRef2) // make sure reference data is deleted
		{
			delete ref_data; ref_data = NULL;
			is_ref.close();
		}
	}

	if(fil_count > 1) fil_flag=1;

	
	TRACE("Total files for each channel ... counted %d; supposed to be %d\n", 
		fil_count, fil_tot);

	if(totalSecs1 != totalSecs2)
	{
		TRACE("Error - Channels have different total record length....\n");
		return ERR_TOTAL_SECS;
	}


/*--------------------------------------------------------------------------*/
/* Check parameters */

  if (seg_size_pwr==0)
    seg_size_pwr=seg_pwr_default;
  if (seg_size_pwr<seg_pwr_low) {
    TRACE("\nWarning - Periodogram power set to lower limit: %d\n\n",
	   seg_pwr_low);
    seg_size_pwr=seg_pwr_low;
  }
  if (seg_size_pwr>seg_pwr_high) {
    TRACE("\nWarning - Periodogram power set to upper limit: %d\n\n",
	   seg_pwr_high);
    seg_size_pwr=seg_pwr_high;
  }
  seg_size=(int)(pow(2.0,(float)seg_size_pwr)+0.5);
  seg_size_2=seg_size/2;

  if (freq_out>freq_nyquist) {
    TRACE("\nWarning - output freq reduced to nyquist frequency: %d\n\n",
	   freq_nyquist);
    freq_out=freq_nyquist;
  }
  deltaf=(float)samp_rate/(float)seg_size;
  out_pts=(int)((float)freq_out/deltaf);
  if(out_pts>=seg_size_2-2)
    out_pts=seg_size_2-2;

  deltat=1000.0/samp_rate;
  lag_bin_tot=(int)((float)lag_tot/deltat);
  lag_bin_neg=(int)((float)lag_neg/deltat);
  if (lag_bin_tot>seg_size || lag_bin_neg>seg_size_2) {
    TRACE("Warning - lag bin range reduced to periodogram length: %d\n\n",
           seg_size);
    lag_bin_tot=seg_size;
    lag_bin_neg=seg_size_2;
  }

/*--------------------------------------------------------------------------*/
/* Scale data by pre-specified SD */

  if (sd_scale_1>0.0 && elecType1 == 0)
    for (fil_count=0;fil_count<fil_tot;fil_count++)
      an_sdscale(an_dat_1[fil_count],an_samp_tot_1[fil_count],sd_scale_1);
  
  if (sd_scale_2>0.0 && elecType2 == 0)
    for (fil_count=0;fil_count<fil_tot;fil_count++)
      an_sdscale(an_dat_2[fil_count],an_samp_tot_2[fil_count],sd_scale_2);

/*--------------------------------------------------------------------------*/
/* Rectify Analogue data */

  if (rect_chan_1)
    for (fil_count=0;fil_count<fil_tot;fil_count++)
      an_rect_a(an_dat_1[fil_count],an_samp_tot_1[fil_count],rect_type);
  
  if (rect_chan_2)
    for (fil_count=0;fil_count<fil_tot;fil_count++)
      an_rect_a(an_dat_2[fil_count],an_samp_tot_2[fil_count],rect_type);

/*--------------------------------------------------------------------------*/
/* Memory Allocation */

  d1t_r=vector(0,seg_size-1);
  d2t_r=vector(0,seg_size-1);
  q21=vector(0,seg_size-1);
  d1t_i=vector(0,seg_size_2); /* <-NB */
  d2t_i=vector(0,seg_size_2); /*  ""  */
  f11=vector(0,seg_size_2);
  f22=vector(0,seg_size_2);
  f21_r=vector(0,seg_size_2);
  f21_i=vector(0,seg_size_2);
  for (i=0;i<=seg_size_2;i++) {
    f11[i]=0;
    f22[i]=0;
    f21_r[i]=0;
    f21_i[i]=0;
  }
  
/*--------------------------------------------------------------------------*/
/* Main Loop */

  spike_tot_f1=0;
  spike_tot_f2=0;
  seg_tot_f=0;
  if(elecType1 == 0 && elecType2 == 0)
  {
	TRACE("Total Sec: %d   Rate(fac): %d(%d); %d(%d)   T: %d\n",sec_tot_f,
         samp_rate_1,samp_fac_1,samp_rate_2,samp_fac_2,seg_size);
  }
  else if( (elecType1 == 0 && elecType2 == 1) || (elecType1 == 1 && elecType2 == 0) )
  {
	  TRACE("Total Sec: %d   Rate(fac) - An: %d(%d); Sp: %d   T: %d\n",
		  sec_tot_f, elecType1 == 0 ? samp_rate_1 : samp_rate_2,
					 elecType1 == 0 ? samp_fac_1 : samp_fac_2,
					 elecType2 == 0 ? samp_rate_1 : samp_rate_2,
					 elecType2 == 0 ? samp_fac_1 : samp_fac_2, seg_size);
  }

  for (fil_count=0;fil_count<fil_tot;fil_count++) {

	spike_start_1=0;
	spike_start_2=0;
    seg_start=0;
    seg_stop=seg_size;

	 // get number of segments when dealing with analogue data
	if(elecType1==0)
		seg_no=(int)(an_samp_tot_1[fil_count]/seg_size);
	else if(elecType2==0)
		seg_no=(int)(an_samp_tot_2[fil_count]/seg_size);

    seg_tot_f+=seg_no;

	// when just dealing with point process data (temporary fix at the moment)
	if(elecType1 == 1 && elecType2 == 1) 
		seg_no = (int)(1000*totalSecs1/seg_size);
	else
	{
    TRACE("Processing: %d  ",fil_count+1);
    TRACE(" Samples: %6d   Segments: %3d\n",an_samp_tot_1[fil_count],seg_no);
	}

    if (seg_no==0 && (!fil_flag)) {
      TRACE("\nData too short for this segment length\n\n");
      return ERR_DATA_SHORT_FOR_SEGMENT;
    }
    if (seg_no<10 && (!fil_flag))
      TRACE("\nWarning - low number of segments\n\n");

    for (seg_count=0;seg_count<seg_no;seg_count++) {

		if(elecType1 == 0) // analogue
		{
			ts_trend(&an_dat_1[fil_count][seg_start],seg_size,trend_chan_1);
			ts_ft(&an_dat_1[fil_count][seg_start],seg_size,seg_size,d1t_r,d1t_i,0);
		}
		else // point process
		{
			spike_tot_f1+=pp_ft(spike_times_1[fil_count],spike_tot_1[fil_count],
				&spike_start_1,seg_start,seg_stop,seg_size,d1t_r,d1t_i,0);
		}
		
		if(elecType2 == 0) // analogue
		{
			ts_trend(&an_dat_2[fil_count][seg_start],seg_size,trend_chan_2);
			ts_ft(&an_dat_2[fil_count][seg_start],seg_size,seg_size,d2t_r,d2t_i,0);
		}
		else // point process
		{
			spike_tot_f2+=pp_ft(spike_times_2[fil_count],spike_tot_2[fil_count],
				&spike_start_2,seg_start,seg_stop,seg_size,d2t_r,d2t_i,0);
		}
     
      for (i=0;i<=seg_size_2;i++) {
        f11[i]+=rmagn2(d1t_r[i],d1t_i[i]);
        f22[i]+=rmagn2(d2t_r[i],d2t_i[i]);
        f21_r[i]+=frba(d1t_r[i],d1t_i[i],d2t_r[i],d2t_i[i]);
        f21_i[i]+=fiba(d1t_r[i],d1t_i[i],d2t_r[i],d2t_i[i]);
      }
      seg_start=seg_stop;
      seg_stop+=seg_size;
    }
	TRACE("		Processed a pair of data files\n");

  }
 
   TRACE("Total segments: %d\n", seg_tot_f);



// free original data storage

	if(elecType1 == 0) // analogue
	{
		TRACE("Freeing up analogue data storage for electrode 1\n");
		free_ivector(an_samp_tot_1, 0);

		for(fil_count=0;fil_count<fil_tot;fil_count++) 
		{
			free_vector(an_dat_1[fil_count], 0);
			TRACE("		Freed a data set in electrode 1\n");
		}
		TRACE("		About to free the data pointers in electrode 1\n");
		//free_pvector(an_dat_1, 0);
		TRACE("		Freed the data pointers in electrode 1\n");
	}
	else // point process
	{
		TRACE("Freeing up point process data storage for electrode 1\n");
		//free_ivector(spike_tot_1, 0);

		//for (fil_count=0;fil_count<fil_tot;fil_count++) 
		//{
		//	free_ivector(spike_times_1[fil_count], 0);
		//}
		//free_ipvector(spike_times_1, 0);
	}


	if(elecType2 == 0) // analogue
	{
		TRACE("Freeing up analogue data storage for electrode 2\n");
		free_ivector(an_samp_tot_2, 0);
		for (fil_count=0;fil_count<fil_tot;fil_count++) 
		{
			free_vector(an_dat_2[fil_count], 0);
			TRACE("		Freed a data set in electrode 2\n");
		}
		//free_pvector(an_dat_2, 0);
		TRACE("		Freed the data pointers in electrode 2\n");
	}
	else // point process
	{
		TRACE("Freeing up point process data storage for electrode 2\n");
		//free_ivector(spike_tot_2, 0);

		//for (fil_count=0;fil_count<fil_tot;fil_count++) 
		//{
		//	free_ivector(spike_times_2[fil_count], 0);
		//}
		//free_ipvector(spike_times_2, 0);
	}
 
/*--------------------------------------------------------------------------*/
/* Normalize spectra, invert, estimate cumulant, hanning & confidence limits*/

  two_pi_t_fac=2.0*pi*(float)(seg_size*seg_tot_f);
  for (i=0;i<=seg_size_2;i++) {
    f11[i]/=two_pi_t_fac;
    f22[i]/=two_pi_t_fac;
    f21_r[i]/=two_pi_t_fac;
    f21_i[i]/=two_pi_t_fac;
  }
  if (inv_flag)
    for (i=0;i<=seg_size_2;i++)
      f21_i[i]= -f21_i[i];

  if (notch_flag) {
    mains_ind=(int)((50.0/deltaf));
    f11[mains_ind]=0.5*(f11[mains_ind-2]+f11[mains_ind+2]);
    f11[mains_ind-1]=0.5*(f11[mains_ind-2]+f11[mains_ind-3]);
    f11[mains_ind+1]=0.5*(f11[mains_ind+2]+f11[mains_ind+3]);
    f22[mains_ind]=0.5*(f22[mains_ind-2]+f22[mains_ind+2]);
    f22[mains_ind-1]=0.5*(f22[mains_ind-2]+f22[mains_ind-3]);
    f22[mains_ind+1]=0.5*(f22[mains_ind+2]+f22[mains_ind+3]);
    f21_r[mains_ind]=0.5*(f21_r[mains_ind-2]+f21_r[mains_ind+2]);
    f21_r[mains_ind-1]=0.5*(f21_r[mains_ind-2]+f21_r[mains_ind-3]);
    f21_r[mains_ind+1]=0.5*(f21_r[mains_ind+2]+f21_r[mains_ind+3]);
    f21_i[mains_ind]=0.5*(f21_i[mains_ind-2]+f21_i[mains_ind+2]);
    f21_i[mains_ind-1]=0.5*(f21_i[mains_ind-2]+f21_i[mains_ind-3]);
    f21_i[mains_ind+1]=0.5*(f21_i[mains_ind+2]+f21_i[mains_ind+3]);
  }


// need to check these are ok when 2 spike channels..
  an_samp_tot_f=seg_tot_f*seg_size;
  q21_fac=2.0*pi/(float)seg_size;
  q12_ft(f21_r,f21_i,seg_size,seg_size_2,q21,q21_fac);
  q_var=q12_var(f11,f22,seg_size,seg_size_2,(long)an_samp_tot_f);

  if (han_flag) {
    han(f11,seg_size_2);
    han(f22,seg_size_2);
    han(f21_r,seg_size_2);
    han(f21_i,seg_size_2);
    var_mul=0.375;
  }

  f_var=1.0/(float)seg_tot_f;
  f_var*=var_mul;
  f_sd=sqrt(f_var);
  f_c95=0.8512*f_sd;


if(elecType1 == 1 && elecType2 == 0)
{
	Np1=(float)spike_tot_f1/(float)(an_samp_tot_f*2.0*pi);
	log_p1=log10(Np1);
	p1_u=log_p1+f_c95;
	p1_l=log_p1-f_c95;
}
else if(elecType2 == 1 && elecType1 == 0)
{
	Np2=(float)spike_tot_f2/(float)(an_samp_tot_f*2.0*pi);
	log_p2=log10(Np2);
	p2_u=log_p2+f_c95;
	p2_l=log_p2-f_c95;
}
else if(elecType1 == 1 && elecType2 == 1)
{
}

  ch21_pwr=1.0/((float)seg_tot_f-1.0);
  ch21_pwr*=var_mul;
  ch21_c95=1.0-pow(0.05,ch21_pwr);
  q21_c95=1.96*sqrt(q_var);


// ok.......all analysis has been done
//			now need to output info into files...


  TRACE("OK...preparing to output results files\n");


	CString title;
	CString title1 = elec1name;
	CString title2 = elec2name;
	CString titleX = outputNameStem;
	if(!outputNameStem.IsEmpty()) outputNameStem += CString("_");
	
	CString currentFullPath;

	resInfo res;

	ofstream os;

	if (!inv_flag) {

		if (rect_chan_1) title1 += CString("-R");
		if (rect_chan_2) title2 += CString("-R");
     
		titleX += "_yx";

		fi=f11;
		fo=f22;
	}
	else {

		if (rect_chan_1) title2 += CString("-R");
		if (rect_chan_2) title1 += CString("-R");
     
		titleX += "_yx";
    
		fi=f22;
		fo=f11;
	}


	if(doSpectrum1)
	{
		
	
		if (!inv_flag) 
			title =  outputNameStem  + title1 + CString(".spc");
		else 
			title =  outputNameStem  + title2 + CString(".spc");

		currentFullPath = outputPathName + title ;


		

		os.open((LPCSTR)currentFullPath, ios::out, filebuf::sh_none);
		if(os.bad()) return ERR_FILE_BAD;
		

	
		os << "log10-power-spectrum" << '\n';
		
		os <<  samp_rate << '\n';
		os << totalSecs1 << '\n';
		os << out_pts << '\n';
		
		os << setiosflags( ios::left ) << setiosflags(ios::fixed);

		fx_max=log10(f11[1]);

		for(i=1;i<=out_pts;i++)  {
		  f1l=log10(f11[i]);
		  freq=(float)i*deltaf;
		  os << setprecision(6) << freq << " " << f1l << '\n';
		  if(f1l>fx_max)
			fx_max=f1l;
		}
		fx_mid=fx_max-f_c95;
		freq=(float)out_pts*deltaf*1.0;
		os << setprecision(6) << 
			freq << " " << fx_mid-f_c95 << '\n' << freq << " " <<  fx_mid+f_c95;
		os.close();

		
		CFile tempFile(currentFullPath, CFile::modeRead | CFile::shareDenyNone);
//
		res.pathname = tempFile.GetFilePath();
		res.filename = title;
		res.type = FILE_RES_SPECTRA;
		outputFileInfoList.insert(outputFileInfoList.end(), res);
//

	}

	if(doSpectrum2)
	{
		if (!inv_flag)
		  title = outputNameStem  + title2  + CString(".spc");
		else
		  title = outputNameStem  + title1  + CString(".spc");

		currentFullPath = outputPathName + title ;

		os.open((LPCSTR)currentFullPath,  ios::out, filebuf::sh_none);
		if(os.bad()) return ERR_FILE_BAD;
		
		
		os << "log10-power-spectrum" << '\n';
	
		os <<  samp_rate << '\n';
		os << totalSecs1 << '\n';
		os << out_pts << '\n';
		
		os << setiosflags(ios::left) << setiosflags(ios::fixed);

		fx_max=log10(f22[1]);

		for(i=1;i<=out_pts;i++)  {
		  f2l=log10(f22[i]);
		  freq=(float)i*deltaf;
		  os << setprecision(6) << freq << " " <<  f2l << '\n';
		  if(f2l>fx_max)
			fx_max=f2l;
		}
		fx_mid=fx_max-f_c95;
		freq=(float)out_pts*deltaf*1.0;
		os << setprecision(6) << 
			freq <<  " " << fx_mid-f_c95 << '\n' << freq << " " <<  fx_mid+f_c95;
		os.close();

		
		CFile tempFile(currentFullPath, CFile::modeRead | CFile::shareDenyNone);
		
//
		res.pathname = tempFile.GetFilePath();
		res.filename = title;
		res.type = FILE_RES_SPECTRA;
		outputFileInfoList.insert(outputFileInfoList.end(), res);
//
	}



	if(doCoherence)
	{
		title = outputNameStem + title1 +
								 CString("_") + title2 + CString(".coh");
		currentFullPath = outputPathName + title;



		os.open((LPCSTR)currentFullPath, ios::out, filebuf::sh_none);
		if(os.bad()) return ERR_FILE_BAD;
	
		os << "coherence" << '\n';
		os <<  samp_rate << '\n';
		os << totalSecs1 << '\n';
		os << seg_tot_f << '\n';
		os << out_pts << '\n';

		for(i=1;i<=out_pts;i++) {
		  fx21=rmagn2(f21_r[i],f21_i[i]);
		  ch21=cab(fx21,fi[i],fo[i]);
		  freq=(float)i*deltaf;
		  os << setprecision(6) << freq <<  " " << ch21 << '\n';
		  }
		os <<  setprecision(6) << 
			0.0 << " " <<  ch21_c95 << '\n' << (float)freq_out << " " <<  ch21_c95;
		os.close();

	
		CFile tempFile(currentFullPath, CFile::modeRead | CFile::shareDenyNone);
//
		res.pathname = tempFile.GetFilePath();
		res.filename = title;
		res.type = FILE_RES_COHERENCE;
		outputFileInfoList.insert(outputFileInfoList.end(), res);
//
	}

	if(doPhase)
	{

		title = title = outputNameStem  + title1 +
								 CString("_") + title2 +  CString(".phz");
		currentFullPath = outputPathName + title;


		os.open((LPCSTR)currentFullPath, ios::out, filebuf::sh_none);
		if(os.bad()) return ERR_FILE_BAD;

		os << "phase" << '\n';
		os <<  samp_rate << '\n';
		os << totalSecs1 << '\n';
		os << out_pts << '\n';

		
		for(i=1;i<=out_pts;i++) {
		  ph21=atan2(f21_i[i],f21_r[i]);
		  freq=(float)i*deltaf;
		  os << setprecision(6) << freq << " " <<  ph21 << '\n';
		  }
		os << setprecision(6) << 
			0.0 << " " <<  0.0 << '\n' << (float)freq_out << " " <<  0.0;

		if(phaseConf)
		{
			float pconf;

			os << '\n' << "CONF" << '\n';
			for(i=1;i<=out_pts;i++) {
				fx21=rmagn2(f21_r[i],f21_i[i]);
				ch21=cab(fx21,fi[i],fo[i]);
				freq=(float)i*deltaf;
				pconf = 1.96*sqrt( ((1.0/ch21) - 1.0)/(2*seg_tot_f) );
				os << setprecision(6) << freq << " " <<  pconf << '\n';
			}
		}

		os.close();

	
		CFile tempFile(currentFullPath, CFile::modeRead | CFile::shareDenyNone);
	
//
		res.pathname = tempFile.GetFilePath();
		res.filename = title;
		res.type = FILE_RES_PHASE;
		outputFileInfoList.insert(outputFileInfoList.end(), res);
//
	}

	if(doCumulant)
	{
		title = title = outputNameStem + title1 +
								 CString("_") + title2 +  CString(".cum");
		currentFullPath = outputPathName + title;
	

		os.open((LPCSTR)currentFullPath, ios::out, filebuf::sh_none);
		if(os.bad()) return ERR_FILE_BAD;

		lag_start=(float)-lag_bin_neg*deltat;
		lag_stop=(float)(lag_bin_tot-lag_bin_neg)*deltat;
		q_max=0;
		for(i=0;i<lag_bin_tot;i++) {
		  q_scale=q21[i+seg_size_2-lag_bin_neg];
		  if (q_scale<0.0)
			q_scale*=-1.0;
		  if (q_scale>q_max)
			q_max=q_scale;
		}
		q_scale_fac= -(int)log10(q_max)+1;
		q_scale=pow(10.0,q_scale_fac);
		
		os << "cumulant" << '\n';
		os <<  samp_rate << '\n';
		os << totalSecs1 << '\n';
		os << lag_bin_tot << '\n';

		for(i=0;i<lag_bin_tot;i++) {
		  t_out=(i-lag_bin_neg)*deltat;
		  os << setprecision(6) << t_out << " " << q21[i+seg_size_2-lag_bin_neg]*q_scale << '\n';
		}
		os << setprecision(6) << 
			lag_start << " " <<  0.0 << '\n' << lag_stop << " " <<  0.0 << '\n';
		os << setprecision(6) << 
			lag_start << " " <<  q21_c95*q_scale << '\n' << lag_stop << " " <<  q21_c95*q_scale << '\n';
		os << setprecision(6) << 
			lag_start << " " <<  -q21_c95*q_scale << '\n' << lag_stop << " " <<  -q21_c95*q_scale;
		os.close();

	
		CFile tempFile(currentFullPath, CFile::modeRead | CFile::shareDenyNone);
	
//
		res.pathname = tempFile.GetFilePath();
		res.filename = title;
		res.type = FILE_RES_CUMULANT;
		outputFileInfoList.insert(outputFileInfoList.end(), res);
//
	}


	// free up all the memory storage for results...seems to be a wierd problem

//results memory
	TRACE("About to free up all the memory used for results\n");
/*
	free_vector(d1t_r, 0);
	free_vector(d2t_r, 0);
	free_vector(q21, 0);
	free_vector(d1t_i, 0);
	free_vector(d2t_i, 0);
	free_vector(f11, 0);
	free_vector(f22, 0);
	free_vector(f21_r, 0);
	free_vector(f21_i, 0);
*/



return 1;
}





/*
 From File: s3sub.c, DMH, 23/04/99
 Subroutine file for Fourier/spectral analysis programs.
 Version 3.0
*/

/*--------------------------------------------------------------------------*/

int Analysis::pp_ft(int *spike_times,int spike_tot,int *spike_start,
            int samp_start,int samp_stop,int seg_size,
            float *dt_r,float *dt_i,int fn_flag)
/*
 pp_ft: Routine to estimate finite Fourier transform for segment of
 point process data. Two sections:
             1: Determine number of spikes in segment and set up zero mean
                Point Process representation.
             2: Perform DFT on this sequence using real valued FFT routine,
                and return real and imaginery values in separate arrays.

 Suitable for variable length data segments, includes zero padding if no of
 samples in data segment less than FFT segment (seg_size). Return value is
 number of spikes in segment.

Input variables
  spike_times   :Array of spike times.
  spike_tot     :Total number of spikes in array.
  spike_start   :Spike index to start scanning for valid spikes in segment.
  samp_start    :First sample in segment.
  samp_stop     :Last  sample in segment (These define range of spike times).
  seg_size      :Number of samples in FFT segment (power of 2).
  dt_r          :Array for real values, required index range: 0 - (seg_size-1)
                 Storage for seg_size values needed as input to FFT.
  dt_i          :Array for imaginery values, required index range:
                 [0 - (seg_size/2)] for values up to   Pi
                 [0 - (seg_size-1)] for values up to 2*Pi.
  fn_flag       :Flag to signal frequency range of dt_r & dt_i values to
                 output: 0 - Up to Pi, 1 - Up to 2*Pi.

 Changed on output
   spike_start  :Index of first spike past end of segment, time>=samp_stop.
                 Normally passed as spike_start for next segment.
   dt_r         :Real part of DFT, Hermitian sequence, range of values
                 controlled by fn_flag.
   dt_i         :Imaginery part of DFT. Range as for real values.
*/

{        
  int   ind,spike_count,dat_pts;
  int   seg_time,spike_no;
  float spike_zero,spike_one;

/*
 Local variables
    ind          :Index counter: For setting up zero valued Point process, and
                  re-ordering FFT values.
    spike_count  :Spike counter for spikes in present segment.
    dat_pts      :No of samples in segment.
    seg_time     :Time of each spike within segment, used as array index
                  to set up Point process array.
    spike_no     :Number of spikes in segment.
    spike_zero   :Equivalent zero value (No spike) for zero-mean Point Process.
    spike_one    :Equivalent one value (Spike present) for zero mean Point
                  Process.
*/

  dat_pts=(int)(samp_stop-samp_start)+1;  /* No of samples in segment */
  spike_count= *spike_start;
  while (spike_times[spike_count]<samp_start && spike_count<spike_tot)
    spike_count++;
  *spike_start=spike_count;  /* Now have first spike in segment */

  while (spike_times[spike_count]<=samp_stop && spike_count<spike_tot)
    spike_count++;
  spike_no= spike_count- *spike_start;  /* No of spikes in segment */

  spike_zero= -(float)spike_no/(float)dat_pts;
  spike_one=1.0+spike_zero;
  for (ind=0;ind<dat_pts;ind++)
    dt_r[ind]=spike_zero;  /* Set all samples to spike_zero value */
  for (ind= *spike_start;ind<spike_count;ind++) {
    seg_time=(int)(spike_times[ind]-samp_start);
    dt_r[seg_time]=spike_one;  /* Set samples with spike to spike_one value */
  }
  *spike_start=spike_count;  /* Now points to first spike after segment */

  for (ind=dat_pts;ind<seg_size;ind++)
    dt_r[ind]=0;  /* Zero padding, if necessary */

  rvfft(dt_r,seg_size);

  if (fn_flag)
    for (ind=1;ind<seg_size/2;ind++) {
      dt_i[ind]=dt_r[seg_size-ind];
      dt_i[seg_size-ind]= -dt_i[ind];  /* Values up to 2*Pi */
      dt_r[seg_size-ind]=dt_r[ind];
    }
  else
    for (ind=1;ind<seg_size/2;ind++)
      dt_i[ind]=dt_r[seg_size-ind];   /* Values up to Pi */

  dt_i[0]=0.0;
  dt_i[seg_size/2]=0.0;

  return (spike_no);
}

/*--------------------------------------------------------------------------*/

void Analysis::ts_ft(float *an_dat,int an_pts,int seg_size,
            float *dt_r,float *dt_i,int fn_flag)
/*
 ts_ft: Routine to estimate finite Fourier transform for segment of
 real valued time series. Two sections:
             1: Set up (zero mean) floating point representation.
             2: Perform DFT on this sequence using real valued FFT routine,
                and return real and imaginery values in separate arrays.

 Suitable for variable length data segments, includes zero padding if no of
 samples in data segment less than FFT segment (seg_size). No return value.
 Routine expects input segment to be zero mean and have any trend removed.

Input variables
  an_dat        :Pointer to first data sample in segment, NOT first sample in
                 array.
  an_pts        :Number of samples in segment.
  seg_size      :number of sample in FFT (power of 2).
  dt_r          :Array for real values, required index range: 0 - (seg_size-1)
                 Storage for seg_size values needed as input to FFT.
  dt_i          :Array for imaginery values, required index range:
                 [0 - (seg_size/2)] for values up to   Pi
                 [0 - (seg_size-1)] for values up to 2*Pi.
  fn_flag       :Flag to signal frequency range of dt_r & dt_i values to
                 output: 0 - Up to Pi, 1 - Up to 2*Pi.

 Changed on output
   dt_r         :Real part of DFT, Hermitian sequence, range of values
                 controlled by fn_flag.
   dt_i         :Imaginery part of DFT. Range as for real values.
*/

{        
  int   ind;

/*
 Local variables
    ind          :Index counter.
*/

  for (ind=0;ind<an_pts;ind++)
    dt_r[ind]=an_dat[ind];  /* Data samples */
  for (;ind<seg_size;ind++)
    dt_r[ind]=0.0;  /* Zero padding, if necessary */

  rvfft(dt_r,seg_size);

  if (fn_flag)
    for (ind=1;ind<seg_size/2;ind++) {
      dt_i[ind]=dt_r[seg_size-ind];
      dt_i[seg_size-ind]= -dt_i[ind];  /* Values up to 2*Pi */
      dt_r[seg_size-ind]=dt_r[ind];
    }
  else
    for (ind=1;ind<seg_size/2;ind++)
      dt_i[ind]=dt_r[seg_size-ind];   /* Values up to Pi */

  dt_i[0]=0.0;
  dt_i[seg_size/2]=0.0;

}

/*--------------------------------------------------------------------------*/

void Analysis::ts_trend(float *an_dat,int an_pts,int trend_flag)
/*
 Routine to remove trend from segment of time series before Fourier
 transform. Two types of trend removal:
 1) Mean of series removed.
 2) Linear trend removal based on model y=a+bx.

Input variables
  an_dat        :Pointer to first data sample in segment.
  an_pts        :Number of samples in segment.
  trend_flag    :Type of trend to remove - 0: Mean,  1: y=a+bx model.

 Changed on output
   an_dat       :Contains detrended data. Original values lost.
*/

{        
  int   ind;
  float a,b,av;

/*
 Local variables
    ind          :Array index counter.
    a,b          :Coefficients for linear fit to data y=a+b*x.
    av           :Mean value.
*/

  if (trend_flag==1) {
    fit(an_dat,an_pts,&a,&b);
    for (ind=0;ind<an_pts;ind++)
      an_dat[ind]-=a+b*(float)ind;
  }
  else {
    for (ind=0,av=0.0;ind<an_pts;ind++)
      av+=an_dat[ind];
    av/=(float)an_pts;
    for (ind=0;ind<an_pts;ind++)
      an_dat[ind]-=av;
  }

}

/*---------------------------------------------------------------------------*/

void Analysis::rvfft(float *x,int n)

/*
 Real valued, in place, split-radix FFT subroutine, using decimation in
 time, and cos/sin in second loop. Ref: Sorensen, HV, Jones, DL, Heideman,
 MT and Burrus CS,  Real-Valued FFT Algorithms, IEEE Trans ASSP, 35(6), 1987, 
 pp849-863. This routine is C translation of Fortran listing pp858-860 
 in above reference.

 Input:     x   :Real valued sequence, contains estimate of DFT on output.
            n   :Sequence length - MUST be power of 2, (unchanged).

 Output ordering:       (Re[0],Re[1],...,Re[n/2],Im[n/2-1],...,Im[2],Im[1])
 Corresponding index:   (   0,    1, ...,   n/2,    n/2+1, ...,  n-2,  n-1).
 Routine uses single offset pointer: x_1, to access correct data elements based
 on 1->n Fortran indexing scheme in original routine.
*/

{
  int   i,id,is,i0,i1,i2,i3,i4,i5,i6,i7,i8;
  int   j,k,m,n1,n2,n4,n8;
  float a,a3,cc1,cc3,e,r1,sq2;
  float ss1,ss3,t1,t2,t3,t4,t5,t6,xt;
  float *x_1=x-1;
  j=1;
  n1=n-1;
  for (i=1;i<=n1;i++) {
    if (i<j) {
      xt=x_1[j];
      x_1[j]=x_1[i];
      x_1[i]=xt;
    }
    k=n/2;
    while (k<j) {
      j-=k;
      k/=2;
    }
    j+=k;
  }
  is=1;
  id=4;
  do {
    for (i0=is;i0<=n;i0+=id) {
      i1=i0+1;
      r1=x_1[i0];
      x_1[i0]=r1+x_1[i1];
      x_1[i1]=r1-x_1[i1];
    }
    is=2*id-1;
    id*=4;
  } while (is<n);
  n2=2;
  sq2=static_cast<float>(1.41421356);
  m=(int)((log((float)n)/log(2.0))+0.5);
  for (k=2;k<=m;k++) {
    n2*=2;
    n4=n2/4;
    n8=n2/8;
    e=6.2831853/n2;
    is=0;
    id=n2*2;
    do {
      for (i=is;i<n;i+=id) {
	i1=i+1;
	i2=i1+n4;
	i3=i2+n4;
	i4=i3+n4;
	t1=x_1[i4]+x_1[i3];
	x_1[i4]=x_1[i4]-x_1[i3];
	x_1[i3]=x_1[i1]-t1;
	x_1[i1]=x_1[i1]+t1;
	if (n4>1) {
	  i1+=n8;
	  i2+=n8;
	  i3+=n8;
	  i4+=n8;
	  t1=(x_1[i3]+x_1[i4])/sq2;
	  t2=(x_1[i3]-x_1[i4])/sq2;
	  x_1[i4]=x_1[i2]-t1;
	  x_1[i3]=-x_1[i2]-t1;
	  x_1[i2]=x_1[i1]-t2;
	  x_1[i1]=x_1[i1]+t2;
	}
      }
      is=2*id-n2;
      id*=4;
    } while (is<n);
    a=e;
    for (j=2;j<=n8;j++) {
      a3=3*a;
      cc1=cos(a);
      ss1=sin(a);
      cc3=cos(a3);
      ss3=sin(a3);
      a=j*e;
      is=0;
      id=2*n2;
      do {
	for (i=is;i<n;i+=id) {
	  i1=i+j;
	  i2=i1+n4;
	  i3=i2+n4;
	  i4=i3+n4;
	  i5=i+n4-j+2;
	  i6=i5+n4;
	  i7=i6+n4;
	  i8=i7+n4;
	  t1=x_1[i3]*cc1+x_1[i7]*ss1;
	  t2=x_1[i7]*cc1-x_1[i3]*ss1;
	  t3=x_1[i4]*cc3+x_1[i8]*ss3;
	  t4=x_1[i8]*cc3-x_1[i4]*ss3;
	  t5=t1+t3;
	  t6=t2+t4;
	  t3=t1-t3;
	  t4=t2-t4;
	  t2=x_1[i6]+t6;
	  x_1[i3]=t6-x_1[i6];
	  x_1[i8]=t2;
	  t2=x_1[i2]-t3;
	  x_1[i7]=-x_1[i2]-t3;
	  x_1[i4]=t2;
	  t1=x_1[i1]+t5;
	  x_1[i6]=x_1[i1]-t5;
	  x_1[i1]=t1;
	  t1=x_1[i5]+t4;
	  x_1[i5]=x_1[i5]-t4;
	  x_1[i2]=t1;
	}
	is=2*id-n2;
	id*=4;
      } while (is<n);
    }
  }
}

/*---------------------------------------------------------------------------*/

void Analysis::irvfft(float *x,int n)

/*
 Real valued, in place, split-radix IFFT subroutine. Hermitian symmetric
 input and real output in array x. Routine uses decimation in
 time, and cos/sin in second loop. Ref: Sorensen, HV, Jones, DL, Heideman,
 MT and Burrus CS,  Real-Valued FFT Algorithms, IEEE Trans ASSP, 35(6), 1987, 
 pp849-863. This routine is C translation of Fortran listing pp860-862
 in above reference.

 Input:     x   :Hermitian frequency component series.
            n   :Sequence length - MUST be power of 2, (unchanged).

 Input  ordering:       (Re[0],Re[1],...,Re[n/2],Im[n/2-1],...,Im[2],Im[1])
 Corresponding index:   (   0,    1, ...,   n/2,    n/2+1, ...,  n-2,  n-1).
 Routine uses single offset pointer: x_1, to access correct data elements based
 on 1->n Fortran indexing scheme in original routine.
 NB: Factor 1/n NOT included in this version of transform.
*/

{
  int   i,id,is,i0,i1,i2,i3,i4,i5,i6,i7,i8;
  int   j,k,m,n1,n2,n4,n8;
  float a,a3,cc1,cc3,e,r1,sq2;
  float ss1,ss3,t1,t2,t3,t4,t5,xt;
  float *x_1=x-1;
  n2=2*n;
  sq2=static_cast<float>(1.41421356);
  m=(int)((log((float)n)/log(2.0))+1.0e-06);
  for (k=1;k<m;k++) {
    is=0;
    id=n2;
    n2/=2;
    n4=n2/4;
    n8=n4/2;
    e=6.2831853/n2;
    do {
      for (i=is;i<n;i+=id) {
	i1=i+1;
	i2=i1+n4;
	i3=i2+n4;
	i4=i3+n4;
	t1=x_1[i1]-x_1[i3];
	x_1[i1]+=x_1[i3];
	x_1[i2]*=2;
	x_1[i3]=t1-2*x_1[i4];
	x_1[i4]=t1+2*x_1[i4];
	if (n4>1) {
	  i1+=n8;
	  i2+=n8;
	  i3+=n8;
	  i4+=n8;
	  t1=(x_1[i2]-x_1[i1])/sq2;
	  t2=(x_1[i4]+x_1[i3])/sq2;
	  x_1[i1]+=x_1[i2];
	  x_1[i2]=x_1[i4]-x_1[i3];
	  x_1[i3]=2*(-t2-t1);
	  x_1[i4]=2*(-t2+t1);
	}
      }
      is=2*id-n2;
      id*=4;
    } while (is<n-1);
    a=e;
    for (j=2;j<=n8;j++) {
      a3=3*a;
      cc1=cos(a);
      ss1=sin(a);
      cc3=cos(a3);
      ss3=sin(a3);
      a=j*e;
      is=0;
      id=2*n2;
      do {
	for (i=is;i<n;i+=id) {
	  i1=i+j;
	  i2=i1+n4;
	  i3=i2+n4;
	  i4=i3+n4;
	  i5=i+n4-j+2;
	  i6=i5+n4;
	  i7=i6+n4;
	  i8=i7+n4;
	  t1=x_1[i1]-x_1[i6];
	  x_1[i1]+=x_1[i6];
	  t2=x_1[i5]-x_1[i2];
	  x_1[i5]+=x_1[i2];
	  t3=x_1[i8]+x_1[i3];
	  x_1[i6]=x_1[i8]-x_1[i3];
	  t4=x_1[i4]+x_1[i7];
	  x_1[i2]=x_1[i4]-x_1[i7];
	  t5=t1-t4;
	  t1+=t4;
	  t4=t2-t3;
	  t2+=t3;
	  x_1[i3]=t5*cc1+t4*ss1;
	  x_1[i7]=-t4*cc1+t5*ss1;
	  x_1[i4]=t1*cc3-t2*ss3;
	  x_1[i8]=t2*cc3+t1*ss3;
	}
	is=2*id-n2;
	id*=4;
      } while (is<n-1);
    }
  }
  is=1;
  id=4;
  do {
    for (i0=is;i0<=n;i0+=id) {
      i1=i0+1;
      r1=x_1[i0];
      x_1[i0]=r1+x_1[i1];
      x_1[i1]=r1-x_1[i1];
    }
    is=2*id-1;
    id*=4;
  } while (is<n);
  j=1;
  n1=n-1;
  for (i=1;i<=n1;i++) {
    if (i<j) {
      xt=x_1[j];
      x_1[j]=x_1[i];
      x_1[i]=xt;
    }
    k=n/2;
    while (k<j) {
      j-=k;
      k/=2;
    }
    j+=k;
  }
}

/*---------------------------------------------------------------------------*/

void Analysis::fit(float *y,int ndata,float *a,float *b)

/*
   Routine to fit straight line to series of data points, y, using model
   y = a + bx. x values assumed to be equispaced [0 - (ndata-1)]. Based on
   routine in Press et al., Numerical recipies in C, 14.2, p523. This is
   simplified version using equispaced x values and without any SD for data
   samples. Intended to remove linear trend from time series befor Fourier
   transform.

   Input  y     :Array of data points (Unaltered on exit).
          ndata :Number of points.
 (Output) a,b   :Parameter values of fitted line (Altered on exit).
*/

{
  int i;
  float t,sxoss,sx=0.0,sy=0.0,st2=0.0,ss;

  *b=0.0;
  for (i=0;i<ndata;i++) {
    sx+=(float)i;
    sy+=y[i];
  }
  ss=ndata;
  sxoss=sx/ss;
  for (i=0;i<ndata;i++) {
    t=(float)i-sxoss;
    st2+=t*t;
    *b +=t*y[i];
  }
  *b /=st2;
  *a=(sy-sx*(*b))/ss;
}

/*---------------------------------------------------------------------------*/

void Analysis::an_sdscale(float *data,int pts_tot,float sd_new)

/*
 Routine to scale analogue data to pre-specified Standard Deviation
  1: Existing SD estimated using moment() function.
  2: Data is scaled by new factor: SD/sd_new

 Input    data:  Pointer to data array.
       pts_tot:  Number of points in array.
        sd_new:  Standard deviation required for data array.

 Output   data:  Data is replaced by scaled values, original values lost.

*/
 
{
  int   i;
  float sdev,ave,scale_fac;

/*
 Local Variables
    i          : index counter.
    ave        : Average of data.
    sdev       : Standard Deviation of data.
    scale_fac  : Factor to scale data by to achieve required SD.
*/

  moment(data,pts_tot,&ave,&sdev);
  scale_fac=sd_new/sdev;
  for (i=0;i<pts_tot;i++)
    data[i]*=scale_fac;

}

/*---------------------------------------------------------------------------*/

void Analysis::an_rect_a(float *data,int pts_tot,int rect_type)

/*
 Routine to Rectify analogue signal. Two steps, data is
  1: Converted to zero mean.
  2: Rectified, rectification specified by rect_type
      0: Full wave
      1: Half wave (Bottom half rejected)
      2: Half wave (Top    half rejected)

 Input    data:  Pointer to data array.
       pts_tot:  Number of points in array.

 Output   data:  Data is replaced by rectified values, original values lost.

*/
 
{
  int   i;
  float ave;

/*
 Local Variables
    i          : index counter.
    ave        : Average for rectification.
*/

  ave=0.0;
  for (i=0;i<pts_tot;i++)
    ave+=data[i];
  ave/=(float)pts_tot;
  for (i=0;i<pts_tot;i++)
    data[i]-=ave;

  if (rect_type==0)
    for (i=0;i<pts_tot;i++) {
      if (data[i]<0.0)
        data[i]= -data[i];
    }

  if (rect_type==1) {
    for (i=0;i<pts_tot;i++) {
      if (data[i]<0.0)
        data[i]=0.0;
    }
  }

 /* Modified 08/05/98: invert to +ve values */
  if (rect_type==2) {
    for (i=0;i<pts_tot;i++) {
      if (data[i]>0.0)
        data[i]=0.0;
      else
        data[i]= -data[i];
    }
  }
}

/*--------------------------------------------------------------------------*/

void Analysis::han(float *dat,int dat_pts)

/*
 Routine to apply hanning to data array. Data is smoothed in place, and 
 original values are lost. For data point dat[i] smoothed value is:
    dat[i] = 0.25*dat[i-1] + 0.5*dat[i] + 0.25*dat[i+1].
 First and last values are smoothed with equal weights:
    dat[0] = 0.5*dat[0] + 0.5*dat[1]
    dat[dat_pts-1] = 0.5*dat[dat_pts-2] + 0.5*dat[dat_pts-1]

Input variables
  dat        :Pointer to first data sample in array.
  dat_pts    :Number of samples in array.

 Changed on output
   dat       :Smoothed data.

*/

{        
  int   ind;
  float dat_0,dat_1,dat_2;

/*
 Local variables
    ind                  :Array index counter.
    dat_0,dat_1,dat_2    :Intermediate data value storage.
*/
  dat_0=dat[0];
  dat_1=dat[1];
  dat_2=dat[2];
  dat[0]=0.50*(dat[0]+dat[1]);

  for (ind=1;ind<dat_pts-2;ind++) {
    dat[ind]=0.25*(dat_0+dat_2)+0.5*dat_1;
    dat_0=dat_1;
    dat_1=dat_2;
    dat_2=dat[ind+2];
  }

  dat[dat_pts-2]=0.25*(dat_0+dat_2)+0.5*dat_1;
  dat[dat_pts-1]=0.50*(dat_1+dat_2);
  
}

/*---------------------------------------------------------------------------*/

void Analysis::moment(float *data,int pts_tot,float *ave,float *sdev)

/*
 Routine to estimate mean and standard deviation of an array of data.
 Input    data:  Pointer to data array.
       pts_tot:  Number of points in array.

 Output    ave:  Estimated average value.
          sdev:  Estimated standard deviation.
*/
 
{
  int i;
  float sum,s;

  sum=0.0;
  for (i=0;i<pts_tot;i++)
    sum+=data[i];
  *ave=sum/(float)pts_tot;

  sum=0.0;
  for (i=0;i<pts_tot;i++) {
    s=data[i]-(*ave);
    sum+=(s*s);
  }
  (*sdev)=sqrt((sum)/(float)(pts_tot-1));
}

/*---------------------------------------------------------------------------*/

void Analysis::q12_ft(float *f_r,float *f_i,int seg_size,int seg_size_2,
	    float *cov,float norm_fac)
/*
 Routine to estimate 2nd order covariance through inverse DFT of 2nd order
 cross-spectrum by hermitian IDFT.
 Routine has two sections -
                 1: Set up single hermitian sequence in separate array from
                    real and imaginary input sequences.
                 2: Perform inverse DFT on this array using real valued
                    IFFT routine, and return real sequence,
                    normalizing output if necessary (norm_fac>0).

Input variables
  f_r           :Real part of cross spectrum.
  f_i           :Imaginary part of cross spectrum.
  seg_size      :Number of samples in IDFT and output sequence (power of 2).
  seg_size_2    :seg_size/2, number of values in cross spectral input arrays.
  cov           :Array for output sequence.
  norm_fac      :Factor to normalize output sequence, multiplicative factor.

 Changed on output
   cov          :Real array - IDFT of hermitian input sequence,
                 index range 0 - (seg_size-1).
*/

{        
  int   ind;

/*
 Local variables
    ind          :Array index counter.
*/

  cov[0]=f_r[0];
  cov[seg_size_2]=f_r[seg_size_2];
  for (ind=1;ind<seg_size_2;ind++) {
    cov[ind]=f_r[ind];
    cov[seg_size-ind]=f_i[ind];
    if (ind%2==1) {
      cov[ind]*= -1.0;
      cov[seg_size-ind]*= -1.0;
    }
  }

  irvfft(cov,seg_size);

  if (norm_fac>0.0)
    for (ind=0;ind<seg_size;ind++)
      cov[ind]*=norm_fac;
}

/*---------------------------------------------------------------------------*/

float Analysis::q12_var(float *f11, float *f22,int seg_size,int seg_size_2,long samp_tot)
/*
  Function to return estimated variance of covariance estimate - q21(u).
  Expression to estimate this:
     (2 Pi/R)(2 Pi/T) Sum{ 2*f11[i]*f22[i] : i=0->(T/2-1) }
  Input   f11        :Estimate of f11[i], i=0,...,seg_size_2-1.
          f22        :Estimate of f22[i], i=0,...,seg_size_2-1.
          seg_size   :Segment length, T.
          seg_size_2 :T/2.
          samp_tot   :Record length, R.
 This estimate assumes b=1, ie integration limits Pi/b in inverse DFT to 
 estimate q21(u).
*/
{
  int   i;
  float var,var_fac;

  for (var=0.0,i=0;i<seg_size_2;i++)
    var+=2.0*f11[i]*f22[i];
  var_fac=4.0*pi*pi/((float)seg_size*(float)samp_tot);
  var*=var_fac;
  return(var);
}

/*---------------------------------------------------------------------------*/


int Analysis::an_in_f_d(float **an_dat,int an_pts,ifstream &is, float *ref_data, int an_fac,int sm_pts)

/*
 Function to read in sampled data from file, stored as a binary block of real
 values. This data is expanded, using interpolation, by an Integer factor of
 an_fac (an_fac >= 1). Trend removal done BEFORE interpolation.
 Routine allocates memory dynamically for input and storage of interpolated
 data as necessary. Returns number of points in interpolated data array.
 Trend removal done using smoothed version of original data to estimate
 trend, smoothing determined by sm_pts.

 Variables:   an_dat     :Unassigned pointer for data.
              an_pts     :Number of data points to read in.
              is...C++ ifstream. Must be open!!( XX fin  :Pointer to disk file to read, must be assigned,)
                          assumed to start at first sample.
			  ref_data...array of reference values to be subtracted from data before anything else
              an_fac     :Expansion/Interpolation factor for data.
              sm_pts     :Number of data points to use in moving average
                          estimate of trend.
*/

{
  int   i,in_pts,an_pts_new;
  float *an_tmp,*an_sm;
 
  an_tmp=vector(0,an_pts-1);
  
  is.read( (char *)an_tmp, sizeof(float)*(size_t)an_pts);
  
	if(ref_data)
	{
	  for(i=0;i<an_pts;i++)
	  {
		  an_tmp[i] -= ref_data[i];
	  }
	}

  if (in_pts!=an_pts)
    TRACE("\nWarning: an_in_f_d(), Data input error\n");

  if (sm_pts>1) {
    an_sm=vector(0,an_pts-1);
    sm_av_ncls_a(an_tmp,an_sm,an_pts,sm_pts, static_cast<float>(0.1) );
    for (i=0;i<an_pts;i++)
      an_tmp[i]-=an_sm[i];
    free_vector(an_sm,0);
  }

  if (an_fac>1) {
    an_interpolate(an_tmp,(long)an_pts,an_dat,(long*)&an_pts_new,an_fac);
    free_vector(an_tmp,0);
  }
  else {
    *an_dat=an_tmp;
    an_pts_new=an_pts;
  }

  return(an_pts_new);

}




/*---------------------------------------------------------------------------*/

void Analysis::an_interpolate(float *an_dat,long an_samp_tot,float **an_dat_1,
          long *an_samp_tot_1,int an_int_tot)
/*
 Routine to interpolate analogue data array to one with an_int_tot times
 as many samples.
 Interpolation is done by filling in missing samples with same value and
 smoothing array with a moving average window of (approximately) same
 length as spacing between samples (Spacing between samples (an_int_tot)
 assmued to be even, this gives odd length for smoothing window
 (an_int_tot+1), resulting in symmetrical smoothing).

Input variables
  an_dat        :Pointer to first sample in data array.
  an_samp_tot   :Number of samples in array.
  an_dat_1      :Unassigned pointer for interpolated array.
  an_samp_tot_1 :Pointer for number of samples in interpolated array.
  an_int_tot    :Number of samples to interpolated by.

 Changed on output
  an_dat_1      :Pointer to interpolated array.
  an_samp_tot_1 :Pointer to number of samples in interpolated array.
*/

{        
  int   an_int_count;
  long  an_ind,an_ind_1;
  float *an_temp;

/*
 Local variables
    ind     :Array index counter.
    av      :Mean value of data samples.
*/

  *an_samp_tot_1=an_samp_tot*an_int_tot;
   *an_dat_1=vector_l(0,*an_samp_tot_1-1);
  an_temp=vector_l(0,*an_samp_tot_1-1);

  an_int_count=0;
  an_ind=0;
  for (an_ind_1=0;an_ind_1< *an_samp_tot_1;an_ind_1++) {
    an_temp[an_ind_1]=an_dat[an_ind];
    an_int_count++;
    if (an_int_count==an_int_tot) {
      an_int_count=0;
      an_ind++;
    }
  }

  sm_av_ncls_a(an_temp,*an_dat_1,(int)(*an_samp_tot_1),an_int_tot+1,0.0);
 free_vector(an_temp,0);

}

/*--------------------------------------------------------------------------*/

void Analysis::sm_av_ncls_a(float *dat_in,float *dat_out,int pts_tot,
      int sm_pts,float sm_fac)

/*
 Routine to apply uniform (rectangular window, moving average) smoothing to
 data array. Smoothing is applied to whole array, data is assumed non-circular
 outside array limits - smoothing window is reduced symmetrically at ends of
 array. sm_pts MUST be odd for this routine.
 As sm_av_ncls, except end (sm_fac) fraction of points are set to same value,
 with the value derived from each segment at start and end.

Input variables
  dat_in     :Pointer to first data sample in input array.
  dat_out    :Pointer to first data sample in output array.
  pts_tot    :Total number of pts in array.
  sm_pts     :Number of pts in smoothing window,
              Odd number  - smoothed symmetrically about each point,
              Even number - smoothed about point left of centre line in window.
  sm_fac     :Fraction of points to set constant at start and end of array.

 Changed on output
   dat_out   :Smoothed data, original values ignored.

*/

{        
  int   ind_up,ind_down,ind_dat,ind_sm;
  int   sm_pts_2,sm_count,sm_pts_fac;
  float sm_dat;

/*
 Local variables
    ind_up        :Index for each data point forward in smoothing window.
    ind_down      :Index for each data point backward in smoothing window.
    ind_dat       :Index for scanning data array.
    ind_sm        :Index for scanning smoothing window.
    sm_pts_2      :Number of points each side of centre point in window.
    sm_pts_fac    :Number of points in end segments to set constant.
    sm_count      :Counts number of points used in each averaging,
                   normally sm_pts, but less at ends of array.
    sm_dat        :Smoothed value for each point in data array.
*/

  sm_pts_2=(sm_pts-1)/2;
  sm_pts_fac=(int)((float)sm_pts*sm_fac);
  for (ind_dat=0;ind_dat<pts_tot;ind_dat++) {
    sm_dat=dat_in[ind_dat];
    sm_count=1;
    for (ind_sm=0;ind_sm<sm_pts_2;ind_sm++) {
      ind_up=ind_dat+ind_sm;
      ind_down=ind_dat-ind_sm;
      if (ind_down>=0 && ind_up<pts_tot) {
        sm_dat+=dat_in[ind_up];
        sm_dat+=dat_in[ind_down];
        sm_count+=2;
      }
    }
    sm_dat/=(float)sm_count;
    dat_out[ind_dat]=sm_dat;
  }

  if (sm_pts_fac>1) {
    sm_dat=0.0;
    for (ind_sm=0;ind_sm<sm_pts_fac;ind_sm++)
      sm_dat+=dat_in[ind_sm];
    sm_dat/=(float)sm_pts_fac;
    for (ind_sm=0;ind_sm<sm_pts_fac;ind_sm++)
      dat_out[ind_sm]=sm_dat;

    sm_dat=0.0;
    for (ind_sm=1;ind_sm<=sm_pts_fac;ind_sm++)
      sm_dat+=dat_in[pts_tot-ind_sm];
    sm_dat/=(float)sm_pts_fac;
    for (ind_sm=0;ind_sm<sm_pts_fac;ind_sm++)
      dat_out[pts_tot-ind_sm]=sm_dat;
  }

}

/*---------------------------------------------------------------------------*/

float Analysis::rmagn2(float re,float im)
/*
  Function to return squared magnitude of complex number.
  Input   re:  Real part, Re{}.
          im:  Real part, Im{}.
*/
{
  return(re*re+im*im);
}

/*---------------------------------------------------------------------------*/

float Analysis::frba(float ar,float ai,float br,float bi)
/*
 Function to return real part of cross-spectra estimate between processes
 A and B, Re{Fab}.
 Input   ar   :Real part of DFT of Point process A, Re{daT}.
         ai   :Imaginery part of DFT of Point process A, Im{daT}.
         br   :Real part of DFT of Point process B, Re{dbT}.
         bi   :Imaginery part of DFT of Point process B, Im{dbT}.
*/
{
  return(ar*br+ai*bi);
}

/*---------------------------------------------------------------------------*/

float Analysis::fiba(float ar,float ai,float br,float bi)
/*
 Function to return imaginery part of cross-spectra estimate between processes
 A and B, Im{Fab}.
 Input   ar   :Real part of DFT of Point process A, Re{daT}.
         ai   :Imaginery part of DFT of Point process A, Im{daT}.
         br   :Real part of DFT of Point process B, Re{dbT}.
         bi   :Imaginery part of DFT of Point process B, Im{dbT}.
*/
{
  return(ar*bi-br*ai);
}
 
/*---------------------------------------------------------------------------*/

float Analysis::cab(float ab2,float aa,float bb)
/*
 Function to return coherence estimate between processes A and B, |Rab|**2.
 Input   ab2  :Magnitude of cross spectra between processes A and B squared, 
               |Fab|**2.
         aa   :Autospectra of process A, Faa.
         bb   :Autospectra of process B, Fbb.
*/
{
  return(ab2/(aa*bb));
}

/*---------------------------------------------------------------------------*/





