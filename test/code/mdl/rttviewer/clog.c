/**
 **************************************************************************************************
 * @file        clog.c
 * @author
 * @version   v0.1.0
 * @date        
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */
#include "clog.h"
#include "self_def.h"
/**
 * @addtogroup    XXX 
 * @{  
 */

/**
 * @addtogroup    clog_Modules 
 * @{  
 */

/**
 * @defgroup      clog_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      clog_Macros_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */
 
/**
 * @defgroup      clog_Constants_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      clog_Private_Types
 * @brief         
 * @{  
 */

/**
 * @}
 */
 
/**
 * @defgroup      clog_Private_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */
 
/**
 * @defgroup      clog_Public_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      clog_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      clog_Functions 
 * @brief         
 * @{  
 */
void Clog_Float(const char * buf ,float  value)
{
	char cbuf[30];
	snprintf( cbuf , 30 , "%s%f\r\n" , buf , value);
	DEBUG("%s" , cbuf);
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

