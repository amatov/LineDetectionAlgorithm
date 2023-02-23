// ButtonDoc.cpp : implementation of the CButtonDoc class
//

#include "stdafx.h"
#include "Button.h"
#include <process.h>

#include "ButtonDoc.h"

#include "CommonDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonDoc

IMPLEMENT_DYNCREATE(CButtonDoc, CDocument)

BEGIN_MESSAGE_MAP(CButtonDoc, CDocument)
	//{{AFX_MSG_MAP(CButtonDoc)
	ON_COMMAND(ID_EDIT_CONFIG, OnEditConfig)
	ON_COMMAND(ID_ACTION_START, OnActionStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const BOOL False = 0;
const BOOL True = 1;

/////////////////////////////////////////////////////////////////////////////
// CButtonDoc construction/destruction

CButtonDoc::CButtonDoc()
{
	// TODO: add one-time construction code here


////////////////////////////CommonDLg
	defpars.alphaTZ = 0.4;
	defpars.alpha = 0.001;
	defpars.cycles = 5000;
	defpars.epsilon = 0.1;
	defpars.expT = 0;
	defpars.fileName = "";
	defpars.graphDisp = False;
	defpars.autoIniT = 0;
	defpars.Tzero = 2;
	defpars.IniT = 30;
	defpars.nbSubS = 0;
	defpars.maxNbSubS = 10;
	defpars.maxNbRetry = 3;
	defpars.detectionType = 2;//line detection
	defpars.STOP_CY = 0;
	defpars.stopCyProp = 0;
	defpars.updateTzero = 1.5;
	defpars.monWellCl = 0;
	defpars.drawBadS = 0;
	defpars.rnd = 0;
	defpars.rndOrigin = 0;
	defpars.retryWithL = 1;
	defpars.noPtsRmv = 0;//couldnt find it
	defpars.stopTemp = 300;
	defpars.noCorr = 0;
	defpars.monT = 0;
	defpars.monAB = 0;
	defpars.result = "";//?
	
/////////////////////////LineDetectDlg
	defpars.bmpOrNot = 0;
	defpars.color = 0;
	defpars.noise = 0;
	defpars.display = 0;//mon
	defpars.segment = 0;
	defpars.segThre = 0;
	defpars.segEpsi = 0;
	defpars.cluster = 0;
	defpars.lastStart = 0;
	defpars.tanEst = 0;
	defpars.deltaC = 0;
	defpars.partial = 1;

///////////////////////ConicDetectDlg

	defpars.detection = 0;//radio button
///////////////////////ARmodelingDLg

	defpars.ORDER_AR = 1;
	defpars.winnow = 0;
	defpars.contiguous = 0;
	defpars.indexMin = 0;
	defpars.indexMax = 0;
	defpars.arOutFile = "";
	
}

CButtonDoc::~CButtonDoc()
{
}

BOOL CButtonDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here

	params = defpars;

	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////

CString nextWord(CString& s){
	CString tmp;
	int j;
	do {
		j = s.Find(' ');
		if(j!=-1) {
			tmp = s.Left(j);
			s = s.Right(s.GetLength() - j - 1);
		}
	} while(j!=-1 && tmp.IsEmpty());
	return tmp;
}

// CButtonDoc serialization

void CButtonDoc::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here

		CString str, s, t;
		//int i; 
		while(ar.ReadString(s)) {
			str += s + " ";	
		}

		while( !(s = nextWord(str)).IsEmpty() ) {
//////////////////////////////////////////////Common parameters
			if(s == "-cycles") {
				t = nextWord(str);
				params.cycles = atoi(t);
				}

			if(s == "-nb_subsys") {
				t = nextWord(str);
				params.nbSubS = atoi(t);
			
			}

			if(s == "-max_num_subsys") {
				t = nextWord(str);
				params.maxNbSubS = atoi(t);
				//i = atoi(t);
				//TRACE("max_num_subsys=%d\n",i);
				//AfxMessageBox(t);
			}

			if(s == "-epsilon") {
				t = nextWord(str);
				params.epsilon = atof(t);
				//AfxMessageBox(t);
			}

			if(s == "-no_corr") {
				t = nextWord(str);
				params.noCorr = atof(t);;
				//AfxMessageBox(s);
			}

			if(s == "-stop_cycles") {
				t = nextWord(str);
				params.STOP_CY = atoi(t);
				//AfxMessageBox(t);
			}

			if(s == "-stop_cycles_prop") {
				t = nextWord(str);
				params.stopCyProp = atof(t);
				//AfxMessageBox(t);
			}

			if(s == "-stop_temp") {
				t = nextWord(str);
				params.stopTemp = atof(t);
				//AfxMessageBox(t);
			}
				
			if(s == "-max_retry") {
				t = nextWord(str);
				params.maxNbRetry = atoi(t);
				//AfxMessageBox(t);
			}
			
			if(s == "-retry_with_last") {
				t = nextWord(str);
				params.retryWithL = atoi(t);
				//AfxMessageBox(t);
			}
			
			if(s == "-no_pts_rmv") {
				params.noPtsRmv = True;
				//AfxMessageBox(s);
			}

			if(s == "-rnd") {
				params.rnd = True;
				//AfxMessageBox(s);
			}

			if(s == "-rnd_origin") {
				params.rndOrigin = True;
				//AfxMessageBox(s);
			}
//////////////////////////////////////////////Manual temperature
			if(s == "-T") {
				t = nextWord(str);
				params.IniT = atof(t);
				//AfxMessageBox(t);
			}

			if(s == "-alpha") {
				t = nextWord(str);
				params.alpha = atof(t);
				//AfxMessageBox(t);
			}

			if(s == "-exponential") {
				params.expT = True;
				//AfxMessageBox(s);
			}
////////////////////////////////////////////Automatic temperature
			if(s == "-auto_temp_set") {
				params.autoIniT = True;
				//AfxMessageBox(s);
			}

			if(s == "-tzero") {
				t = nextWord(str);
				params.Tzero = atof(t);
				//AfxMessageBox(t);
			}
		
			if(s == "-update_tz") {
				t = nextWord(str);
				params.updateTzero = atof(t);
				//AfxMessageBox(t);
			}

			if(s == "-alpha_tz") {
				t = nextWord(str);
				params.alphaTZ = atof(t);
				//AfxMessageBox(t);
			}
///////////////////////////////////////////AR modeling parameters
			if(s == "-AR_model") {
				params.detectionType = 0;//? taka li triabva?
				//AfxMessageBox(s);
			}

			if(s == "-winnow") {
				params.winnow = True;
				//AfxMessageBox(s);
			}

			if(s == "-in") {
				params.fileName = nextWord(str);
				
				//AfxMessageBox(t);
			}

			//if(s == "-in2") {
			//	t = nextWord(str);
				//AfxMessageBox(t);
			//}

			if(s == "-index") {
				t = nextWord(str);
				params.indexMin = atoi(t);
				t = nextWord(str);
				params.indexMax = atoi(t);
				//AfxMessageBox(t);
			}

			if(s == "-AR_order") {
				t = nextWord(str);
				params.ORDER_AR = atoi(t);
				//AfxMessageBox(t);
			}

			if(s == "-AR_out_file") {
				params.arOutFile = nextWord(str);
				//AfxMessageBox(t);
			}

			if(s == "-contiguous") {
				t = nextWord(str);
				params.contiguous = atoi(t);
				//AfxMessageBox(t);
			}
////////////////////////////////////////Optical flow modeling (EMPTY)
			if(s == "-optical_flow") {
				//AfxMessageBox(s);
			}

			if(s == "-x") {
				t = nextWord(str);
				//AfxMessageBox(t);
			}

			if(s == "-y") {
				t = nextWord(str);
				//AfxMessageBox(t);
			}

			if(s == "-dim_x") {
				t = nextWord(str);
				//AfxMessageBox(t);
			}

			if(s == "-dim_y") {
				t = nextWord(str);
				//AfxMessageBox(t);
			}

			if(s == "-param") {
				t = nextWord(str);
				//AfxMessageBox(t);
			}
/////////////////////////////////////////////Line detection
			if(s == "-line_detection") {
				params.detectionType = 2;
				//AfxMessageBox(s);
			}
			
			if(s == "-tan_estimation") {
				t = nextWord(str);
				params.tanEst = True;
				//AfxMessageBox(t);
			}

			if(s == "-segment") {
				t = nextWord(str);
				params.segment = atoi(t);
				//AfxMessageBox(t);
			}

			if(s == "-seg_win_threshold") {
				t = nextWord(str);
				params.segThre = atoi(t);
				//AfxMessageBox(t);
			}

			if(s == "-seg_epsilon") {
				t = nextWord(str);
				params.segEpsi = atof(t);
				//AfxMessageBox(t);
			}

			if(s == "-mon") {
				t = nextWord(str);
				params.display = atoi(t);
				//AfxMessageBox(t);
			}

			if(s == "-bmp") {
				params.bmpOrNot = True;
				//AfxMessageBox(s);
			}

			if(s == "-back_color") {
				t = nextWord(str);
				params.color = atoi(t);
				//AfxMessageBox(t);
			}

			if(s == "-bmp_out") {//nisho oshte
				//AfxMessageBox(s);
			}

			if(s == "-cluster") {
				t = nextWord(str);
				params.cluster = atoi(t);
				//AfxMessageBox(t);
			}

			if(s == "-noise") {
				t = nextWord(str);
				params.noise = atof(t);
				//AfxMessageBox(t);
			}

			if(s == "-partial") {
				t = nextWord(str);
				params.partial = atof(t);
				//AfxMessageBox(t);
			}

			if(s == "-delta_c_min") {
				t = nextWord(str);
				params.deltaC = atof(t);
				//AfxMessageBox(t);
			}

			if(s == "-last_as_start") {
				params.lastStart = True;
				//AfxMessageBox(s);
			}
////////////////////////////////////////////////Conic detection(EMPTY)
			if(s == "-conic_detection") {
				//AfxMessageBox(s);
			}

			if(s == "-conic2_detection") {
				//AfxMessageBox(s);
			}
			
			if(s == "-conic3_detection") {
				//AfxMessageBox(s);
			}

			if(s == "-relative_epsilon") {
				t = nextWord(str);
				//AfxMessageBox(t);
			}

			if(s == "-initial_r") {
				t = nextWord(str);
				//AfxMessageBox(t);
			}

			if(s == "-remove_bad_pts") {
			//	AfxMessageBox(s);
			}

			if(s == "-postprocess_mode") {
				t = nextWord(str);
				//AfxMessageBox(t);
			}

			if(s == "-tan_estimation") {
				t = nextWord(str);
				//AfxMessageBox(t);
			}
/////////////////////////////////////////////////
			if(s == "-result") {
				params.result = nextWord(str);
			//	AfxMessageBox(t);
			}

			if(s == "-mon_t") {
				t = nextWord(str);
				params.monT = True;
			//	AfxMessageBox(t);
			}

			if(s == "-mon_ab") {
				t = nextWord(str);
				params.monAB = True;
			//	AfxMessageBox(t);
			}

			if(s == "-syst_out") {//ne
				t = nextWord(str);
				//AfxMessageBox(t);
			}

			if(s == "-syst_in") {//ne
				t = nextWord(str);
			//	AfxMessageBox(t);
			}

			if(s == "-graph") {
				params.graphDisp = True;
			//	AfxMessageBox(s);
			}

			if(s == "-mon_well_classified") {
				params.monWellCl = True;
			//	AfxMessageBox(s);
			}

			if(s == "-draw_bad_subsystem") {
				params.drawBadS = True;
			//	AfxMessageBox(s);
			}
		} // while
	}
}

