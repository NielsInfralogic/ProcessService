#pragma once

#include "stdafx.h"

#define MAX_ERRMSG 2048
#define MAXCHANNELSPERPUBLICATION 100

#define LOCKCHECKMODE_NONE	0
#define LOCKCHECKMODE_READ	1
#define LOCKCHECKMODE_READWRITE	2
#define LOCKCHECKMODE_RANGELOCK	3


#define SERVICETYPE_PLANIMPORT 1
#define SERVICETYPE_FILEIMPORT 2
#define SERVICETYPE_PROCESSING 3
#define SERVICETYPE_EXPORT     4
#define SERVICETYPE_DATABASE   5
#define SERVICETYPE_FILESERVER 6
#define SERVICETYPE_MAINTENANCE 7


#define SERVICESTATUS_STOPPED 0
#define SERVICESTATUS_RUNNING 1
#define SERVICESTATUS_HASERROR 2

#define POLLINPUTTYPE_LOWRESPDF 0
#define POLLINPUTTYPE_HIRESPDF 1
#define POLLINPUTTYPE_PRINTPDF 2

#define CHANNELSTATUSTYPE_INPUT 0
#define CHANNELSTATUSTYPE_PROCESSING 1
#define CHANNELSTATUSTYPE_EXPORT 2 
#define CHANNELSTATUSTYPE_MERGE 3

#define PROOFSTATUSNUMBER_PROOFING   5
#define PROOFSTATUSNUMBER_PROOFERROR 6
#define PROOFSTATUSNUMBER_PROOFED   10

#define CONVERTSTATUSNUMBER_CONVERTING   5
#define CONVERTSTATUSNUMBER_CONVERTERROR 6
#define CONVERTSTATUSNUMBER_CONVERTED   10

#define PROOFOUTPUT_SOFT				0	
#define PROOFOUTPUT_HARD				1
#define PROOFOUTPUT_SOFT_HARD			2

#define HARDPROOF_PRINTER				0
#define HARDPROOF_FILE					1

#define ROTATION_NONE					0
#define ROTATION_90						1
#define ROTATION_180					2
#define ROTATION_270					3		

#define ICCRENDERING_PERCEPTUAL				0
#define ICCRENDERING_RELATIVECOLORIMETRIC	1
#define ICCRENDERING_SATURATION				2
#define ICCRENDERING_ABSOLUTECOLORIMETRIC	3

#define	RESAMPLINGMETHOD_BOXFILTER		0
#define	RESAMPLINGMETHOD_BILIEARFILTER	1
#define	RESAMPLINGMETHOD_BICUBICFILTER	2
#define	RESAMPLINGMETHOD_BLACKMANFILTER	3
#define	RESAMPLINGMETHOD_HAMMINGFILTER	4
#define RESAMPLINGMETHOD_BOX			0
#define RESAMPLINGMETHOD_BICUBIC		1
#define RESAMPLINGMETHOD_HARMANN		2

// Ghostscript image generation;Acrobat preview;RIP via external RIP;
#define PDFPROOFMETHOD_GHOSTSCRIPT 0
#define PDFPROOFMETHOD_CALLAS		1
#define PDFPROOFMETHOD_EXTERNALRIP 2

#define READVIEWERROR_RETRIES	3


#define	PROOFTRIMFRAMEMODE_PAGEFORMAT	0
#define	PROOFTRIMFRAMEMODE_BBOX			1

#define READVIEWCROPFORMAT_LAYOUT		0
#define READVIEWCROPFORMAT_PAGEFORMAT	1
#define READVIEWCROPFORMAT_PAGE			2

#define EVENTCODE_PROOFERROR 16
#define EVENTCODE_PROOFED 20


#define EVENTCODE_CONVERTED_LOWRES		110
#define EVENTCODE_CONVERTERROR_LOWRES		116


#define EVENTCODE_CONVERTED_PRINT		120
#define EVENTCODE_CONVERTERROR_PRINT	126

