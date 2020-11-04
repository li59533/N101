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


static dspi_transfer_t dspi1_transfer;
static uint8_t tx_testbuf[2] = {0x00 ,0x00 };
static uint8_t rx_testbuf[2] = {0x00 ,0x00 };

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
//	gpio_pin_config_t config =
//	{
//		kGPIO_DigitalOutput,
//		0,
//	};
    PORT_SetPinMux(PORTE, 0U, kPORT_MuxAlt2);
//	GPIO_PinInit( GPIOE , 0, &config);	
//	GPIO_PinWrite(GPIOE, 0, 1);
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
    masterConfig.ctarConfig.bitsPerFrame = 16U;
    masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
    masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseSecondEdge;
    masterConfig.ctarConfig.direction = kDSPI_MsbFirst;
    masterConfig.ctarConfig.pcsToSckDelayInNanoSec = 1000;
    masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec = 1000;
    masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000;

    masterConfig.whichPcs = kDSPI_Pcs1;
    masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

    masterConfig.enableContinuousSCK = false;
    masterConfig.enableRxFifoOverWrite = false;
    masterConfig.enableModifiedTimingFormat = false;
    masterConfig.samplePoint = kDSPI_SckToSin0Clock;


    DSPI_MasterInit(SPI1, &masterConfig, CLOCK_GetFreq(DSPI1_CLK_SRC));	
	
	DSPI_MasterTransferCreateHandle(SPI1, &g_m_sp1_handle, DSPI1_MasterUserCallback, NULL);
	
	//NVIC_SetPriority( SPI1_IRQn , 1U);
	
	DSPI_Enable(SPI1,  true);
	
	
	dspi1_transfer.txData = tx_testbuf;
	dspi1_transfer.rxData = rx_testbuf;
	dspi1_transfer.dataSize = 2;
	dspi1_transfer.configFlags = kDSPI_MasterPcs1;
}

static void DSPI1_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
		DEBUG("DSPI1_MasterUserCallback\r\n");
		//GPIO_PinWrite(GPIOE, 0, 1);
		uint32_t read_buf;
		read_buf = DSPI_ReadData(SPI1);
		DEBUG("SPI rev:%X\r\n" ,read_buf );
        __NOP();
    }
}

void BSP_SPI_Send(uint8_t *buf , uint8_t len )
{
	dspi_transfer_t dspi_transfer;
	
	tx_testbuf[1] = tx_testbuf[1] | 0x32;
	dspi_transfer.txData = tx_testbuf;
	dspi_transfer.rxData = rx_testbuf;
	dspi_transfer.dataSize = 7;
	dspi_transfer.configFlags = kDSPI_MasterPcs1;
	
	//GPIO_PinWrite(GPIOE, 0, 0);
	DSPI_MasterTransferNonBlocking(SPI1, &g_m_sp1_handle , &dspi_transfer);
	//DSPI_MasterTransferBlocking(SPI1, &dspi_transfer);
}

// ------ Transfer ------
void BSP_SPI_WriteByte(uint8_t addr , uint8_t val)
{
	dspi1_transfer.txData[0] = val ;
	dspi1_transfer.txData[1] = addr ; 
	//DSPI_MasterTransferNonBlocking(SPI1, &g_m_sp1_handle , &dspi1_transfer);
	
	DSPI_MasterTransferBlocking(SPI1 , &dspi1_transfer);
}

void BSP_SPI_ReadByte(uint8_t addr)
{
	dspi1_transfer.txData[0] = 0x00 ;
	dspi1_transfer.txData[1] = addr | 0x80; 
	//DSPI_MasterTransferNonBlocking(SPI1, &g_m_sp1_handle , &dspi1_transfer);
	DSPI_MasterTransferBlocking(SPI1 , &dspi1_transfer);
	
	
}


uint8_t BSP_SPI_ReadByte_Test(uint8_t addr)
{
	uint8_t val = 0;
	dspi1_transfer.txData[0] = 0x00 ;
	dspi1_transfer.txData[1] = addr | 0x80; 
	//DSPI_MasterTransferNonBlocking(SPI1, &g_m_sp1_handle , &dspi1_transfer);
	DSPI_MasterTransferBlocking(SPI1 , &dspi1_transfer);	
	val = (uint8_t ) DSPI_ReadData(SPI1);
	return val ;
}


// ------ IRQ ------

void SPI1_IRQHandler()
{
	DEBUG("SPI1_DriverIRQHandler\r\n");
	DSPI_MasterTransferHandleIRQ( SPI1 , &g_m_sp1_handle);
}
/*
// -----------------
void BSP_SPI_Write16Bits(uint8_t bsp_spix , uint16_t data)
{
	SPI_EnableInterrupts(SPI0 , kSPI_TxEmptyInterruptEnable);	
	SPI_WriteData(SPI0, data);
	
//	SPI_WriteBlocking(SPI0, uint8_t *buffer, size_t size)
	
}
//static uint16_t spivalue = 0;
void SPI0_IRQHandler(void)
{
	//DEBUG("SPI0_IRQHandler\r\n");
	if((SPI_GetStatusFlags(SPI0) & kSPI_TxBufferEmptyFlag)== kSPI_TxBufferEmptyFlag)
	{
		BSP_AD7682_GetValue(SPI_ReadData(SPI0));
		SPI_DisableInterrupts(SPI0, kSPI_TxEmptyInterruptEnable);

		//DEBUG("SPI0 : value->%d\r\n" , spivalue);
	}	
}

*/ 


/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