/////////////////////////////////////////////////////////////////////////////
// CButtonDoc diagnostics

#ifdef _DEBUG
void CButtonDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CButtonDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CButtonDoc commands

void CButtonDoc::OnEditConfig() 
{
	// TODO: Add your command handler code here
	CCommonDlg dlg(params);
	dlg.m_alpha = params.alpha;
	dlg.m_alphaTZ = params.alphaTZ;
	dlg.m_autoIniT = params.autoIniT;
	dlg.m_cycles = params.cycles;
	dlg.m_detectionType = params.detectionType;
	dlg.m_drawBadS = params.drawBadS;
	dlg.m_epsilon = params.epsilon;
	dlg.m_expT = params.expT;
	dlg.m_fileName = params.fileName;
	dlg.m_graphDisp = params.graphDisp;
	dlg.m_IniT = params.IniT;
	dlg.m_maxNbRetry = params.maxNbRetry;
	dlg.m_monAB = params.monAB;
	dlg.m_monT = params.monT;
	dlg.m_monWellCl = params.monWellCl;
	dlg.m_noCorr = params.noCorr;
	dlg.m_noPtsRmv = params.noPtsRmv;
	dlg.m_result = params.result;
	dlg.m_retryWithL = params.retryWithL;
	dlg.m_rnd = params.rnd;
	dlg.m_rndOrigin = params.rndOrigin;
	dlg.m_STOP_CY = params.STOP_CY;
	dlg.m_stopCyProp = params.stopCyProp;
	dlg.m_stopTemp = params.stopTemp;
	dlg.m_Tzero = params.Tzero;
	dlg.m_updateTzero = params.updateTzero;
	dlg.m_maxNbSubS = params.maxNbSubS;
	dlg.m_nbSubS = params.nbSubS;

	int ret = dlg.DoModal();
	
	if (ret == IDOK){
	
	params.alpha = dlg.m_alpha;
	params.alphaTZ = dlg.m_alphaTZ;
	params.autoIniT = dlg.m_autoIniT;
	params.cycles = dlg.m_cycles;
	params.detectionType = dlg.m_detectionType;
	params.drawBadS = dlg.m_drawBadS;
	params.epsilon = dlg.m_epsilon;
	params.expT = dlg.m_expT;
	params.fileName = dlg.m_fileName;
	params.graphDisp = dlg.m_graphDisp;
	params.IniT = dlg.m_IniT;
	params.maxNbRetry = dlg.m_maxNbRetry;
	params.monAB = dlg.m_monAB;
	params.monT = dlg.m_monT;
	params.monWellCl = dlg.m_monWellCl;
	params.noCorr = dlg.m_noCorr;
	params.noPtsRmv = dlg.m_noPtsRmv;
	params.result = dlg.m_result;
	params.retryWithL = dlg.m_retryWithL;
	params.rnd = dlg.m_rnd;
	params.rndOrigin = dlg.m_rndOrigin;
	params.STOP_CY = dlg.m_STOP_CY;
	params.stopCyProp = dlg.m_stopCyProp;
	params.stopTemp = dlg.m_stopTemp;
	params.Tzero = dlg.m_Tzero;
	params.updateTzero = dlg.m_updateTzero;
	params.maxNbSubS = dlg.m_maxNbSubS;
	params.nbSubS = dlg.m_nbSubS;
	};


}

