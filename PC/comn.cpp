//Author: Pentagrid  https://www.youtube.com/channel/UCOlrSCBtFFEF7uTX22WIhDA


#include <Windows.h>
#include "resource.h"
#include "mcomm.h"


//This project for some reason requires MFC to be installed to your Visual Studio



double conv(wchar_t str[]){     
int i=0,j=0;
__int64	nb=0;
while(str[i]!=0 && str[i]!='.' && i<20){
	if(str[i]>='0' && str[i]<='9'){
	nb*=10;
	nb+=str[i]-'0';
	}
	i++;
}
if(str[i]=='.'){
	i++;
	while(str[i]!=0 && j<2 && i<20){
		if(str[i]>='0' && str[i]<='9'){
		nb*=10;
		nb+=str[i]-'0';
		j++;
		}
	i++;
    }
	while(j<2){nb*=10;j++;}
}else{
	nb*=100;
}
return double(nb)/100.0;
}

void iconv(wchar_t str[],double dpd){
	__int64 d=__int64(dpd*100.0+0.5);
	int i=0,i2=0,ii,jj,b,f1,f2;
	f1=d%10;
	d/=10;
	f2=d%10;
	d/=10;
	if(f2||f1){str[i++]=f1+'0';str[i++]=f2+'0';str[i++]='.';}
	while(d/10>0){
		str[i]=d%10+'0';
		d/=10;
		i++;
		i2++;
		if(i2%3==0)str[i++]=',';
	}
	str[i++]=d%10+'0';
	str[i]=0;
	for(ii=0,jj=i-1;ii<jj;ii++,jj--){b=str[ii];str[ii]=str[jj];str[jj]=b;}
}

////////////////////

namespace mydlg{

	wchar_t serror[][50]={
		L"COMM ERROR",
		L"err2",

	};

	tcomm mycom;	
	wchar_t strsel[50]=L"";
		
	WNDPROC defEdit1Proc=NULL;
	WNDPROC defEdit2Proc=NULL;
	
	HWND sb1hwnd=NULL,
	sb2hwnd=NULL,
	dlghwnd=NULL;

	int range=4096;

	double dfreq=5000.0;
	double dduty=0.5;

	wchar_t str[100],str2[100];

	int online=0;


	
	void updateAll(){
		//
		//set bits and send 
		mycom.prm.setf(dfreq,dduty);
		mycom.sdrv();
		if(mycom.error)SetDlgItemText(dlghwnd,IDC_STATIC1,serror[0]);
		//
		double realf=mycom.prm.tt2hz();
		double reald=double(mycom.prm.duty)/double(mycom.prm.mduty);
		//
		// set freq edit text
		iconv(str,realf);
		SetDlgItemText(dlghwnd,IDC_EDIT1,str);
		//
		// set duty edit text
		iconv(str,reald*100.0);
		SetDlgItemText(dlghwnd,IDC_EDIT2,str);
		//
		// set freq scroll
		SCROLLINFO si;
		ZeroMemory(&si,sizeof(si));
		si.cbSize=sizeof(SCROLLINFO);
		int r;
		double f,r3=double(range)/3.0;
		r=mycom.prm.getrange(&f);
		si.nPos=int(range-(r*r3+f*r3));
		si.fMask =SIF_POS;
		SetScrollInfo(sb1hwnd,SB_CTL,&si,TRUE);
		//
		// set duty scroll
		ZeroMemory(&si,sizeof(si));
		si.cbSize=sizeof(SCROLLINFO);
		si.nPos=mycom.prm.duty*range/mycom.prm.mduty;
		si.fMask =SIF_POS;
		SetScrollInfo(sb2hwnd,SB_CTL,&si,TRUE);
		//
		//set strings
		double period=mycom.prm.tt2ns(), ton=period*reald;
		iconv(str,ton);
		wsprintf(str2,L"Ton = %s ns  ",str);
		SetDlgItemText(dlghwnd,IDC_STATIC9,str2);
		//
		iconv(str,period);
		wsprintf(str2,L"Period= %s ns ",str);
		SetDlgItemText(dlghwnd,IDC_STATIC12,str2);
		//
		iconv(str,period-ton);
		wsprintf(str2,L"Toff= %s ns  ",str);
		SetDlgItemText(dlghwnd,IDC_STATIC10,str2);
		//
		iconv(str,(1.0-reald)*100.0);
		wsprintf(str2,L"!Duty= %s %% ",str);
		SetDlgItemText(dlghwnd,IDC_STATIC11,str2);
		//
	}


