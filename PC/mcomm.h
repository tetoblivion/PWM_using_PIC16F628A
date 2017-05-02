//Author: Pentagrid  https://www.youtube.com/channel/UCOlrSCBtFFEF7uTX22WIhDA



#ifndef MCOMM_H
#define MCOMM_H

#include<Windows.h>

//This project for some reason requires MFC to be installed to your Visual Studio



struct tprm{
	unsigned char tt,ps;
	unsigned short mduty,duty;
	//
	void sett(unsigned int t,double d); //period in ns  (nano seconds)	//d duty<1.0 duty>0.0	
	void setf(double f,double d); //frequ in Hz  (hertz) //d duty<1.0 duty>0.0
	int tt2ns();		
	double tt2hz();		
	void nextf(double d);	
	void prevf(double d);
	//
	int getrange(double *f);
	void setrange(int r,double f,double d);
};

struct tcomm{
HANDLE file;
wchar_t ports[21][21];
int n,error;
tprm prm; //params
//
tcomm(){file=INVALID_HANDLE_VALUE;n=0;error=0;}
void enumcoms();
void setupcom(wchar_t str[]);
void sdrv();
void on();
void off();
void reset();
};

#endif