#define FILESERVERTYPE_MAIN			1
#define FILESERVERTYPE_LOCAL		2
#define FILESERVERTYPE_OUTPUTCENTER	3
#define FILESERVERTYPE_INKCENTER	4
#define FILESERVERTYPE_WEBCENTER	5
#define FILESERVERTYPE_BACKUPSERVER	6
#define FILESERVERTYPE_PDFSERVER	7
#define FILESERVERTYPE_PROOFCENTER	8
#define FILESERVERTYPE_ADDITIONALPREVIEW	9
#define FILESERVERTYPE_ADDITIONALTHUMBNAIL	10
#define FILESERVERTYPE_ADDITIONALREADVIEWPREVIEW	11


#define MAILTYPE_DBERROR		0
#define MAILTYPE_FILEERROR	1
#define MAILTYPE_FOLDERERROR 2

#define MAILTYPE_TXERROR		4
#define MAILTYPE_POLLINGERROR		5
#define MAILTYPE_PROOFINGERROR 6

typedef struct  {
	CString	sFileName;
	CString	sFolder;
	CTime	tJobTime;
	CTime	tWriteTime;
	DWORD	nFileSize;
} FILEINFOSTRUCT;

typedef CList <FILEINFOSTRUCT, FILEINFOSTRUCT&> FILELISTTYPE;

class REGEXPSTRUCT {
public:
	REGEXPSTRUCT() {
		m_useExpression = FALSE;
		m_matchExpression = _T("");
		m_formatExpression = _T("");
		m_partialmatch = FALSE;
		m_comment = _T("");
	};

	BOOL		m_useExpression;
	CString		m_matchExpression;
	CString		m_formatExpression;
	BOOL		m_partialmatch;
	CString		m_comment;
};

typedef CArray <REGEXPSTRUCT, REGEXPSTRUCT&> REGEXPLIST;

/////////////////////////////////////////
// Static strings for setup
/////////////////////////////////////////

static CString sHardproofFormats[] = { _T("TIFF"),_T("PDF"),_T("PostScript/EPS"),_T("DCS"),_T("HP-RTL"),_T("HP-PCL"),_T("Epson ESC/P2"),_T("CIP3") };
static CString sICCRenderingIntent[] = { _T("Perceptual"),_T("Relative Colorimetric"),_T("Saturation"),_T("Absolute Colorimetric") };
static CString sProofTypes[] = { _T("Softproof"),_T("Hardproof"),_T("File") };

/////////////////////////////////////////
//
// Structure and list collection definitions
//
/////////////////////////////////////////