	void save(){
		HANDLE file=NULL;
		DWORD br;
		file= CreateFile(L"saved.stt",
                 GENERIC_READ|GENERIC_WRITE,
                 FILE_SHARE_READ,
                 NULL,                 
                 OPEN_ALWAYS,
                 FILE_ATTRIBUTE_NORMAL,
                 NULL
                 );
		if(file){
			WriteFile(file,strsel,sizeof(strsel),&br,NULL);
			WriteFile(file,&dfreq,sizeof(dfreq),&br,NULL);
			WriteFile(file,&dduty,sizeof(dduty),&br,NULL);
			CloseHandle(file);
			SetDlgItemText(dlghwnd,IDC_STATIC1,L"Saved");
		}else{
			SetDlgItemText(dlghwnd,IDC_STATIC1,L"Not Saved");
		}
		//updateAll();
	}


	void load(){
		//wchar_t str[100];
		HANDLE file=NULL;
		DWORD br;
		file= CreateFile(L"saved.stt",
                 GENERIC_READ,
                 FILE_SHARE_READ,
                 NULL,                 
                 OPEN_EXISTING,
                 FILE_ATTRIBUTE_NORMAL,
                 NULL
                 );
		if(file!=INVALID_HANDLE_VALUE){
			ReadFile(file,strsel,sizeof(strsel),&br,NULL);
			ReadFile(file,&dfreq,sizeof(dfreq),&br,NULL);
			ReadFile(file,&dduty,sizeof(dduty),&br,NULL);
			CloseHandle(file);
			mycom.setupcom(strsel);
			if(!mycom.error){
				SetDlgItemText(dlghwnd,IDC_STATIC1,L"Loaded");
				SendDlgItemMessage(dlghwnd,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)strsel);
				SendDlgItemMessage(dlghwnd,IDC_COMBO1,CB_SELECTSTRING,0,(LPARAM)strsel);
			}else SetDlgItemText(dlghwnd,IDC_STATIC1,L"Comm Select Error");
		}else{
			SetDlgItemText(dlghwnd,IDC_STATIC1,L"Not Loaded");
		}
		updateAll();
	}




	void enterf(){
		GetDlgItemText(dlghwnd,IDC_EDIT1,str,100);
		dfreq=conv(str);
		if(dfreq<1225)dfreq=1225;
		if(dfreq>5000000)dfreq=5000000;
		updateAll();
	}



	void enterd(){
		GetDlgItemText(dlghwnd,IDC_EDIT2,str,100);
		dduty=conv(str)/100.0;
		if(dduty>1.0)dduty=1.0;
		if(dduty<0.0)dduty=0.0;
		updateAll();
	}


	void slided(){
		SCROLLINFO si;
		ZeroMemory(&si,sizeof(si));
		si.cbSize=sizeof(SCROLLINFO);
		//
		si.fMask =SIF_TRACKPOS; 
		GetScrollInfo(sb2hwnd,SB_CTL,&si);
		//
		dduty=double(si.nTrackPos)/range;
		//
		updateAll();
	}

	void slidef(){
		int r13=range/3, r23=2*range/3, posi=0;
		SCROLLINFO si;
		ZeroMemory(&si,sizeof(si));
		si.cbSize=sizeof(SCROLLINFO);
		//
		si.fMask =SIF_TRACKPOS; 
		GetScrollInfo(sb1hwnd,SB_CTL,&si);
		//
		posi=range-si.nTrackPos;
		if(posi<r13){
			mycom.prm.setrange(0,double(posi)/r13,dduty);
		}else if(posi<r23){
			mycom.prm.setrange(1,double(posi-r13)/r13,dduty);
		}else{
			mycom.prm.setrange(2,double(posi-r23)/r13,dduty);
		}
		//
		dfreq=mycom.prm.tt2hz();
		//
		updateAll();
	}

	void clickd(int dir){
		if(dir){
			if(mycom.prm.duty<mycom.prm.mduty)dduty+=1.0/double(mycom.prm.mduty);
		}else{
			if(mycom.prm.duty>0)dduty-=1.0/double(mycom.prm.mduty);
		}
		updateAll();
	}


	void clickf(int dir){
		if(dir){
			mycom.prm.nextf(dduty);
		}else{
			mycom.prm.prevf(dduty);
		}
		dfreq=mycom.prm.tt2hz();
		updateAll();
	}


	void onoff(){
		if(online){
			mydlg::mycom.off();
			SetDlgItemText(dlghwnd,IDOK,L"Turn ON");
			if(mydlg::mycom.error)SetDlgItemText(dlghwnd,IDC_STATIC1,L"ON/OFF error");else SetDlgItemText(dlghwnd,IDC_STATIC1,L"OFF");
			online=0;
		}else{
			mydlg::mycom.on();
			//
			SetDlgItemText(dlghwnd,IDOK,L"Turn OFF");
			if(mydlg::mycom.error){
				SetDlgItemText(dlghwnd,IDC_STATIC1,L"ON/OFF error");
				online=0;
			}else{
				SetDlgItemText(dlghwnd,IDC_STATIC1,L"ON");
				updateAll();
				online=7;
			}
		}
	}



	wchar_t cmdf[50] = L"";
	wchar_t cmdd[50] = L"";

	void trySetFromCmdLine() {
		if (strsel[0]!=0 && cmdf[0] != 0 && cmdd[0] !=0) {
			dfreq = conv(cmdf);
			if (mydlg::dfreq < 1225)dfreq = 1225;
			if (mydlg::dfreq > 5000000)dfreq = 5000000;
			dduty = conv(cmdd) / 100.0;
			if (dduty > 1.0)dduty = 1.0;
			if (dduty < 0.0)dduty = 0.0;
			mycom.setupcom(strsel);
			if (!mycom.error) {
				SetDlgItemText(dlghwnd, IDC_STATIC1, L"Loaded");
				SendDlgItemMessage(dlghwnd, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)strsel);
				SendDlgItemMessage(dlghwnd, IDC_COMBO1, CB_SELECTSTRING, 0, (LPARAM)strsel);
			}
			else SetDlgItemText(dlghwnd, IDC_STATIC1, L"Comm Select Error");
			onoff();
			updateAll();
		}
	}

}//namespace


