/**
 **************************************************************************************************
 * @file        led_task.h
 * @author
 * @version    v0.1.0
 * @date        
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */
#ifndef _LED_TASK_H_
#define _LED_TASK_H_

/**
 * @addtogroup    XXX 
 * @{ 
 */
#include "self_def.h"
/**
 * @addtogroup    led_task_Modules 
 * @{  
 */

/**
 * @defgroup      led_task_Exported_Macros 
 * @{  
 */
#define LED_TASK_TEST_EVENT 			(0X01 << 0)
#define LED_TASK_TEST2_EVENT			(0X01 << 1)
/**
 * @}
 */

/**
 * @defgroup      led_task_Exported_Constants
 * @{  
 */

/**
 * @}
 */
 
/**
 * @defgroup      led_task_Exported_Types 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      led_task_Exported_Variables 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      led_task_Exported_Functions 
 * @{  
 */
uint32_t Led_Task_Init(void); 
void Led_Task(void * pvParameter);
void Led_Task_Event_Start(uint32_t events, uint8_t event_from);
void Led_Task_Tim_Init(void);
void Led_Task_StartTim(uint16_t time_count);

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