class PROOFPROCESSSTRUCT {
public:
	PROOFPROCESSSTRUCT() {
		m_ID = 0;
		m_proofname = _T("");
		m_incrementalproof = FALSE;
		m_waitforblack = TRUE;
		m_colorwaittime = 0;
		m_templateID = 0;
		m_generateforflash = FALSE;
		m_generatereadview = FALSE;
		m_readviewpageformatcrop = 0;
		m_showframe = FALSE;
		m_showframemode = 0;

		m_showimageareaframe = FALSE;
		m_rotateoddpages = FALSE;
		m_writepagecomment = FALSE;
		m_generateheatmap = FALSE;
		m_colorformonosubeditions = FALSE;
		m_alwayssameversion = FALSE;

		m_maxinkwarnratio = 1;

		resample.m_invert = FALSE;
		resample.m_mirror = FALSE;
		resample.m_rotate = 0;
		resample.m_grayscale = FALSE;
		resample.m_resolution = 72.0;
		resample.m_resamplingmethod = 0;
		resample.m_resamplingmethodblack = 0;
		resample.m_specialmethodblack = FALSE;
		resample.m_writeeachcolor = FALSE;
		resample.m_maxinkwarning = FALSE;
		resample.m_maxinklimit = 250;

		output.m_outputtype = HARDPROOF_FILE;
		output.m_softproof = TRUE;
		output.m_hardproof = FALSE;
		output.m_jpegquality = 100;
		output.m_cmykjpeg = FALSE;
		output.m_jpegthumbnail = TRUE;
		output.m_thumbnailresolution = 10;
		output.m_printerdriver = 0;
		output.m_outputfileformat = 0;
		output.m_outputfolder = _T("");
		output.m_postcommand = _T("");
		output.m_useboundingbox = FALSE;
		output.m_boundingboxsizex = 0.0;
		output.m_boundingboxsizey = 0.0;
		output.m_boundingboxposx = 0.0;
		output.m_boundingboxposy = 0.0;
		icc.m_icc_enable = FALSE;
		icc.m_icc_input_profile = _T("");
		icc.m_icc_monitor_profile = _T("");
		icc.m_icc_printer_profile = _T("");
		icc.m_icc_rendering = 0;
		icc.m_icc_enable_proof = FALSE;
		icc.m_icc_proof_profile = _T("");
		icc.m_icc_proof_rendering = 0;
		icc.m_icc_out_of_gamut = FALSE;
		linearization.m_enablelinearization = FALSE;
		linearization.m_LUTfile = _T("LUT.ini");
		filter.m_usefilter = FALSE;

		output.m_printresolution = 300.0;

		output.m_colorconfig = 0;
		output.m_pixelformat = 0;
		output.m_compression = 0;
		output.m_dithering = 0;
		output.m_interleaved = FALSE;

		output.m_ps_asci85 = TRUE;
		output.m_ps_autoscale = FALSE;
		output.m_ps_level2 = TRUE;
		output.m_ps_bottommargin = 0.0;
		output.m_ps_leftmargin = 0.0;
		output.m_ps_colorconfig = 0;
		output.m_ps_compression = 0;
		output.m_ps_scalefactor = 100.0;
		output.m_ps_headerfile = _T("");
		output.m_ps_includeheader = FALSE;
		output.m_ps_papersize = 0;
		output.m_ps_avoiddeadzone = TRUE;
		output.m_ps_traynumber = 0;
		output.m_ps_generateeps = FALSE;
		output.m_ps_includepreview = FALSE;
		output.m_ps_previewformat = 0;
		output.m_ps_previewresolution = 0;



		output.m_dcs_format = 5;
		output.m_dcs_encoding = 0;
		output.m_dcs_compression = 1;
		output.m_dcs_previewformat = 0;
		output.m_dcs_previewtype = 0;
		output.m_dcs_previewres = 72;

		output.m_pcl_colorconfig = 1;
		output.m_pcl_papersize = 0;
		output.m_pcl_papertray = 0;
		output.m_pcl_papertype = 0;
		output.m_pcl_leftmargin = 0.0;
		output.m_pcl_topmargin = 0.0;
		output.m_pcl_scalefactor = 100.0;
		output.m_pcl_autoscale = FALSE;
		output.m_pcl_modeltype = 2;
		output.m_pcl_unidirectional = FALSE;
		output.m_pcl_density = 1.0;
		output.m_pcl_gamma = 0.615;

		output.m_rtl_scalingfactor = 100.0;
		output.m_rtl_plottermargins = 0;
		output.m_rtl_plotterarea = 0;
		output.m_rtl_plotterresolution = 0;
		output.m_rtl_compression = 1;
		output.m_rtl_gamma = 0.615;

		output.m_gammacorrection = 0.0;
		output.m_usegammacorrection = FALSE;

		m_usedensitymask = FALSE;
		m_waitforcmyk = FALSE;
	};

	int m_ID;
	CString m_proofname;
	BOOL m_incrementalproof;
	int	 m_colorwaittime;
	int	 m_templateID;
	BOOL m_waitforblack;
	BOOL m_generateforflash;
	BOOL m_generatereadview;
	int	 m_readviewpageformatcrop;
	BOOL m_showframe;
	int		m_showframemode;
	BOOL	m_generateheatmap;

	BOOL	m_showimageareaframe;
	BOOL	m_rotateoddpages;