LRESULT edit1proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message){
	case WM_KEYDOWN:
		switch(wParam){
		case  VK_RETURN:
			mydlg::enterf();
			return TRUE;
		}		
	break;
	}
	return CallWindowProc(mydlg::defEdit1Proc,hwnd,message,wParam,lParam);;
}


LRESULT edit2proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message){
	case WM_KEYDOWN:
		switch(wParam){
		case  VK_RETURN:
			mydlg::enterd();
			return TRUE;
		}
	break;
	}
	return CallWindowProc(mydlg::defEdit2Proc,hwnd,message,wParam,lParam);
}




BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
DWORD gmr=0;
int i;
int kf=0;


//
SCROLLINFO si;
 ZeroMemory(&si,sizeof(si));
 si.cbSize=sizeof(SCROLLINFO);
 int sipos=0;


	switch(message){
	case WM_INITDIALOG:

		 mydlg::sb1hwnd=GetDlgItem(hwnd,IDC_SCROLLBAR1);
		 mydlg::sb2hwnd=GetDlgItem(hwnd,IDC_SCROLLBAR2);
		 mydlg::dlghwnd=hwnd;

		 si.fMask =SIF_POS | SIF_RANGE;
         si.nMin  =0;
         si.nMax  =mydlg::range;
         si.nPos  =0;

         SetScrollInfo(mydlg::sb1hwnd,SB_CTL,&si,TRUE);
		 SetScrollInfo(mydlg::sb2hwnd,SB_CTL,&si,TRUE);

		 mydlg::defEdit1Proc=(WNDPROC)SetWindowLongPtr(GetDlgItem(hwnd,IDC_EDIT1),GWLP_WNDPROC,(LONG_PTR)edit1proc);
         mydlg::defEdit2Proc=(WNDPROC)SetWindowLongPtr(GetDlgItem(hwnd,IDC_EDIT2),GWLP_WNDPROC,(LONG_PTR)edit2proc);		 

		 mydlg::trySetFromCmdLine();
    break;

	case WM_KEYDOWN:
		switch(wParam){
		case  VK_RETURN:
			return FALSE; 
			break;
		}
		break;

    case WM_COMMAND:
       switch(LOWORD(wParam)){
	   case IDOK:
		   mydlg::onoff();
	   break;
	   case  IDC_BUTTON2:
		   mydlg::save();
	   break;
	   case  IDC_BUTTON1:
		   mydlg::load();
	   break;
	   }
	   
	   switch(HIWORD(wParam)){
	   case	CBN_DROPDOWN: 
		   SendDlgItemMessage(hwnd,IDC_COMBO1,CB_RESETCONTENT,0,0);
		   //
		   mydlg::mycom.enumcoms();
		   for(i=0;i<mydlg::mycom.n;i++){
			   SendDlgItemMessage(hwnd,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)mydlg::mycom.ports[i]);
		   }
		   SetDlgItemText(hwnd,IDC_STATIC1,L"COMMs probed");
	   break;
	   case CBN_SELENDOK:
		   DlgDirSelectComboBoxEx(hwnd,mydlg::strsel,13,IDC_COMBO1);		   
		   mydlg::mycom.setupcom(mydlg::strsel);
		   if(!mydlg::mycom.error)SetDlgItemText(hwnd,IDC_STATIC1,L"COMM selected");
		   else SetDlgItemText(hwnd,IDC_STATIC1,L"COMM not selected");
	   break;
	   }
	   
	break;
	case WM_HSCROLL:
		
	if((HWND)lParam==mydlg::sb2hwnd){
		switch(LOWORD(wParam)){
		case SB_PAGERIGHT:
		case SB_LINERIGHT:
			mydlg::clickd(1);
		break;
		case SB_PAGELEFT:
		case SB_LINELEFT:
			mydlg::clickd(0);
		break;
		//case SB_THUMBPOSITION: 
		case SB_THUMBTRACK:
			mydlg::slided();
        break;
		}
		
	}else if((HWND)lParam==mydlg::sb1hwnd){
		switch(LOWORD(wParam)){
		case SB_PAGELEFT:
		case SB_LINELEFT:
			mydlg::clickf(0);
		break;
		case SB_PAGERIGHT:
		case SB_LINERIGHT:
			mydlg::clickf(1);
		break;
		case SB_THUMBTRACK:
			mydlg::slidef();
        break;
		}
	}
	break;
	case WM_CLOSE:		
		SetWindowLongPtr(GetDlgItem(hwnd,IDC_EDIT1),GWLP_WNDPROC,(LONG_PTR)mydlg::defEdit1Proc);
		SetWindowLongPtr(GetDlgItem(hwnd,IDC_EDIT2),GWLP_WNDPROC,(LONG_PTR)mydlg::defEdit2Proc);
        EndDialog(hwnd,0);
    break;
	default:
		return FALSE;
	break;
	}
	return TRUE;

}//DlgProc()

													       

