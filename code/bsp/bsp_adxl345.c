/**
 **************************************************************************************************
 * @file        bsp_adxl345.c
 * @author
 * @version
 * @date        5/28/2016
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */
#include "self_def.h"
#include "bsp_adxl345.h"
#include "ADXL345.H"
#include "clog.h"
/**
 * @addtogroup    XXX 
 * @{  
 */
#include "bsp_spi.h"

#include "app_sample.h"
/**
 * @addtogroup    bsp_adxl345_Modules 
 * @{  
 */

/**
 * @defgroup      bsp_adxl345_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_adxl345_Macros_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_adxl345_Constants_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_adxl345_Private_Types
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_adxl345_Private_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_adxl345_Public_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_adxl345_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/** 
 * @defgroup      bsp_adxl345_Functions 
 * @brief         
 * @{  
 */
 
void BSP_Adxl345_Init(void)
{
	BSP_SPI_Init( BSP_SPI1 );
	
	//BSP_SPI_ReadByte(XL345_DEVID);
	DEBUG("Device ID:%X\r\n" , BSP_SPI_ReadByte(XL345_DEVID));
	
	
	BSP_SPI_WriteByte(XL345_DATA_FORMAT , 0X0b);
	BSP_SPI_WriteByte(XL345_POWER_CTL , 0X08);
	BSP_SPI_WriteByte(XL345_BW_RATE , 0X0f);
	BSP_SPI_WriteByte(XL345_INT_ENABLE , 0X00);
	BSP_SPI_WriteByte(XL345_FIFO_CTL , 0X00);
	BSP_SPI_WriteByte(XL345_INT_MAP , 0X00);
}

void BSP_Adxl345_Sample_3Axis(void)
{
	
	uint8_t buf[6];
	int16_t Acc_X = 0;
	int16_t Acc_Y = 0;
	int16_t Acc_Z = 0;
	
	buf[0] = BSP_SPI_ReadByte(XL345_DATAX0);
	buf[1] = BSP_SPI_ReadByte(XL345_DATAX1);
	
	buf[2] = BSP_SPI_ReadByte(XL345_DATAY0);
	buf[3] = BSP_SPI_ReadByte(XL345_DATAY1);
	
	buf[4] = BSP_SPI_ReadByte(XL345_DATAZ0);
	buf[5] = BSP_SPI_ReadByte(XL345_DATAZ1);
	
	Acc_X = ((buf[1] << 8 | buf[0])); 
	Acc_Y = ((buf[3] << 8 | buf[2])); 
	Acc_Z = ((buf[5] << 8 | buf[4])); 	
	
	
	APP_GetOriginalData( Acc_X);
	APP_GetOriginalData( Acc_Y);
	APP_GetOriginalData( Acc_Z);
}



// ------ Test Code ---------

void BSP_Adxl345_TestCode(void)
{

	char str_buf[50];
	
	uint8_t buf[6];
	int16_t Acc_X = 0;
	int16_t Acc_Y = 0;
	int16_t Acc_Z = 0;
	
	buf[0] = BSP_SPI_ReadByte(XL345_DATAX0);
	buf[1] = BSP_SPI_ReadByte(XL345_DATAX1);
	
	buf[2] = BSP_SPI_ReadByte(XL345_DATAY0);
	buf[3] = BSP_SPI_ReadByte(XL345_DATAY1);
	
	buf[4] = BSP_SPI_ReadByte(XL345_DATAZ0);
	buf[5] = BSP_SPI_ReadByte(XL345_DATAZ1);
	
	Acc_X = ((buf[1] << 8 | buf[0])); 
	Acc_Y = ((buf[3] << 8 | buf[2])); 
	Acc_Z = ((buf[5] << 8 | buf[4])); 
	
	//snprintf(str_buf , 50 , "X=%4.3f m/s2   Y=%4.3f m/s2   Z=%4.3f m/s2 \r\n" ,Acc_X*3.9/1000*9.8,Acc_Y*3.9/1000*9.8,Acc_Z*3.9/1000*9.8);
	
	//DEBUG("%s" , str_buf);
	
}



/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

