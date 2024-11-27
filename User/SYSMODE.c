#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "AD.h"
#include "encoder.h"
#include "Key.h"

void SYS_Init(){
				Key_Init();
				AD_Init();
				Encoder_Init();
				OLED_Init();
}

void SYS_ISO(){
	
}