void getFromCommandLine(LPSTR cmdLine) {

	int i = 0;
	while (cmdLine[i] != 0 && cmdLine[i] == ' ')i++;

	int j = 0;
	while (cmdLine[i] != 0 && cmdLine[i] != ' ' && j < 40) {
		mydlg::strsel[j] = cmdLine[i];
		i++;
		j++;
	}
	mydlg::strsel[j] = 0;
	while (cmdLine[i] != 0 && cmdLine[i] == ' ')i++;
	

	j = 0;
	while (cmdLine[i] != 0 && cmdLine[i] != ' ' && j < 40) {
		mydlg::cmdf[j] = cmdLine[i];
		i++;
		j++;
	}
	mydlg::cmdf[j] = 0;
	while (cmdLine[i] != 0 && cmdLine[i] == ' ')i++;

	j = 0;
	while (cmdLine[i] != 0 && cmdLine[i] != ' ' && j < 40) {
		mydlg::cmdd[j] = cmdLine[i];
		i++;
		j++;
	}
	mydlg::cmdd[j] = 0;
	while (cmdLine[i] != 0 && cmdLine[i] == ' ')i++;
}



int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR cmdLine,int nCmdShow){

	getFromCommandLine(cmdLine);

	DialogBox(hInst, 
		MAKEINTRESOURCE(IDD_DIALOG1),
		GetDesktopWindow(),
		(DLGPROC)DlgProc);

return 0;
}






