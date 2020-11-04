/**
 **************************************************************************************************
 * @file        bsp_led.h
 * @author
 * @version    v0.1.0
 * @date        
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */
#ifndef _BSP_LED_H_
#define _BSP_LED_H_

/**
 * @addtogroup    XXX 
 * @{ 
 */
#include "self_def.h"
/**
 * @addtogroup    bsp_led_Modules 
 * @{  
 */

/**
 * @defgroup      bsp_led_Exported_Macros 
 * @{  
 */

#define BSP_COUNT       1
#define BSP_LED_0       0
#define BSP_LED_1       1
#define BSP_LED_2      	2
#define BSP_LED_3       3
#define BSP_LED_4       4
#define BSP_LED_5       5
#define BSP_LED_6       6


 // --------User Define ---------
#define BSP_LED_TEST    BSP_LED_0

#define BSP_LED_DATA 			BSP_LED_0
#define BSP_LED_RANGE			BSP_LED_1
#define BSP_LED_C				BSP_LED_2
#define BSP_LED_D				BSP_LED_3
#define BSP_LED_TEST_UNIT		BSP_LED_4
#define BSP_LED_STANDARD_UINT	BSP_LED_5
#define BSP_LED_EXCITATION		BSP_LED_6 

 // --------End User Define------
/**
 * @}
 */

/**
 * @defgroup      bsp_led_Exported_Constants
 * @{  
 */

/**
 * @}
 */
 
/**
 * @defgroup      bsp_led_Exported_Types 
 * @{  
 */

typedef enum
{
	Blink_HighSpeed = 1,
	Blink_MidSpeed ,
	Blink_LowSpeed , 
}BSP_LED_FuncBlink_e;
/**
 * @}
 */

/**
 * @defgroup      bsp_led_Exported_Variables 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_led_Exported_Functions 
 * @{  
 */

void Bsp_LedInit(void);
void Bsp_LedOpen(uint8_t bsp_ledx);
void Bsp_LedClose(uint8_t bsp_ledx);
void Bsp_LedToggle(uint8_t bsp_ledx);


void BSP_Led_UpdateStatus(void); // let this func in 10ms
void BSP_Led_Blink(uint8_t bsp_ledx , uint8_t blink_count , uint8_t duty_cycle , uint16_t period);
void BSP_LED_BlinkStandard(uint8_t bsp_ledx ,uint8_t count , BSP_LED_FuncBlink_e funcblink);

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
