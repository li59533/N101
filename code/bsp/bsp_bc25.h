/**
 **************************************************************************************************
 * @file        bsp_bc25.h
 * @author
 * @version
 * @date        5/28/2016
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */
#ifndef _BSP_BC25_H_
#define _BSP_BC25_H_

/**
 * @addtogroup    XXX 
 * @{ 
 */
#include "self_def.h"
/**
 * @addtogroup    bsp_bc25_Modules 
 * @{  
 */

/**
 * @defgroup      bsp_bc25_Exported_Macros 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_bc25_Exported_Constants
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_bc25_Exported_Types 
 * @{  
 */

/**
 * @}
 */

/** 
 * @defgroup      bsp_bc25_Exported_Variables 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_bc25_Exported_Functions 
 * @{  
 */
 
void BSP_BC25_Init(void); 
void BSP_BC25_NB_pStart(void);
void BSP_BC25_NB_HRest(void);
void BSP_BC25_Loop(void);
void BSP_BC25_NB_PSMwakeup(void);

void BSP_BC25_Send(uint8_t *buf , uint16_t len );
 
void BSP_BC25_Report(uint8_t *buf ,uint16_t len); 
 
void BSP_BC25_CMD_InQueue(uint8_t at_cmd);
uint8_t BSP_BC25_CMD_OutQueue(void);
void BSP_BC25_T_InQueue(uint8_t * buf ,uint16_t len);
uint8_t BSP_BC25_T_OutQueue(uint8_t * buf , uint16_t * len);
void BSP_BC25_R_InQueue(uint8_t * buf , uint16_t len);
uint8_t BSP_BC25_R_OutQueue(uint8_t * buf , uint16_t * len);
uint8_t BSP_BC25_R_GetQueueCount(void);
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
