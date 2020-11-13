/**
 **************************************************************************************************
 * @file        bsp_smc.h
 * @author
 * @version
 * @date        5/28/2016
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */
#ifndef _BSP_SMC_H_
#define _BSP_SMC_H_

/**
 * @addtogroup    XXX 
 * @{ 
 */
#include "self_def.h"
#include "bsp_conf.h"
/**
 * @addtogroup    bsp_smc_Modules 
 * @{  
 */

/**
 * @defgroup      bsp_smc_Exported_Macros 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_smc_Exported_Constants
 * @{  
 */
typedef enum _app_power_mode
{
    kAPP_PowerModeMin = 'A' - 1,
    kAPP_PowerModeRun,  /* Normal RUN mode */
    kAPP_PowerModeWait, /* WAIT mode. */
    kAPP_PowerModeStop, /* STOP mode. */
    kAPP_PowerModeVlpr, /* VLPR mode. */
    kAPP_PowerModeVlpw, /* VLPW mode. */
    kAPP_PowerModeVlps, /* VLPS mode. */
    kAPP_PowerModeLls, /* LLS mode. */
    kAPP_PowerModeVlls0, /* VLLS0 mode. */
    kAPP_PowerModeVlls1, /* VLLS1 mode. */
    kAPP_PowerModeVlls2, /* VLLS2 mode. */
    kAPP_PowerModeVlls3, /* VLLS3 mode. */
    kAPP_PowerModeMax
} app_power_mode_t;

/**
 * @}
 */

/**
 * @defgroup      bsp_smc_Exported_Types 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_smc_Exported_Variables 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_smc_Exported_Functions 
 * @{  
 */
void APP_SetClockVlpr(void);
void APP_SetClockRunFromVlpr(void);
void APP_PowerModeSwitch(smc_power_state_t curPowerState, app_power_mode_t targetPowerMode);
void BSP_SMC_SetPowerModeProtection(void);
void APP_ShowPowerMode(smc_power_state_t powerMode);
void BSP_SMC_PowerPre_CallbackReg(void (*callback)(smc_power_state_t originPowerState, app_power_mode_t targetMode));
void BSP_SMC_PowerPost_CallbackReg(void (*callback)(smc_power_state_t originPowerState, app_power_mode_t targetMode));





// ------------ TestCode ------------
void BSP_SMC_TestCode(void);



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
