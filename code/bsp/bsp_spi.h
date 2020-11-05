/**
 **************************************************************************************************
 * @file        bsp_spi.h
 * @author
 * @version
 * @date        5/28/2016
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */
#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

/**
 * @addtogroup    XXX 
 * @{ 
 */
#include "self_def.h"
/**
 * @addtogroup    bsp_spi_Modules 
 * @{  
 */

/**
 * @defgroup      bsp_spi_Exported_Macros 
 * @{  
 */
#define BSP_SPI0 0
#define BSP_SPI1 1
/**
 * @}
 */

/**
 * @defgroup      bsp_spi_Exported_Constants
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_spi_Exported_Types 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_spi_Exported_Variables 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_spi_Exported_Functions 
 * @{  
 */
void BSP_SPI_Init(uint8_t BSP_SPIx);
void BSP_SPI_Send(uint8_t *buf , uint8_t len );
void BSP_SPI_WriteByte(uint8_t addr , uint8_t val);
uint8_t BSP_SPI_ReadByte(uint8_t addr);


void BSP_SPI_ReadBytes(uint8_t addr , uint8_t * buf );

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
#endif