	BOOL	m_writepagecomment;
	BOOL	m_colorformonosubeditions;
	BOOL	m_alwayssameversion;

	BOOL	m_usedensitymask;
	BOOL	m_waitforcmyk;

	int		m_maxinkwarnratio;

	struct resample {
		BOOL	m_invert;
		BOOL	m_mirror;
		int		m_rotate;
		BOOL	m_grayscale;
		double	m_resolution;
		int		m_resamplingmethod;
		int		m_resamplingmethodblack;
		BOOL	m_specialmethodblack;
		BOOL	m_writeeachcolor;
		BOOL	m_maxinkwarning;
		int		m_maxinklimit;
	} resample;

	struct output {
		double m_printresolution;
		int m_outputtype;
		BOOL m_softproof;
		BOOL m_hardproof;
		UINT m_jpegquality;
		BOOL	m_cmykjpeg;
		BOOL m_jpegthumbnail;
		int m_thumbnailresolution;
		int m_printerdriver;
		int m_outputfileformat;
		CString m_outputfolder;
		CString m_postcommand;
		BOOL	m_useboundingbox;
		double m_boundingboxsizex;
		double m_boundingboxsizey;
		double m_boundingboxposx;
		double m_boundingboxposy;

		// TIFF-proof
		int  m_colorconfig;
		int  m_pixelformat;
		int  m_compression;
		int	 m_dithering;
		BOOL m_interleaved;

		// PS-proof
		BOOL	m_ps_asci85;
		BOOL	m_ps_autoscale;
		BOOL	m_ps_level2;
		double	m_ps_bottommargin;
		double	m_ps_leftmargin;
		int		m_ps_colorconfig;
		int		m_ps_compression;
		double	m_ps_scalefactor;
		CString m_ps_headerfile;
		BOOL	m_ps_includeheader;
		int		m_ps_papersize;
		BOOL	m_ps_avoiddeadzone;
		int		m_ps_traynumber;
		BOOL	m_ps_generateeps;
		BOOL	m_ps_includepreview;
		int		m_ps_previewformat;
		int		m_ps_previewresolution;

		// DCS-proof		
		int m_dcs_format;
		int m_dcs_encoding;
		int m_dcs_compression;
		int m_dcs_previewformat;
		int m_dcs_previewtype;
		int m_dcs_previewres;


		int m_pcl_colorconfig;
		int m_pcl_papersize;
		int m_pcl_papertray;
		int m_pcl_papertype;
		double m_pcl_leftmargin;
		double m_pcl_topmargin;
		double m_pcl_scalefactor;
		BOOL m_pcl_autoscale;
		int m_pcl_modeltype;
		BOOL m_pcl_unidirectional;
		double m_pcl_density;
		double m_pcl_gamma;


		double m_rtl_scalingfactor;
		int m_rtl_plottermargins;
		int m_rtl_plotterarea;
		int m_rtl_plotterresolution;
		int m_rtl_compression;
		double m_rtl_gamma;

		BOOL	m_usegammacorrection;
		double	m_gammacorrection;


	} output;

	struct icc {
		BOOL	m_icc_enable;
		CString m_icc_input_profile;
		CString m_icc_monitor_profile;
		CString m_icc_printer_profile;
		CString m_icc_proof_profile;
		int		m_icc_rendering;
		int		m_icc_proof_rendering;
		BOOL	m_icc_enable_proof;
		BOOL	m_icc_out_of_gamut;
	} icc;

	struct linearization {
		BOOL	m_enablelinearization;
		CString m_LUTfile;
	} linearization;

	struct filter {
		BOOL	m_usefilter;
		BOOL	m_filterblackonly;
		double	m_filtercoeff[25];
	} filter;
};

typedef CArray <PROOFPROCESSSTRUCT, PROOFPROCESSSTRUCT&> PROOFPROCESSLIST;



class ALIASSTRUCT {
public:
	ALIASSTRUCT() { sType = _T("Color"); sLongName = _T(""); sShortName = _T(""); };
	CString	sType;
	CString sLongName;
	CString sShortName;
};
typedef CArray <ALIASSTRUCT, ALIASSTRUCT&> ALIASLIST;



