/****************************************************************************/
/*                                                                          */
/* Declaration of types                                                     */
/*                                                                          */
/****************************************************************************/
#ifndef __BUTTON_H
#define __BUTTON_H

struct Parameters{



////////////////////////////CommonDLg
	double	alphaTZ;
	double	alpha;
	int		cycles;
	double	epsilon;
	BOOL	expT;
	CString	fileName;
	BOOL	graphDisp;
	BOOL	autoIniT;
	double	Tzero;
	double	IniT;
	int		nbSubS;
	int		maxNbSubS;
	int		maxNbRetry;
	int		detectionType;
	int		STOP_CY;
	double	stopCyProp;
	double	updateTzero;
	BOOL	monWellCl;
	BOOL	drawBadS;
	BOOL	rnd;
	BOOL	rndOrigin;
	int		retryWithL;
	BOOL	noPtsRmv;
	double	stopTemp;
	double	noCorr;
	BOOL	monT;
	BOOL	monAB;
	CString	result;
	
/////////////////////////LineDetectDlg
	BOOL	bmpOrNot;
	int		color;
	double	noise;
	int		display;
	int		segment;
	int		segThre;
	double	segEpsi;
	int		cluster;
	BOOL	lastStart;
	BOOL	tanEst;
	double	deltaC;
	double	partial;

///////////////////////ConicDetectDlg

	int		detection;
///////////////////////ARmodelingDLg

	int		ORDER_AR;
	BOOL	winnow;
	int		contiguous;
	int		indexMin;
	int		indexMax;
	CString	arOutFile;
};



#endif

