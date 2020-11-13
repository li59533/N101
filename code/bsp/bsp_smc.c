/**
 **************************************************************************************************
 * @file        bsp_smc.c
 * @author
 * @version
 * @date        5/28/2016
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */


#include "bsp_smc.h"

#include "clog.h"
/**
 * @addtogroup    XXX 
 * @{  
 */

/**
 * @addtogroup    bsp_smc_Modules 
 * @{  
 */

/**
 * @defgroup      bsp_smc_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_smc_Macros_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_smc_Constants_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_smc_Private_Types
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_smc_Private_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_smc_Public_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_smc_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */

void (*bsp_PowerPreSwitch)(smc_power_state_t originPowerState, app_power_mode_t targetMode);
void (*bsp_PowerPostSwitch)(smc_power_state_t originPowerState, app_power_mode_t targetMode);

/**
 * @}
 */

/**
 * @defgroup      bsp_smc_Functions 
 * @brief         
 * @{  
 */
void APP_SetClockVlpr(void)
{
    const sim_clock_config_t simConfig = {
        .pllFllSel = 3U,        /* PLLFLLSEL select IRC48MCLK. */
        .er32kSrc = 5U,         /* ERCLK32K selection, use RTC. */
        .clkdiv1 = 0x00040000U, /* SIM_CLKDIV1. */
    };

    CLOCK_SetSimSafeDivs();
    CLOCK_SetInternalRefClkConfig(kMCG_IrclkEnable, kMCG_IrcFast, 0U);

    /* MCG works in PEE mode now, will switch to BLPI mode. */

    CLOCK_ExternalModeToFbeModeQuick();  /* Enter FBE. */
    CLOCK_SetFbiMode(kMCG_Dmx32Default, kMCG_DrsLow, NULL); /* Enter FBI. */
    CLOCK_SetLowPowerEnable(true);       /* Enter BLPI. */

    CLOCK_SetSimConfig(&simConfig);
}

void APP_SetClockRunFromVlpr(void)
{
    const sim_clock_config_t simConfig = {
        .pllFllSel = 1U,        /* PLLFLLSEL select PLL. */
        .er32kSrc = 5U,         /* ERCLK32K selection, use RTC. */
        .clkdiv1 = 0x01040000U, /* SIM_CLKDIV1. */
    };

    const mcg_pll_config_t pll0Config = {
        .enableMode = 0U, .prdiv = 0x1U, .vdiv = 0x6U,
    };

    CLOCK_SetSimSafeDivs();

    /* Currently in BLPI mode, will switch to PEE mode. */
    /* Enter FBI. */
    CLOCK_SetLowPowerEnable(false);
    /* Enter FBE. */
    CLOCK_SetFbeMode(3U, kMCG_Dmx32Default, kMCG_DrsLow, NULL);
    /* Enter PBE. */
    CLOCK_SetPbeMode(kMCG_PllClkSelPll0, &pll0Config);
    /* Enter PEE. */
    CLOCK_SetPeeMode();

    CLOCK_SetSimConfig(&simConfig);
}


/*
 * Power mode switch.
 */
