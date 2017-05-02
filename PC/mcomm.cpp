//Author: Pentagrid  https://www.youtube.com/channel/UCOlrSCBtFFEF7uTX22WIhDA



#include"mcomm.h"


//This project for some reason requires MFC to be installed to your Visual Studio



void tprm::setf(double f,double d){ //frequ in Hz  (hertz) //d duty<1.0 duty>0.0
		sett((unsigned int)(1000000000.0/f+0.5),d);
}


void tprm::sett(unsigned int t,double d){ //period in ns  (nano seconds) //d duty<1.0 duty>0.0
unsigned short step;
		if(t<200){  
			tt=0;
			ps=0;
			return;
		}else if(t<51200){        // not <= because 255*4+4>1023
			step=200; //200ns/bit
			ps=0;
		}else if(t<204800){
			step=800; //800ns/bit
			ps=1;
		}else if(t<819200){
			step=3200; //3200ns/bit
			ps=2;
		}else{
			tt=254;
			ps=2;
			return;
		}
		tt=t/step-1;  //
		mduty=(short(tt)<<2)+4;
		//
		duty=(unsigned short)(d*double(mduty)+0.5);
}


int tprm::getrange(double *f){
	if(ps==0){
		*f=double(tt)/254;
		return 0;
	}else if(ps==1){
		*f=double(tt-63)/191;
		return 1;
	}else{
		*f=double(tt-63)/191;
		return 2;
	}
}


void tprm::setrange(int r,double f,double d){
	if(f<0.0)f=0.0;else if(f>1.0)f=1.0;
	if(r==0){
		ps=0;
		tt=(unsigned char)(f*254+0.5);
	}else if(r==1){
		ps=1;
		tt=(unsigned char)(f*191+0.5+63);
	}else{
		ps=2;
		tt=(unsigned char)(f*191+0.5+63);
	}
	mduty=(short(tt)<<2)+4;
	//
	duty=(unsigned short)(d*double(mduty)+0.5);
}



int tprm::tt2ns(){
		int s;
		if(ps==0)s=1;
		else if(ps==1)s=4;
		else s=16;
		return 200*(tt+1)*s;
}


double tprm::tt2hz(){
		return double(1000000000)/tt2ns();
}


void tprm::nextf(double d){
		if(ps==0 && tt<=0){
			return;
		}else if(ps==1 && tt<=64){
			ps=0;
			tt=254;
		}else if(ps==2 && tt<=64){
			ps=1;
			tt=254;
		}else{
			tt--;
		}
		mduty=(short(tt)<<2)+4;
		//
		duty=(unsigned short)(d*double(mduty)+0.5);
}

void tprm::prevf(double d){
		if(ps==2 && tt>=254){
			return;
		}else if(ps==0 && tt>=254){
			ps=1;
			tt=64;
		}else if(ps==1 && tt>=254){
			ps=2;
			tt=64;
		}else{
			tt++;
		}
		mduty=(short(tt)<<2)+4;
		//
		duty=(unsigned short)(d*double(mduty)+0.5);
}



//**********************



void tcomm::enumcoms(){
	int i,j;
	if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);file=INVALID_HANDLE_VALUE;
	HANDLE file2=INVALID_HANDLE_VALUE;
	wchar_t str[15];
	//
	for(i=0;i<20;i++)ports[i][0]=0;
	//
	for(i=0,n=0;i<20;i++){
	wsprintf(str,L"\\\\.\\COM%d",i);
	file2=CreateFile(str,GENERIC_WRITE | GENERIC_READ, 0, 0, OPEN_EXISTING, 0, NULL);
	if(file2!=INVALID_HANDLE_VALUE){
		j=0;while(str[j]!=0 && j<100){ports[n][j]=str[j];j++;}ports[n][j]=0;
		n++;
		CloseHandle(file2);
	}
	}
}

void tcomm::setupcom(wchar_t str[]){
	if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
	file=INVALID_HANDLE_VALUE;
	DCB dcb;
	int r=0;
	error=0;
	//
	file=CreateFile(str, GENERIC_WRITE | GENERIC_READ, 0, 0, OPEN_EXISTING, 0, NULL);
	if(file==INVALID_HANDLE_VALUE){error=4;return;}
	r=GetCommState(file,&dcb);
	//
	dcb.DCBlength = sizeof(dcb);
    dcb.BaudRate=9600;    
	dcb.ByteSize=8;
	dcb.fParity=0;
	dcb.fBinary=1;
	dcb.Parity=NOPARITY;//MARKPARITY;//SPACEPARITY;//NOPARITY;//
	dcb.StopBits=ONESTOPBIT;//TWOSTOPBITS;//ONESTOPBIT;//TWOSTOPBITS;//
	dcb.fRtsControl=RTS_CONTROL_DISABLE;
	dcb.fDtrControl=DTR_CONTROL_DISABLE;
	dcb.EofChar=0x0;
	dcb.fDsrSensitivity=0;
	dcb.fInX=0;
	dcb.fOutX=0;
	dcb.fOutxCtsFlow=0;
	dcb.fOutxDsrFlow=0;
	dcb.fTXContinueOnXoff=0;
	dcb.fNull=0;
    //  
	SetCommState(file,&dcb);
    //
	COMMTIMEOUTS tmos;
	GetCommTimeouts(file,&tmos);
	//
	tmos.ReadIntervalTimeout=200;
	tmos.ReadTotalTimeoutConstant=10;
	tmos.ReadTotalTimeoutMultiplier=10;
	tmos.WriteTotalTimeoutConstant=200;
	tmos.WriteTotalTimeoutMultiplier=10;
	//
	SetCommTimeouts(file,&tmos);
	//
}

void tcomm::sdrv(){
	if(file!=INVALID_HANDLE_VALUE){
	DWORD wttn;

	char t1=prm.tt;
	char t2=prm.ps;

	char t3=prm.duty>>2;
	char t4=prm.duty&3;

	char r1, r2, r3, r4;
	char n1, n2, n3, n4;

	error=0;

         WriteFile(file,&t1,1,&wttn,NULL);
		 ReadFile(file,&r1,1,&wttn,NULL);
		 ReadFile(file,&n1,1,&wttn,NULL);
		 if(t1!=r1 || n1!=4)error=3;

		 WriteFile(file,&t2,1,&wttn,NULL);		 
		 ReadFile(file,&r2,1,&wttn,NULL);
		 ReadFile(file,&n2,1,&wttn,NULL);
		 if(t2!=r2 || n2!=3)error=3;

		 WriteFile(file,&t3,1,&wttn,NULL);		 
		 ReadFile(file,&r3,1,&wttn,NULL);
		 ReadFile(file,&n3,1,&wttn,NULL);
		 if(t3!=r3 || n3!=2)error=3;

		 WriteFile(file,&t4,1,&wttn,NULL);		 
		 ReadFile(file,&r4,1,&wttn,NULL);
		 ReadFile(file,&n4,1,&wttn,NULL);
		 if(t4!=r4 || n4!=1)error=3;
	}else{
		error=3;
	}
}



void tcomm::on(){
	DWORD gmr=0;
	error=0;
	EscapeCommFunction(file,CLRDTR);//ON
	GetCommModemStatus(file,&gmr);
    if(gmr&MS_DSR_ON)error=1; 
}

void tcomm::off(){
	DWORD gmr=0;
	error=0;
	EscapeCommFunction(file,SETDTR);//OFF
	GetCommModemStatus(file,&gmr);
	if(!(gmr&MS_DSR_ON))error=2;
}

void tcomm::reset(){
	error=0;
	off();
	if(error)return;
	on();
}