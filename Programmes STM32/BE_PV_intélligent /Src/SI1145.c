/*
 * SI1145.c
 *
 *  Created on: 25 févr. 2020
 *      Author: YAHMI & ARHAB
*/
#include "SI1145.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"


uint8_t ReadByte(I2C_HandleTypeDef *hi2c, uint8_t address)
{
uint8_t value;

	HAL_I2C_Master_Transmit(hi2c,SI114X_ADDR<<1,&address,1,1000);
    HAL_I2C_Master_Receive(hi2c,SI114X_ADDR<<1,&value,1,1000);

    return value;
}
/*--------------------------------------------------------*/


void WriteByte( I2C_HandleTypeDef* hi2c, uint8_t address, uint8_t val) {
	uint8_t dataraw[2];

	dataraw[0]=address;
	dataraw[1]=val;

HAL_I2C_Master_Transmit(hi2c,SI114X_ADDR<<1,(uint8_t *)dataraw,2,1000);

}

/*--------------------------------------------------------*/
//default init

void DeInit( I2C_HandleTypeDef* hi2c)
{
  //ENABLE UV reading
  //these reg must be set to the fixed value
  WriteByte(hi2c,SI114X_UCOEFF0, 0x29);
  WriteByte(hi2c,SI114X_UCOEFF1, 0x89);
  WriteByte(hi2c,SI114X_UCOEFF2, 0x02);
  WriteByte(hi2c,SI114X_UCOEFF3, 0x00);
  WriteParamData(hi2c,SI114X_CHLIST, SI114X_CHLIST_ENUV |SI114X_CHLIST_ENALSIR | SI114X_CHLIST_ENALSVIS |SI114X_CHLIST_ENPS1);
  //
  //set LED1 CURRENT(22.4mA)(It is a normal value for many LED)
  //
  WriteParamData(hi2c,SI114X_PS1_ADCMUX, SI114X_ADCMUX_LARGE_IR);
  WriteByte(hi2c,SI114X_PS_LED21, SI114X_LED_CURRENT_22MA);
  WriteParamData(hi2c,SI114X_PSLED12_SELECT, SI114X_PSLED12_SELECT_PS1_LED1); //
  //
  //PS ADC SETTING
  //
  WriteParamData(hi2c,SI114X_PS_ADC_GAIN, SI114X_ADC_GAIN_DIV1);
  WriteParamData(hi2c,SI114X_PS_ADC_COUNTER, SI114X_ADC_COUNTER_511ADCCLK);
  WriteParamData(hi2c,SI114X_PS_ADC_MISC, SI114X_ADC_MISC_HIGHRANGE|SI114X_ADC_MISC_ADC_RAWADC);
  //
  //VIS ADC SETTING
  //
  WriteParamData(hi2c,SI114X_ALS_VIS_ADC_GAIN, SI114X_ADC_GAIN_DIV1);
  WriteParamData(hi2c,SI114X_ALS_VIS_ADC_COUNTER, SI114X_ADC_COUNTER_511ADCCLK);
  WriteParamData(hi2c,SI114X_ALS_VIS_ADC_MISC, SI114X_ADC_MISC_HIGHRANGE);
  //
  //IR ADC SETTING
  //
  WriteParamData(hi2c,SI114X_ALS_IR_ADC_GAIN, SI114X_ADC_GAIN_DIV1);
  WriteParamData(hi2c,SI114X_ALS_IR_ADC_COUNTER, SI114X_ADC_COUNTER_511ADCCLK);
  WriteParamData(hi2c,SI114X_ALS_IR_ADC_MISC, SI114X_ADC_MISC_HIGHRANGE);
  //
  //interrupt enable
  //
  WriteByte(hi2c,SI114X_INT_CFG, SI114X_INT_CFG_INTOE);
  WriteByte(hi2c,SI114X_IRQ_ENABLE, SI114X_IRQEN_ALS);
  //
  //AUTO RUN
  //
  WriteByte(hi2c,SI114X_MEAS_RATE0, 0xFF);
  WriteByte(hi2c,SI114X_COMMAND, SI114X_PSALS_AUTO);
}