void APP_PowerModeSwitch(smc_power_state_t curPowerState, app_power_mode_t targetPowerMode)
{
    smc_power_mode_vlls_config_t vlls_config;
    vlls_config.enablePorDetectInVlls0 = true;

    switch (targetPowerMode)
    {
        case kAPP_PowerModeVlpr:
            APP_SetClockVlpr();
            SMC_SetPowerModeVlpr(SMC, false);
            while (kSMC_PowerStateVlpr != SMC_GetPowerModeState(SMC))
            {
            }
            break;

        case kAPP_PowerModeRun:

            /* Power mode change. */
            SMC_SetPowerModeRun(SMC);
            while (kSMC_PowerStateRun != SMC_GetPowerModeState(SMC))
            {
            }

            /* If enter RUN from VLPR, change clock after the power mode change. */
            if (kSMC_PowerStateVlpr == curPowerState)
            {
                APP_SetClockRunFromVlpr();
            }
            break;


        case kAPP_PowerModeWait:
            SMC_PreEnterWaitModes();
            SMC_SetPowerModeWait(SMC);
            SMC_PostExitWaitModes();
            break;

        case kAPP_PowerModeStop:
            SMC_PreEnterStopModes();
            SMC_SetPowerModeStop(SMC, kSMC_PartialStop);
            SMC_PostExitStopModes();
            break;

        case kAPP_PowerModeVlpw:
            SMC_PreEnterWaitModes();
            SMC_SetPowerModeVlpw(SMC);
            SMC_PostExitWaitModes();
            break;

        case kAPP_PowerModeVlps:
            SMC_PreEnterStopModes();
            SMC_SetPowerModeVlps(SMC);
            SMC_PostExitStopModes();
            break;

        case kAPP_PowerModeLls:
            SMC_PreEnterStopModes();
            SMC_SetPowerModeLls(SMC);
            SMC_PostExitStopModes();
            break;

        case kAPP_PowerModeVlls0:
            vlls_config.subMode = kSMC_StopSub0;
            SMC_PreEnterStopModes();
            SMC_SetPowerModeVlls(SMC, &vlls_config);
            SMC_PostExitStopModes();
            break;

        case kAPP_PowerModeVlls1:
            vlls_config.subMode = kSMC_StopSub1;
            SMC_PreEnterStopModes();
            SMC_SetPowerModeVlls(SMC, &vlls_config);
            SMC_PostExitStopModes();
            break;

        case kAPP_PowerModeVlls2:
            vlls_config.subMode = kSMC_StopSub2;
            SMC_PreEnterStopModes();
            SMC_SetPowerModeVlls(SMC, &vlls_config);
            SMC_PostExitStopModes();
            break;

        case kAPP_PowerModeVlls3:
            vlls_config.subMode = kSMC_StopSub3;
            SMC_PreEnterStopModes();
            SMC_SetPowerModeVlls(SMC, &vlls_config);
            SMC_PostExitStopModes();
            break;

        default:
            DEBUG("Wrong value\r\n");
            break;
    }
}

void APP_ShowPowerMode(smc_power_state_t powerMode)
{
    switch (powerMode)
    {
        case kSMC_PowerStateRun:
            DEBUG("    Power mode: RUN\r\n");
            break;
        case kSMC_PowerStateVlpr:
            DEBUG("    Power mode: VLPR\r\n");
            break;
        default:
            DEBUG("    Power mode wrong\r\n");
            break;
    }
}

void BSP_SMC_SetPowerModeProtection(void)
{
	SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
}

void BSP_SMC_PowerPre_CallbackReg(void (*callback)(smc_power_state_t originPowerState, app_power_mode_t targetMode))
{
	bsp_PowerPreSwitch = callback;
}

void BSP_SMC_PowerPost_CallbackReg(void (*callback)(smc_power_state_t originPowerState, app_power_mode_t targetMode))
{
	bsp_PowerPostSwitch = callback;
}

void BSP_SMC_EnterLLS(void)
{
	bsp_PowerPreSwitch(kSMC_PowerStateRun,kAPP_PowerModeLls);
	APP_PowerModeSwitch(kSMC_PowerStateRun, kAPP_PowerModeLls);
	bsp_PowerPostSwitch( kSMC_PowerStateLls, kAPP_PowerModeRun);
}


// ------------ TestCode ------------
void BSP_SMC_TestCode(void)
{
    uint32_t freq = 0;
    uint8_t ch;
    smc_power_state_t curPowerState;	
	
	curPowerState = SMC_GetPowerModeState(SMC);

	freq = CLOCK_GetFreq(kCLOCK_CoreSysClk);

	DEBUG("\r\n####################  Power Mode Switch Demo ####################\n\r\n");
	DEBUG("    Core Clock = %dHz \r\n", freq);

	APP_ShowPowerMode(curPowerState);	
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

