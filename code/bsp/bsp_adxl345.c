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
}

void BSP_Adxl345_read(uint8_t addr , uint8_t *buf , uint8_t len)
{
	
}

void BSP_Adxl345_rev_xyz(void)
{
	uint8_t cmd[2];
	cmd[0] = XL345_DATAX0;
	cmd[1] = 0xC0;
	
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

