#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "VEML7700_Header.h"



#define   SlaveAddress   0x21 //∂¡µÿ÷∑0x20,–¥µÿ÷∑0x10

uint8_t BUF[8] = {0};
uint16_t     dis_data;                       
uint32_t   dis_temp;

 
typedef struct{
	uint8_t ALS_SD;
	uint8_t ALS_INT_EN;
	uint8_t ALS_PERS;
	uint8_t ALS_IT;
	uint8_t ALS_GAIN;
}config;

config CONF;

void Init(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = 50000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C2, &I2C_InitStructure);
	
	I2C_Cmd(I2C2, ENABLE);
}

void VEML7700_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)
	{
		Timeout --;
		if (Timeout == 0)
		{
			break;
		}
	}
}

void VEML7700_Set_Value(uint8_t command,uint16_t value){
	I2C_GenerateSTART(I2C2, ENABLE);
	VEML7700_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2, Slave_ADR_WR, I2C_Direction_Transmitter);
	VEML7700_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C2, command);
	VEML7700_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	I2C_SendData(I2C2, value);
	VEML7700_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	I2C_GenerateSTOP(I2C2, ENABLE);
	
}

uint16_t VEML7700_Get_value(uint8_t command){
	uint16_t rcv;
	
	I2C_GenerateSTART(I2C2, ENABLE);
	VEML7700_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2, Slave_ADR_WR, I2C_Direction_Transmitter);
	VEML7700_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C2, command);
	VEML7700_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	I2C_GenerateSTART(I2C2, ENABLE);
	VEML7700_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2, Slave_ADR_RD, I2C_Direction_Receiver);
	VEML7700_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	
	VEML7700_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);
	rcv = I2C_ReceiveData(I2C2);
	
	
	VEML7700_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);
	rcv |= I2C_ReceiveData(I2C2)<<8;
	
	I2C_GenerateSTOP(I2C2, ENABLE);
	
	return rcv;
}

void Set_Config(){
	uint8_t gain, shutdown, interrupt, persistence, integration;  
	gain=CONF.ALS_GAIN;
	shutdown=CONF.ALS_SD;
	interrupt=CONF.ALS_INT_EN;
	persistence=CONF.ALS_PERS;
	integration=CONF.ALS_IT;
	
	interrupt=interrupt<<1;
	persistence=persistence<<4;
	integration=integration<<6;
	gain=gain<<11;
	
	uint16_t buff=0;
	buff=shutdown+interrupt+persistence+integration+gain;
	
	VEML7700_Set_Value(COMMAND_ALS_CONF,buff);
}



static float normalize_resolution(float value)
{
    switch (CONF.ALS_GAIN)
    {
 
        case ALS_GAIN_X2:
        value /= 2.0;
        break;
        case ALS_GAIN_d4:
            value *= 4.0;
            break;
        case ALS_GAIN_d8:
            value *= 8.0;
            break;
        case ALS_GAIN_X1:
        default:break;
        
    }
    switch (CONF.ALS_GAIN){
 
        case ALS_INTEGRATION_25ms:
            value *= 4;
            break;
        case ALS_INTEGRATION_50ms:
            value *= 2;
            break;
        case ALS_INTEGRATION_200ms:
            value *= 2.0;
            break;
        case ALS_INTEGRATION_400ms:
            value *= 4.0;
            break;
        case ALS_INTEGRATION_800ms:
            value *= 8.0;
            break;
        case ALS_INTEGRATION_100ms:
        default:break;
    }
    return value;    
 
} 

void VEML7700_Init(void)
{
	Init();
	
	CONF.ALS_GAIN=ALS_GAIN_d8;
	CONF.ALS_IT=ALS_INTEGRATION_400ms;
	CONF.ALS_INT_EN=ALS_INTERRUPT_DISABLE;
	CONF.ALS_SD=ALS_POWER_SHUTDOWN;
	Set_Config();
	
	Delay_ms(100);
	
	CONF.ALS_SD = ALS_POWER_ON;
	Set_Config();

}
 

float Get_Lux_Data(void)
{
    //gain value==2
//    return normalize_resolution(get_als_reg_data() * 0.1152);//????25ms
//    return normalize_resolution(get_als_reg_data() * 0.0576);//????50ms
//    return normalize_resolution(get_als_reg_data() * 0.0288);//????100ms
//    return normalize_resolution(get_als_reg_data() * 0.0144);//????200ms
    return normalize_resolution(VEML7700_Get_value(COMMAND_ALS_DATA) * 0.0072);//????400ms   ??????
//    return normalize_resolution(get_als_reg_data() * 0.0036);//????800ms
}