typedef struct ITEMSTRUCTTAG {
	int m_ID;
	CString m_name;
} ITEMSTRUCT;

typedef CArray <ITEMSTRUCT, ITEMSTRUCT&> ITEMLIST;


class STATUSSTRUCT {
public:
	STATUSSTRUCT() { m_statusnumber = 0; m_statusname = _T(""); m_statuscolor = _T("FFFFFF"); };
	int	m_statusnumber;
	CString m_statusname;
	CString m_statuscolor;
};

typedef CArray <STATUSSTRUCT, STATUSSTRUCT&> STATUSLIST;

typedef struct {
	int m_channelID;
	int m_pushtrigger;
	int m_pubdatemovedays;
	int m_releasedelay;

} PUBLICATIONCHANNELSTRUCT;



class PUBLICATIONSTRUCT {
public:
	PUBLICATIONSTRUCT() {
		m_name = _T("");
		m_PageFormatID = 0;
		m_TrimToFormat = FALSE;
		m_LatestHour = 0.0;
		m_ProofID = 1;
		m_Approve = FALSE;
		m_autoPurgeKeepDays = 0;
		m_emailrecipient = _T("");
		m_emailCC = _T("");
		m_emailSubject = _T("");
		m_emailBody = _T("");
		m_customerID = 0;
		m_uploadfolder = _T("");
		m_deadline = CTime(1975, 1, 1);

		m_allowunplanned = TRUE;
		m_priority = 50;
		m_annumtext = _T("");
		m_releasedays = 0;
		m_releasetimehour = 0;
		m_releasetimeminute = 0;
		m_pubdatemove = FALSE;
		m_pubdatemovedays = 0;
		m_alias = _T("");
		m_outputalias = _T("");
		m_extendedalias = _T("");
		m_publisherID = 0;
		m_numberOfChannels = 0;
		for (int i = 0; i < MAXCHANNELSPERPUBLICATION; i++)
			m_channels[i].m_channelID = 0;

	};

	int			m_ID;
	CString		m_name;
	int			m_PageFormatID;
	int			m_TrimToFormat;
	double		m_LatestHour;
	int			m_ProofID;

	int			m_Approve;

	int			m_autoPurgeKeepDays;
	CString		m_emailrecipient;
	CString		m_emailCC;
	CString		m_emailSubject;
	CString		m_emailBody;
	int			m_customerID;
	CString		m_uploadfolder;
	CTime		m_deadline;

	//	CString		m_channelIDList;

	BOOL		m_allowunplanned;
	int			m_priority;

	CString		m_annumtext;

	int			m_releasedays;
	int			m_releasetimehour;
	int			m_releasetimeminute;


	BOOL		m_pubdatemove;
	int  		m_pubdatemovedays;
	CString		m_alias;
	CString		m_outputalias;
	CString		m_extendedalias;

	int			m_publisherID;

	int			m_numberOfChannels;
	PUBLICATIONCHANNELSTRUCT m_channels[MAXCHANNELSPERPUBLICATION];

};

typedef CArray <PUBLICATIONSTRUCT, PUBLICATIONSTRUCT&> PUBLICATIONLIST;


class CHANNELSTRUCT {
public:
	CHANNELSTRUCT() {
		m_channelID = 0;
		m_channelname = _T("");
		m_enabled = TRUE;
		m_mergePDF = FALSE;
		m_pdftype = POLLINPUTTYPE_LOWRESPDF;
		m_channelnamealias = _T("");
		m_pdfprocessID = 0;

	};

	int		m_channelID;
	CString m_channelname;

	BOOL	m_enabled;

	int		m_pdftype;
	BOOL	m_mergePDF;

	CString m_channelnamealias;

	int     m_pdfprocessID;
};

typedef CArray <CHANNELSTRUCT, CHANNELSTRUCT&> CHANNELLIST;


