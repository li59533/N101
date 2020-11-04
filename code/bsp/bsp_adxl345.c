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
/**
 * @addtogroup    XXX 
 * @{  
 */
#include "bsp_spi.h"
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
	
	BSP_SPI_ReadByte(XL345_DEVID);
	
	BSP_SPI_WriteByte(XL345_DATA_FORMAT , 0X0b);
	BSP_SPI_WriteByte(XL345_POWER_CTL , 0X08);
	BSP_SPI_WriteByte(XL345_BW_RATE , 0X0e);
	BSP_SPI_WriteByte(XL345_INT_ENABLE , 0X00);
	BSP_SPI_WriteByte(XL345_FIFO_CTL , 0X00);
	BSP_SPI_WriteByte(XL345_INT_MAP , 0X00);
}




void BSP_Adxl345_read(uint8_t addr , uint8_t *buf , uint8_t len)
{
	
}



/*
XL345_DATA_FORMAT
XL345_POWER_CTL
XL345_INT_ENABLE
*/


void BSP_Adxl345_rev_xyz(void)  
{
	uint8_t cmd[2];
	cmd[0] = XL345_DATAX0;
	cmd[1] = 0xC0;
	
}


// ------ Test Code ---------

void BSP_Adxl345_TestCode(void)
{
	BSP_SPI_ReadByte(XL345_DATAZ0);
	BSP_SPI_ReadByte(XL345_DATAZ1);
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