void CButtonDoc::OnActionStart() 
{
	// TODO: Add your command handler code here
	

	CString s = toString();
	CFile tmpFile("e:\\tmp\\tmp.cfg",CFile::modeCreate | CFile::modeWrite );
	tmpFile.Write(s,s.GetLength());
	tmpFile.Close();

	SetCurrentDirectory("e:\\plime_win\\plime_work");//points to the directory of PLIME
 
	int res = _execl("e:\\Plime_win\\Plime_win\\debug\\Plime_win.exe","-F e:\\tmp\\tmp.cfg",NULL);
	//shows the path to plime_win.exe

}

CString CButtonDoc::toString(){
	CString str;
	char buf[20];
	

	switch(params.detectionType){
	case 0:{
///////////////AR modeling
	str += "-AR_model";
	str += "\n";
	str += "\n";

	str+= "-in " + params.fileName + "\n";

	str+= "-AR_out_file " + params.arOutFile + "\n";

	if(params.winnow){
		str += "-winnow ";
		//_itoa(params.winnow,buf,10);
		//str += buf;
		str += "\n";
	}

	if(params.autoIniT != defpars.autoIniT){
		str += "-auto_temp_set ";
		_itoa(params.autoIniT,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.ORDER_AR != defpars.ORDER_AR){
		str += "-AR_order ";
		_itoa(params.ORDER_AR,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.contiguous != defpars.contiguous){
		str += "-contiguous ";
		_itoa(params.contiguous,buf,10);
		str += buf;
		str += "\n";
	}

	}
	break;
	case 1:{//conic detection
		str += "-conic_detection";
		str += "\n";
		   }break;
	case 2:{
	////////Line detection
		str += "-line_detection";
		str += "\n";
	
	int pos = params.fileName.Find(".bmp");
	if(pos == -1 || pos != params.fileName.GetLength()-4)
		str+= "-in " + params.fileName + "\n";
	else
		str+= "-in " + params.fileName.Left(params.fileName.GetLength()-4) + "\n";

	if(params.tanEst){
		str += "-tan_estimation ";
		//_itoa(params.tanEst,buf,10);
		//str += buf;
		str += "\n";
	}

	if(params.segment != defpars.segment){
		str += "-segment ";
		_itoa(params.segment,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.segThre != defpars.segThre){
		str += "-segment_win_threshold ";
		_itoa(params.autoIniT,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.segEpsi != defpars.segEpsi){
		str += "-seg_epsilon ";
		sprintf(buf, "%f", params.segEpsi);
		//_itoa(params.segEpsi,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.display != defpars.display){//Radio button LD
		str += "-mon ";
		_itoa(params.display,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.bmpOrNot){
		str += "-bmp ";
		//_itoa(params.bmpOrNot,buf,10);
		//str += buf;
		str += "\n";
	}

	if(params.color != defpars.color){
		str += "-back_color ";
		_itoa(params.color,buf,10);
		str += buf;
		str += "\n";
	}
	//skip bmp_out
	if(params.cluster != defpars.cluster){
		str += "-cluster ";
		_itoa(params.cluster,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.noise != defpars.noise){
		str += "-noise ";
		sprintf(buf, "%f", params.noise);
		//_itoa(params.noise,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.partial != defpars.partial){
		str += "-partial ";
		sprintf(buf, "%f", params.partial);
		//_itoa(params.partial,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.deltaC != defpars.deltaC){
		str += "-delta_c_min ";
		sprintf(buf, "%f", params.deltaC);
		//_itoa(params.deltaC,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.lastStart){
		str += "-last_as_start ";
		//_itoa(params.lastStart,buf,10);
		//str += buf;
		str += "\n";
	}

		   }break;}//zatvaria switch



		

//	str+= "-result " + params.result + "\n";
	
	if(params.cycles != defpars.cycles){
		str += "-cycles ";
		_itoa(params.cycles,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.maxNbSubS != defpars.maxNbSubS){
		str += "-max_num_subsys ";
		_itoa(params.maxNbSubS,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.nbSubS != defpars.nbSubS){
		str += "-nb_subsys ";
		_itoa(params.nbSubS,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.epsilon != defpars.epsilon){
		str += "-epsilon ";
		//_itoa(params.epsilon,buf,10);
		sprintf(buf, "%f", params.epsilon);
		str += buf;
		str += "\n";
	}

		if(params.noCorr != defpars.noCorr){
		str += "-no_corr ";
		//_itoa(params.stopCyProp,buf,10);
		sprintf(buf, "%f", params.noCorr);
		str += buf;
		str += "\n";
	}

		if(params.STOP_CY != defpars.STOP_CY){
		str += "-stop_cycles ";
		_itoa(params.STOP_CY,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.stopCyProp != defpars.stopCyProp){
		str += "-stop_cycles_prop ";
		//_itoa(params.stopCyProp,buf,10);
		sprintf(buf, "%f", params.stopCyProp);
		str += buf;
		str += "\n";
	}
	if(params.stopTemp != defpars.stopTemp){
		str += "-stop_temp ";
		//_itoa(params.stopTemp,buf,10);
		sprintf(buf, "%f", params.stopTemp);
		str += buf;
		str += "\n";
	}
	if(params.maxNbRetry != defpars.maxNbRetry){
		str += "-max_retry ";
		_itoa(params.cycles,buf,10);
		str += buf;
		str += "\n";
	}
	if(params.retryWithL != defpars.retryWithL){
		str += "-retry_with_last ";
		_itoa(params.retryWithL,buf,10);
		str += buf;
		str += "\n";
	}
	if(params.noPtsRmv){
		str += "-no_pts_rmv ";
		//_itoa(params.noPtsRmv,buf,10);
		//str += buf;
		str += "\n";
	}
	if(params.rnd){
		str += "-rnd ";
		//_itoa(params.rnd,buf,10);
		//str += buf;
		str += "\n";
	}

		if(params.rndOrigin){
		str += "-rnd_origin ";
		//_itoa(params.rnd,buf,10);
		//str += buf;
		str += "\n";
	}
	//manual temp
	if(params.IniT != defpars.IniT){
		str += "-T ";
		//_itoa(params.IniT,buf,10);
		sprintf(buf, "%f", params.IniT);
		str += buf;
		str += "\n";
	}
	if(params.alpha != defpars.alpha){
		str += "-alpha ";
		//_itoa(params.alpha,buf,10);
		sprintf(buf, "%f", params.alpha);
		str += buf;
		str += "\n";
	}
	if(params.expT){
		str += "-exponential ";
		//_itoa(params.expT,buf,10);
		//str += buf;
		str += "\n";
	}
	//automatic temp
	if(params.autoIniT){
		str += "-auto_temp_set ";
		//_itoa(params.autoIniT,buf,10);
		//str += buf;
		str += "\n";
	}
	
	if(params.Tzero != defpars.Tzero){
		str += "-tzero ";
		sprintf(buf, "%f", params.Tzero);
		//_itoa(params.Tzero,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.updateTzero != defpars.updateTzero){
		str += "-update_tz ";
		sprintf(buf, "%f", params.updateTzero);
		//_itoa(params.updateTzero,buf,10);
		str += buf;
		str += "\n";
	}

	if(params.alphaTZ != defpars.alphaTZ){
		str += "-alpha_tz ";
		sprintf(buf, "%f", params.alphaTZ);
		//_itoa(params.alphaTZ,buf,10);
		str += buf;
		str += "\n";
	}
	//////more common parameters
	if(params.monT){
		str += "-mon_t ";
		//_itoa(params.monT,buf,10);
		//str += buf;
		str += "\n";
	}

	if(params.monAB){
		str += "-monab ";
		//_itoa(params.monAB,buf,10);
		//str += buf;
		str += "\n";
	}

	if(params.graphDisp){
		str += "-graph ";
		//_itoa(params.graphDisp,buf,10);
		//str += buf;
		str += "\n";
	}

	if(params.monWellCl){
		str += "-mon_well_classified ";
		//_itoa(params.monWellCl,buf,10);
		//str += buf;
		str += "\n";
	}

	if(params.drawBadS){
		str += "-draw_bad_subsystem ";
		//_itoa(params.drawBadS,buf,10);
		//str += buf;
		str += "\n";
	}


	return str;
}

