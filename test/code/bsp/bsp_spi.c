/**
 **************************************************************************************************
 * @file        bsp_spi.c
 * @author
 * @version
 * @date        5/28/2016
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */

#include "bsp_conf.h"
#include "bsp_spi.h"
#include "clog.h"

/**
 * @addtogroup    XXX 
 * @{  
 */

/**
 * @addtogroup    bsp_spi_Modules 
 * @{  
 */

/**
 * @defgroup      bsp_spi_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_spi_Macros_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_spi_Constants_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_spi_Private_Types
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_spi_Private_Variables 
 * @brief         
 * @{  
 */
dspi_master_handle_t g_m_sp1_handle;
/**
 * @}
 */

/**
 * @defgroup      bsp_spi_Public_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_spi_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */
static void bsp_spi1_init(void);
static void DSPI1_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData);


/**
 * @}
 */

/**
 * @defgroup      bsp_spi_Functions 
 * @brief         
 * @{  
 */


void BSP_SPI_Init(uint8_t BSP_SPIx)
{
	switch(BSP_SPIx)
	{
		case BSP_SPI0:
			{
			}break;
		case BSP_SPI1:
			{
				bsp_spi1_init();
			}break;
			default:break;
	}
}

static void bsp_spi1_init(void)
{

    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTE0  is configured as SPI1_PCS1 */
    PORT_SetPinMux(PORTE, 0U, kPORT_MuxAlt2);

    /* PORTE1  is configured as SPI1_SOUT */
    PORT_SetPinMux(PORTE, 1U, kPORT_MuxAlt2);

    /* PORTE2  is configured as SPI1_SCK */
    PORT_SetPinMux(PORTE, 2U, kPORT_MuxAlt2);

    /* PORTE3  is configured as SPI1_SIN */
    PORT_SetPinMux(PORTE, 3U, kPORT_MuxAlt2);
	
	
	dspi_master_config_t masterConfig;
    dspi_transfer_t masterXfer;

    /* Master config */
    masterConfig.whichCtar = kDSPI_Ctar0;
    masterConfig.ctarConfig.baudRate = 2000000U;
    masterConfig.ctarConfig.bitsPerFrame = 8U;
    masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
    masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
    masterConfig.ctarConfig.direction = kDSPI_MsbFirst;
    masterConfig.ctarConfig.pcsToSckDelayInNanoSec = 1000000000U / 2000000U;
    masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec = 500;
    masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 500;

    masterConfig.whichPcs = kDSPI_Pcs1;
    masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

    masterConfig.enableContinuousSCK = false;
    masterConfig.enableRxFifoOverWrite = false;
    masterConfig.enableModifiedTimingFormat = false;
    masterConfig.samplePoint = kDSPI_SckToSin0Clock;


    DSPI_MasterInit(SPI1, &masterConfig, DSPI0_CLK_SRC);	
	
	DSPI_MasterTransferCreateHandle(SPI1, &g_m_sp1_handle, DSPI1_MasterUserCallback, NULL);
	
	
}

static void DSPI1_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        __NOP();
    }

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

