#include "math.h"
#include "stm32f10x.h"                  // Device header

double Lux_to_Ev(float lux){
	double ev;
	ev=log10(lux/2.5)/log10(2.0);
	return ev;
}

double AV_to_TV(float lux,uint16_t ISO,float AV){
	double ev2= lux/2.5;
	double t;
	t=(AV*AV*100.0)/(ev2*ISO);
	return t;
}

double TV_to_AV(float lux,uint16_t ISO,float TV){
	double ev2= lux/2.5;
	double f;
	return f;
}