/*--------------------------------------------------------*/
//read half word(2 bytes) data from si114x

uint16_t ReadHalfWord(I2C_HandleTypeDef* hi2c,uint8_t Reg, uint8_t addr)
{
  uint8_t value[2];
  uint16_t Value;

 HAL_I2C_Master_Transmit(hi2c,addr<<1,&(Reg),1,1000);
 HAL_I2C_Master_Receive(hi2c,addr<<1,(uint8_t *)value,2,1000);

	 Value =  value[0] ;
	 Value |= (uint16_t)value[1] << 8 ;

  return Value;
}

/*--------------------------------------------------------*/
//read param data

uint8_t ReadParamData(I2C_HandleTypeDef* hi2c,uint8_t Reg)
{
	WriteByte(hi2c, SI114X_COMMAND, Reg | SI114X_QUERY);
	return ReadByte( hi2c, SI114X_RD);
}

/*--------------------------------------------------------*/
//write param data

uint8_t WriteParamData(I2C_HandleTypeDef* hi2c, uint8_t Reg,uint8_t Value)
{
	//write Value into PARAMWR reg first
   WriteByte(hi2c, SI114X_WR, Value);
   WriteByte(hi2c, SI114X_COMMAND, Reg | SI114X_SET);
   //SI114X writes value out to PARAM_RD,read and confirm its right
   return ReadByte(hi2c, SI114X_RD);
}

/*--------------------------------------------------------*/
//change addres

void ADDR_Data(I2C_HandleTypeDef* hi2c,uint8_t Value)
{
  //write Value into PARAMWR reg first
   WriteByte(hi2c, SI114X_WR, Value);
   WriteByte(hi2c,SI114X_COMMAND , SI114X_I2C_ADDR | SI114X_SET );
   WriteByte(hi2c,SI114X_COMMAND , SI114X_BUSADDR);

}

/*----------------------------------*/
  void Reset(I2C_HandleTypeDef* hi2c)
{
  WriteByte(hi2c, SI114X_MEAS_RATE0, 0);
  WriteByte(hi2c, SI114X_MEAS_RATE1, 0);
  WriteByte(hi2c, SI114X_IRQ_ENABLE, 0);
  WriteByte(hi2c, SI114X_IRQ_MODE1, 0);
  WriteByte(hi2c, SI114X_IRQ_MODE2, 0);
  WriteByte(hi2c, SI114X_INT_CFG, 0);
  WriteByte(hi2c, SI114X_IRQ_STATUS, 0xFF);

  WriteByte(hi2c, SI114X_COMMAND, SI114X_RESET);
  HAL_Delay(10);
  WriteByte(hi2c, SI114X_HW_KEY, 0x17);
  HAL_Delay(10);
}

 /*-------------------------------------------------------*/
  int Begin(I2C_HandleTypeDef* hi2c)
  {

    if(ReadByte(hi2c, SI114X_PART_ID)!=0X45)
    {
      return 0;
    }
    Reset(hi2c);
    //
    //INIT
    //
    DeInit(hi2c);
    return 1;
  }

/*--------------------------------------------------------*/
//Read Visible Value

 uint16_t ReadVisible(I2C_HandleTypeDef* hi2c,uint8_t addr)
{
  return ReadHalfWord(hi2c, SI114X_ALS_VIS_DATA0, addr);
}

 /*--------------------------------------------------------*/
//Read IR Value

 uint16_t ReadIR(I2C_HandleTypeDef* hi2c,uint8_t addr)
{
  return ReadHalfWord(hi2c, SI114X_ALS_IR_DATA0,addr);
}


 /*--------------------------------------------------------*/
//Read UV Value
// this function is a int value ,but the real value must be div 100

uint16_t ReadUV(I2C_HandleTypeDef* hi2c,uint8_t addr)
{
  return (ReadHalfWord(hi2c, SI114X_AUX_DATA0_UVINDEX0,addr));
}