typedef struct  {
	CString m_servername;
	CString m_sharename;
	int	m_servertype;
	CString m_IP;
	CString m_username;
	CString m_password;
	int	m_locationID;
	BOOL m_uselocaluser;
} FILESERVERSTRUCT;

typedef CArray <FILESERVERSTRUCT, FILESERVERSTRUCT&> FILESERVERLIST;

typedef struct  {
	CString m_sJobName;
	CString m_sColor;
	int		m_nProofID;
	int		m_nInputID;
	int		m_nStatus;
	int		m_nApproved;
	CString m_sLocationName;
} NextProofJob;

typedef struct  {
	int		m_PublicationID;
	int		m_ProofID;
} PUBLICATIONPROOFSTRUCT;

typedef CArray <PUBLICATIONPROOFSTRUCT, PUBLICATIONPROOFSTRUCT&> PUBLICATIONPROOFSTRUCTLIST;


typedef struct {
	int m_ProcessID;
	CString m_ProcessName;
	int m_ProcessType;
	CString m_ConvertProfile;
	BOOL m_ExternalProcess;
	CString m_ExternalInputFolder;
	CString m_ExternalOutputFolder;
	CString m_ExternalErrorFolder;
	int m_ProcessTimeOut;
} PDFPROCESSTYPE;

typedef CArray <PDFPROCESSTYPE, PDFPROCESSTYPE&> PDFPROCESSTYPELIST;

class CJobAttributes {
public:
	CJobAttributes() {};

	//~CJobAttributes() {
		//m_pagename.Empty();
	//};



	void Init()
	{
		m_planname = "";
		m_ccdatafilename = "";

		m_sectionID = 0;
		m_editionID = 0;
		m_pressrunID = 0;
		m_locationID = 0;
		m_publicationID = 0;
		m_templateID = 0;
		m_prooftemplateID = 0;
		m_copies = 1;

		m_deviceID = 0;
		m_copynumber = 1;
		m_pagename = _T("1");
		m_colorname = _T("K");
		CTime t = CTime::GetCurrentTime();	// Default to neext day
		t += CTimeSpan(1, 0, 0, 0);

		m_pubdate = t;
		m_auxpubdate = t;
		m_pubdateweekstart = t;
		m_pubdateweekend = t;
		m_hasweekpubdate = FALSE;

		m_version = 0;

		m_mastercopyseparationset = 1;
		m_copyseparationset = 1;
		m_separationset = 101;
		m_separation = 10101;
		m_copyflatseparationset = 1;
		m_flatseparationset = 101;
		m_flatseparation = 10101;

		m_numberofcolors = 1;
		m_colornumber = 1;

		m_layer = 1;
		m_filename = _T("");
		m_pagination = 0;
		m_pageindex = 0;

		m_priority = 50;
		m_comment = _T("");
		m_pageposition = 1;
		m_pagetype = 0;
		m_pagesonplate = 1;
		m_hold = FALSE;
		m_approved = FALSE;
		m_active = TRUE;

		m_status = 0;
		m_externalstatus = 0;
		CTime t2;
		m_deadline = t2;
		m_pressID = 0;
		m_presssectionnumber = 0;
		m_sortnumber = 0;
		m_pubdatefound = FALSE;

		m_presstower = _T("1");
		m_presszone = _T("1");
		m_presscylinder = _T("1");
		m_presshighlow = _T("H");
		m_productionID = 0;
		m_inputID = 0;
		m_jobname = _T("");
		m_markgroup = _T("");
		m_includemarkgroup = 0;
		m_currentmarkgroup = _T("");

		m_commoneditionID = 0;

		m_originalfilename = _T("");

		m_previousstatus = 0;
		m_previousactive = TRUE;
		m_pressruncomment = "";
		m_pressruninkcomment = "";
		m_pressrunordernumber = "";

		m_customername = _T("");
		m_customeralias = _T("");
		m_customeremail = _T("");
		m_originalmastercopyseparationset = 0;
		m_panomate = _T("");
		m_miscstring1 = _T("");
		m_miscstring2 = _T("");
		m_miscstring3 = _T("");

		m_locationgroupIDList = _T("");
		m_locationgroup = _T("");

		m_publisherID = 0;

		m_annumtext = _T("");

		m_pdfprocessID = 0;


	};

