#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "AD.h"
#include "encoder.h"
#include "Key.h"
#include "VEML7700.h"
#include "convert.h"

uint16_t ISO[] ={25,50,100,150,200,280,320,400,560,800,1100,1600,3200};
float AV[] = {1,1.2,1.4,1.8,2.0,2.6,2.8,3.5,4,4.5,5.6,6.7,8.0,9.5,11,13,16,19,22,32};
//char AVS[3][19]= {"1","1.2","1.4","1.8","2.0","2.6","2.8","3.5","4","4.5","5.6","6.7","8.0","9.5","11","13","16","19","22","32"};
uint16_t TVB[] = {30,23,15,11,8,6,4,3,2,1};//10
uint16_t TVS[] = {2,3,4,6,8,12,15,22,30,45,60,90,125,180,250,360,400,500,750,1000,1500,2000,3000,4000};//24
uint8_t iso=2;
uint8_t av = 9;
uint8_t	tv=21;
float veml_v;
double ev;
double tvi;
double avi;
	
typedef enum SYS_Struct{
	SYS_Init,
	SYS_AV,
	SYS_TV,
	SYS_ISO,
	SYS_ERR,
}SYS_Struct;

typedef struct{
	SYS_Struct SSS;
	uint16_t timer;
}SSM;

void Struct_Init(SSM *sm){
	sm->SSS = SYS_Init;
	sm->timer=0;
			
}

int main(void)
{
	SSM sm;
	Struct_Init(&sm);
	
	while (1)
	{
		switch (sm.SSS){
			case SYS_Init:
				Key_Init();
				AD_Init();
				Encoder_Init();
				OLED_Init();
				VEML7700_Init();
				sm.SSS = SYS_ISO;
				break;
			case SYS_AV:
				OLED_Clear();
				OLED_ShowString(1,1,"AV");
				OLED_ShowString(2,1,"F:");
				OLED_ShowNum(1,4,ISO[iso],4);
				while(1){
					veml_v=Get_Lux_Data();
					tvi= AV_to_TV(veml_v,ISO[iso],AV[av]);
					int8_t add;
					add=Encoder_Get();
					if(add>0&&av<18){
						av++;
					}else if(add<0&&av>0){
						av--;
					}
					char avs[4];
					sprintf(avs,"%2.1f",AV[av]);
					OLED_ShowString(2,3,avs);
					OLED_ShowNum(3,1,tvi,4);
					if (Key_GetNum()==1){
						sm.SSS=SYS_TV;
						Delay_ms(50);
						break;
					}
				}
				break;
			case SYS_TV:
				OLED_Clear();
				OLED_ShowString(1,1,"TV");
				OLED_ShowNum(1,4,ISO[iso],4);
				OLED_ShowString(2,1,"S:");
				while(1){
					veml_v=Get_Lux_Data();
					int8_t add;
					add=Encoder_Get();
					if(add>0&&tv<33){
						tv++;
					}else if(add<0&&tv>0){
						tv--;
					}
					if(tv>9){
						OLED_ShowString(2,3,"1/");
						OLED_ShowNum(2,5,TVS[tv-10],4);
					}else{
						OLED_ShowNum(2,3,TVB[tv],2);
						OLED_ShowString(2,5,"S   ");
					}
					OLED_ShowNum(3,1,ev,4);
					if (Key_GetNum()==1){
						sm.SSS=SYS_AV;
						Delay_ms(50);
						break;
					}
				}
				break;
			case SYS_ISO:
				OLED_Clear();
				OLED_ShowString(1,1,"Set a ISO");
				while(1){
					int8_t add;
					add=Encoder_Get();
					if(add>0&&iso<12){
						iso++;
					}else if(add<0&&iso>0){
						iso--;
					}
					OLED_ShowNum(2,4,ISO[iso],4);
					Delay_ms(20);
					if (Key_GetNum()==1){
						sm.SSS=SYS_AV;
						Delay_ms(50);
						break;
					}
				}
				break;
			case SYS_ERR:
				break;
		}
	}
}