	CString m_planname;
	CString m_ccdatafilename;

	CTime	m_pubdate;
	CTime	m_auxpubdate;
	CTime	m_pubdateweekstart;
	CTime	m_pubdateweekend;
	BOOL	m_hasweekpubdate;

	CString	m_pagename;	// page number!
	CString m_colorname;
	int		m_publicationID;
	int		m_sectionID;
	int		m_editionID;
	int		m_pressrunID;
	int		m_templateID;
	int		m_deviceID;
	int		m_prooftemplateID;
	int		m_locationID;
	int		m_pressID;
	int		m_inputID;
	int		m_pressgroupID;

	int		m_copies;
	int		m_copynumber;
	int		m_version;
	int		m_layer;
	CString m_filename;
	int		m_pagination;
	int		m_pageindex;
	int		m_priority;
	CString	m_comment;

	int		m_pageposition;
	int		m_pagetype;
	int		m_pagesonplate;
	int		m_colornumber;
	int		m_numberofcolors;

	BOOL	m_hold;
	int		m_approved;
	BOOL	m_active;

	int		m_sheetnumber;
	int		m_sheetside;

	int		m_mastercopyseparationset;
	int		m_copyseparationset;
	int		m_separationset;
	int		m_separation;
	int		m_copyflatseparationset;
	int		m_flatseparationset;
	int		m_flatseparation;

	int		m_status;
	int		m_externalstatus;
	CTime	m_deadline;
	int		m_presssectionnumber;
	int		m_sortnumber;
	BOOL	m_pubdatefound;

	CString	m_presstower;
	CString	m_presszone;
	CString	m_presscylinder;
	CString	m_presshighlow;

	int		m_productionID;

	int		m_guessedition;
	int		m_guesssection;
	int		m_guessrun;
	int		m_guesslocation;
	CString m_jobname;

	CString m_currentmarkgroup;
	CString m_markgroup;
	int		m_includemarkgroup;

	CString m_polledpublicationname;
	CString m_polledsectionname;
	CString m_pollededitionname;
	CString m_polledissuename;

	int		m_commoneditionID;
	CString m_originalfilename;

	int		m_previousstatus;
	BOOL	m_previousactive;

	int		m_weekreference;

	CString m_pressruncomment;
	CString m_pressruninkcomment;
	CString m_pressrunordernumber;

	CString m_customername;
	CString m_customeralias;
	CString m_customeremail;
	int		m_originalmastercopyseparationset;

	CString m_panomate;
	CString m_locationgroupIDList;
	CString m_locationgroup;

	CString m_miscstring1;
	CString m_miscstring2;
	CString	m_miscstring3;

	int m_publisherID;

	CString m_annumtext;

	int m_pdfprocessID;
};

typedef CArray <CJobAttributes, CJobAttributes&> CJobAttributesList;

#define PDFBOX_MEDIABOX 1
#define PDFBOX_CROPBOX  2
#define PDFBOX_TRIMBOX  3
#define PDFBOX_BLEEDBOX  4
#define PDFBOX_ARTBOX  5


#define SNAP_UL		0
#define SNAP_UC		1
#define SNAP_UR		2

#define SNAP_CL		3
#define SNAP_CENTER	4
#define SNAP_CR		5

#define SNAP_LL		6
#define SNAP_LC		7
#define SNAP_LR		8



typedef struct {
	double fXPos;
	double fYPos;
	double fXSize;
	double fYSize;
} PDFBOX;

typedef struct {
	double fBBoxPosX;
	double fBBoxPosY;
	double fBBoxSizeX;
	double fBBoxSizeY;
	double fPosX;
	double fPosY;
	int    nRotation;
} PDFBOXEX;
