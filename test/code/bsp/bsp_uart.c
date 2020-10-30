/**
 **************************************************************************************************
 * @file        bsp_uart.c
 * @author
 * @version
 * @date        
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */
#include "bsp_conf.h"
#include "bsp_uart.h"
#include "clog.h"
/**
 * @addtogroup    XXX 
 * @{  
 */
#include "modbus_task.h"
#include "modbus_rtu.h"
#include "system_param.h"
/**
 * @addtogroup    bsp_uart_Modules 
 * @{  
 */

/**
 * @defgroup      bsp_uart_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup       bsp_uart_Macros_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_uart_Constants_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup       bsp_uart_Private_Types
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_uart_Private_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_uart_Public_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_uart_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */
static void bsp_uart0_init(void);
static void bsp_uart1_init(void);
static void bsp_uart2_init(void);
static void bsp_uart3_init(void);
static void bsp_uart4_init(void);

static void UART0_UserCallback(UART_Type *base, uart_edma_handle_t *handle, status_t status, void *userData);
static void UART1_UserCallback(UART_Type *base, uart_edma_handle_t *handle, status_t status, void *userData);
static void UART3_UserCallback(UART_Type *base, uart_edma_handle_t *handle, status_t status, void *userData);
static void UART4_UserCallback(UART_Type *base, uart_edma_handle_t *handle, status_t status, void *userData);
/**
 * @}
 */

/**
 * @defgroup      bsp_uart_Functions 
 * @brief         
 * @{  
 */
/*
PE3 -> TX1  ALT3
PE4 -> RX1  ALT3

RX0 PTA15 
TX0 PTA14

RX3 PTC16
TX3 PTC17

*/


void BSP_UART_Init(uint8_t BSP_UARTX)
{
	switch(BSP_UARTX)
	{
		case BSP_UART0: bsp_uart0_init();break;
		case BSP_UART1: bsp_uart1_init();break;
		case BSP_UART2: bsp_uart2_init();break;
		case BSP_UART3: bsp_uart3_init();break;
		case BSP_UART4: bsp_uart4_init();break;
		default:break;
	}
}

void BSP_UART_SetBaudRate(uint8_t BSP_UARTX , uint32_t buadrate)
{
	switch(BSP_UARTX)
	{
		case BSP_UART0: 
		{

		}
		break;
		case BSP_UART1:  UART_SetBaudRate(UART1, buadrate,  CLOCK_GetFreq(kCLOCK_CoreSysClk));;break;
		case BSP_UART2: bsp_uart2_init();break;
		case BSP_UART3: bsp_uart3_init();break;
		default:break;
	}
}
	 

void BSP_Uart0_Close(void)
{

}

void BSP_Uart0_Open(void)
{
	BSP_UART_Init( BSP_UART0 );
	//BSP_UART_SetBaudRate(BSP_UART0 , 115200);
}


/* UART user callback */
static void UART0_UserCallback(UART_Type *base, uart_edma_handle_t *handle, status_t status, void *userData)
{
    userData = userData;

    if (kStatus_UART_TxIdle == status)
    {
		DEBUG("kStatus_UART0_TxIdle\r\n");
    }

    if (kStatus_UART_RxIdle == status)
    {
		DEBUG("kStatus_UART0_RxIdle\r\n");
    }
}


/*
RX0 PTA15 
TX0 PTA14
*/
#define SOPT5_UART0TXSRC_UART_TX 0x00u /*!<@brief UART 1 transmit data source select: UART1_TX pin */
edma_handle_t g_uart0TxEdmaHandle;
edma_handle_t g_uart0RxEdmaHandle;
uart_edma_handle_t g_uart0EdmaHandle;
uart_handle_t g_uart0_handle;
static void bsp_uart0_init(void)
{
	uart_config_t uartConfig;
	edma_config_t config;
	// -------gpio init ------
    /* Port A Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortA);

    /* PTA14  is configured as UART0_TX */
    PORT_SetPinMux(PORTA, 14U, kPORT_MuxAlt3);

    /* PTA15  is configured as UART0_RX */
    PORT_SetPinMux(PORTA, 15U, kPORT_MuxAlt3);

    SIM->SOPT5 = ((SIM->SOPT5 &
                   /* Mask bits to zero which are setting */
                   (~(SIM_SOPT5_UART0TXSRC_MASK)))

                  /* UART 0 transmit data source select: UART0_TX pin. */
                  | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX));
	// -----------------------		
	
	// -------uart 0 init ----
    /* Initialize the UART. */
    /*
     * uartConfig.baudRate_Bps = 115200U;
     * uartConfig.parityMode = kUART_ParityDisabled;
     * uartConfig.stopBitCount = kUART_OneStopBit;
     * uartConfig.txFifoWatermark = 0;
     * uartConfig.rxFifoWatermark = 1;
     * uartConfig.enableTx = false;
     * uartConfig.enableRx = false;
     */
    UART_GetDefaultConfig(&uartConfig);
	uartConfig.baudRate_Bps = 9600;
	uartConfig.parityMode = kUART_ParityDisabled;
	uartConfig.stopBitCount = kUART_OneStopBit;
	
	/*
	switch(g_SystemParam_Config.BaudRate_Bps)
	{
		case 0 : uartConfig.baudRate_Bps = 2400;break;
		case 1 : uartConfig.baudRate_Bps = 4800;break;
		case 2 : uartConfig.baudRate_Bps = 9600;break;
		case 3 : uartConfig.baudRate_Bps = 14400;break;
		case 4 : uartConfig.baudRate_Bps = 19200;break;
		case 5 : uartConfig.baudRate_Bps = 38400;break;
		case 6 : uartConfig.baudRate_Bps = 56000;break;
		case 7 : uartConfig.baudRate_Bps = 115200;break;
		default : uartConfig.baudRate_Bps = 9600;break;
	}	
	
	switch(g_SystemParam_Config.ParityMode)
	{
		case 0 : uartConfig.parityMode = kUART_ParityDisabled;break;
		case 1 : uartConfig.parityMode = kUART_ParityEven;break;
		case 2 : uartConfig.parityMode = kUART_ParityOdd;break;
		default : uartConfig.parityMode = kUART_ParityDisabled;break;
	}
	
	switch(g_SystemParam_Config.StopBitCount)
	{
		case 1 : uartConfig.stopBitCount = kUART_OneStopBit;break;
		case 2 : uartConfig.stopBitCount = kUART_TwoStopBit;break;
		default : uartConfig.stopBitCount = kUART_OneStopBit;break;
	}
	*/
	
	
    uartConfig.enableTx = true;
    uartConfig.enableRx = true;

    UART_Init(UART0, &uartConfig, CLOCK_GetFreq(kCLOCK_CoreSysClk));	
	
	
	// -----------------------
	
	// --------open irq-------
	UART_EnableInterrupts( UART0 ,kUART_TransmissionCompleteInterruptEnable);
	UART_EnableInterrupts( UART0 ,kUART_RxDataRegFullInterruptEnable);
	NVIC_SetPriority(UART0_RX_TX_IRQn , 7);
	EnableIRQ(UART0_RX_TX_IRQn);
	// -----------------------
	
// -----------DMA ------------------------
  /* Init DMAMUX */
    DMAMUX_Init(DMAMUX0);
    /* Set channel for UART */
    DMAMUX_SetSource(DMAMUX0, 0, kDmaRequestMux0UART0Tx);
    
    DMAMUX_EnableChannel(DMAMUX0, 0);


    /* Init the EDMA module */
    EDMA_GetDefaultConfig(&config);
    EDMA_Init(DMA0, &config);
    EDMA_CreateHandle(&g_uart0TxEdmaHandle, DMA0, 0);

    /* Create UART DMA handle. */
    UART_TransferCreateHandleEDMA(UART0, &g_uart0EdmaHandle, UART0_UserCallback, NULL, &g_uart0TxEdmaHandle,
                                  &g_uart0RxEdmaHandle);
	NVIC_SetPriority(DMA0_IRQn , 7);
// ---------------------------------------	
	
}


/* UART1 user callback */
static void UART1_UserCallback(UART_Type *base, uart_edma_handle_t *handle, status_t status, void *userData)
{
    userData = userData;

    if (kStatus_UART_TxIdle == status)
    {
		DEBUG("kStatus_UART1_TxIdle\r\n");
    }

    if (kStatus_UART_RxIdle == status)
    {
		DEBUG("kStatus_UART1_RxIdle\r\n");
    }
}


#define SOPT5_UART1TXSRC_UART_TX 0x00u /*!<@brief UART 1 transmit data source select: UART1_TX pin */
edma_handle_t g_uart1TxEdmaHandle;
edma_handle_t g_uart1RxEdmaHandle;
uart_edma_handle_t g_uart1EdmaHandle;
uart_handle_t g_uart1_handle;
static void bsp_uart1_init(void)
{
	uart_config_t uartConfig;
	edma_config_t config;
	// -------gpio init ------
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTE0 (pin E4) is configured as UART1_TX */
    PORT_SetPinMux(PORTE, 0U, kPORT_MuxAlt3);

    /* PORTE1 (pin E3) is configured as UART1_RX */
    PORT_SetPinMux(PORTE, 1U, kPORT_MuxAlt3);

    SIM->SOPT5 = ((SIM->SOPT5 &
                   /* Mask bits to zero which are setting */
                   (~(SIM_SOPT5_UART1TXSRC_MASK)))

                  /* UART 1 transmit data source select: UART1_TX pin. */
                  | SIM_SOPT5_UART1TXSRC(SOPT5_UART1TXSRC_UART_TX));
	// -----------------------		
	
	// -------uart 0 init ----
    /* Initialize the UART. */
    /*
     * uartConfig.baudRate_Bps = 115200U;
     * uartConfig.parityMode = kUART_ParityDisabled;
     * uartConfig.stopBitCount = kUART_OneStopBit;
     * uartConfig.txFifoWatermark = 0;
     * uartConfig.rxFifoWatermark = 1;
     * uartConfig.enableTx = false;
     * uartConfig.enableRx = false;
     */
    UART_GetDefaultConfig(&uartConfig);
	
	switch(g_SystemParam_Config.BaudRate_Bps)
	{
		case 0 : uartConfig.baudRate_Bps = 2400;break;
		case 1 : uartConfig.baudRate_Bps = 4800;break;
		case 2 : uartConfig.baudRate_Bps = 9600;break;
		case 3 : uartConfig.baudRate_Bps = 14400;break;
		case 4 : uartConfig.baudRate_Bps = 19200;break;
		case 5 : uartConfig.baudRate_Bps = 38400;break;
		case 6 : uartConfig.baudRate_Bps = 56000;break;
		case 7 : uartConfig.baudRate_Bps = 115200;break;
		default : uartConfig.baudRate_Bps = 9600;break;
	}	
	
	switch(g_SystemParam_Config.ParityMode)
	{
		case 0 : uartConfig.parityMode = kUART_ParityDisabled;break;
		case 1 : uartConfig.parityMode = kUART_ParityEven;break;
		case 2 : uartConfig.parityMode = kUART_ParityOdd;break;
		default : uartConfig.parityMode = kUART_ParityDisabled;break;
	}
	switch(g_SystemParam_Config.StopBitCount)
	{
		case 1 : uartConfig.stopBitCount = kUART_OneStopBit;break;
		case 2 : uartConfig.stopBitCount = kUART_TwoStopBit;break;
		default : uartConfig.stopBitCount = kUART_OneStopBit;break;
	}
	
    uartConfig.enableTx = true;
    uartConfig.enableRx = true;

    UART_Init(UART1, &uartConfig, CLOCK_GetFreq(kCLOCK_CoreSysClk));	
	
	
	// -----------------------
	
	// --------open irq-------
	UART_EnableInterrupts( UART1 ,kUART_TransmissionCompleteInterruptEnable);
	UART_EnableInterrupts( UART1 ,kUART_RxDataRegFullInterruptEnable);
	NVIC_SetPriority(UART1_RX_TX_IRQn , 7);
	EnableIRQ(UART1_RX_TX_IRQn);
	// -----------------------
	
// -----------DMA ------------------------
  /* Init DMAMUX */
    DMAMUX_Init(DMAMUX0);
    /* Set channel for UART */
    DMAMUX_SetSource(DMAMUX0, 0, kDmaRequestMux0UART1Tx);
    
    DMAMUX_EnableChannel(DMAMUX0, 0);


    /* Init the EDMA module */
    EDMA_GetDefaultConfig(&config);
    EDMA_Init(DMA0, &config);
    EDMA_CreateHandle(&g_uart1TxEdmaHandle, DMA0, 0);

    /* Create UART DMA handle. */
    UART_TransferCreateHandleEDMA(UART1, &g_uart1EdmaHandle, UART1_UserCallback, NULL, &g_uart1TxEdmaHandle,
                                  &g_uart1RxEdmaHandle);
	NVIC_SetPriority(DMA0_IRQn , 7);
// ---------------------------------------	
	
}

static void bsp_uart2_init(void)
{

}



/* UART3 user callback */
static void UART3_UserCallback(UART_Type *base, uart_edma_handle_t *handle, status_t status, void *userData)
{
    userData = userData;

    if (kStatus_UART_TxIdle == status)
    {
		DEBUG("kStatus_UART3_TxIdle\r\n");
    }

    if (kStatus_UART_RxIdle == status)
    {
		DEBUG("kStatus_UART3_RxIdle\r\n");
    }
}

/*
RX3 PTC16 
TX3 PTC17
*/

edma_handle_t g_uart3TxEdmaHandle;
edma_handle_t g_uart3RxEdmaHandle;
uart_edma_handle_t g_uart3EdmaHandle;
uart_handle_t g_uart3_handle;
static void bsp_uart3_init(void)
{
	uart_config_t uartConfig;
	edma_config_t config;
	// -------gpio init ------
    /* Port C Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);

    /* PTC17 is configured as UART3_TX */
    PORT_SetPinMux(PORTC, 17U, kPORT_MuxAlt3);

    /* PTC16 is configured as UART3_RX */
    PORT_SetPinMux(PORTC, 16U, kPORT_MuxAlt3);

	// -----------------------		
	
	// ------ uart 3 init ----
    /* Initialize the UART. */
    /*
     * uartConfig.baudRate_Bps = 115200U;
     * uartConfig.parityMode = kUART_ParityDisabled;
     * uartConfig.stopBitCount = kUART_OneStopBit;
     * uartConfig.txFifoWatermark = 0;
     * uartConfig.rxFifoWatermark = 1;
     * uartConfig.enableTx = false;
     * uartConfig.enableRx = false;
     */
    UART_GetDefaultConfig(&uartConfig);

	uartConfig.baudRate_Bps = 9600;
	uartConfig.parityMode = kUART_ParityDisabled;
	uartConfig.stopBitCount = kUART_OneStopBit;	
	
	
/*	
	switch(g_SystemParam_Config.BaudRate_Bps)
	{
		case 0 : uartConfig.baudRate_Bps = 2400;break;
		case 1 : uartConfig.baudRate_Bps = 4800;break;
		case 2 : uartConfig.baudRate_Bps = 9600;break;
		case 3 : uartConfig.baudRate_Bps = 14400;break;
		case 4 : uartConfig.baudRate_Bps = 19200;break;
		case 5 : uartConfig.baudRate_Bps = 38400;break;
		case 6 : uartConfig.baudRate_Bps = 56000;break;
		case 7 : uartConfig.baudRate_Bps = 115200;break;
		default : uartConfig.baudRate_Bps = 9600;break;
	}	
	
	switch(g_SystemParam_Config.ParityMode)
	{
		case 0 : uartConfig.parityMode = kUART_ParityDisabled;break;
		case 1 : uartConfig.parityMode = kUART_ParityEven;break;
		case 2 : uartConfig.parityMode = kUART_ParityOdd;break;
		default : uartConfig.parityMode = kUART_ParityDisabled;break;
	}
	switch(g_SystemParam_Config.StopBitCount)
	{
		case 1 : uartConfig.stopBitCount = kUART_OneStopBit;break;
		case 2 : uartConfig.stopBitCount = kUART_TwoStopBit;break;
		default : uartConfig.stopBitCount = kUART_OneStopBit;break;
	}
*/	
    uartConfig.enableTx = true;
    uartConfig.enableRx = true;

    UART_Init(UART3, &uartConfig, CLOCK_GetFreq(kCLOCK_BusClk));	

	// -----------------------
	
	// --------open irq-------
	UART_EnableInterrupts( UART3 ,kUART_TransmissionCompleteInterruptEnable);
	UART_EnableInterrupts( UART3 ,kUART_RxDataRegFullInterruptEnable);
	NVIC_SetPriority(UART3_RX_TX_IRQn , 7);
	EnableIRQ(UART3_RX_TX_IRQn);
	// -----------------------
	
// -----------DMA ------------------------
  /* Init DMAMUX */
    DMAMUX_Init(DMAMUX0);
    /* Set channel for UART */
    DMAMUX_SetSource(DMAMUX0, 3, kDmaRequestMux0UART3Tx);
    
    DMAMUX_EnableChannel(DMAMUX0, 3);


    /* Init the EDMA module */
    EDMA_GetDefaultConfig(&config);
    EDMA_Init(DMA0, &config);
    EDMA_CreateHandle(&g_uart3TxEdmaHandle, DMA0, 3);

    /* Create UART DMA handle. */
    UART_TransferCreateHandleEDMA(UART3, &g_uart3EdmaHandle, UART3_UserCallback, NULL, &g_uart3TxEdmaHandle,
                                  &g_uart3RxEdmaHandle);
	NVIC_SetPriority(DMA3_IRQn , 7);
// ---------------------------------------	
}



/* UART4 user callback */
static void UART4_UserCallback(UART_Type *base, uart_edma_handle_t *handle, status_t status, void *userData)
{
    userData = userData;

    if (kStatus_UART_TxIdle == status)
    {
		DEBUG("kStatus_UART4_TxIdle\r\n");
    }

    if (kStatus_UART_RxIdle == status)
    {
		DEBUG("kStatus_UART4_RxIdle\r\n");
    }
}

/*
RX4 PTC14 
TX4 PTC15
*/

edma_handle_t g_uart4TxEdmaHandle;
edma_handle_t g_uart4RxEdmaHandle;
uart_edma_handle_t g_uart4EdmaHandle;
uart_handle_t g_uart4_handle;
static void bsp_uart4_init(void)
{
	uart_config_t uartConfig;
	edma_config_t config;
	// -------gpio init ------
    /* Port C Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);

    /* PTC15 is configured as UART4_TX */
    PORT_SetPinMux(PORTC, 15U, kPORT_MuxAlt3);

    /* PTC14 is configured as UART4_RX */
    PORT_SetPinMux(PORTC, 14U, kPORT_MuxAlt3);

	// -----------------------		
	
	// ------ uart 3 init ----
    /* Initialize the UART. */
    /*
     * uartConfig.baudRate_Bps = 115200U;
     * uartConfig.parityMode = kUART_ParityDisabled;
     * uartConfig.stopBitCount = kUART_OneStopBit;
     * uartConfig.txFifoWatermark = 0;
     * uartConfig.rxFifoWatermark = 1;
     * uartConfig.enableTx = false;
     * uartConfig.enableRx = false;
     */
    UART_GetDefaultConfig(&uartConfig);

	uartConfig.baudRate_Bps = 9600;
	uartConfig.parityMode = kUART_ParityDisabled;
	uartConfig.stopBitCount = kUART_OneStopBit;	
	
	
/*	
	switch(g_SystemParam_Config.BaudRate_Bps)
	{
		case 0 : uartConfig.baudRate_Bps = 2400;break;
		case 1 : uartConfig.baudRate_Bps = 4800;break;
		case 2 : uartConfig.baudRate_Bps = 9600;break;
		case 3 : uartConfig.baudRate_Bps = 14400;break;
		case 4 : uartConfig.baudRate_Bps = 19200;break;
		case 5 : uartConfig.baudRate_Bps = 38400;break;
		case 6 : uartConfig.baudRate_Bps = 56000;break;
		case 7 : uartConfig.baudRate_Bps = 115200;break;
		default : uartConfig.baudRate_Bps = 9600;break;
	}	
	
	switch(g_SystemParam_Config.ParityMode)
	{
		case 0 : uartConfig.parityMode = kUART_ParityDisabled;break;
		case 1 : uartConfig.parityMode = kUART_ParityEven;break;
		case 2 : uartConfig.parityMode = kUART_ParityOdd;break;
		default : uartConfig.parityMode = kUART_ParityDisabled;break;
	}
	switch(g_SystemParam_Config.StopBitCount)
	{
		case 1 : uartConfig.stopBitCount = kUART_OneStopBit;break;
		case 2 : uartConfig.stopBitCount = kUART_TwoStopBit;break;
		default : uartConfig.stopBitCount = kUART_OneStopBit;break;
	}
*/	
    uartConfig.enableTx = true;
    uartConfig.enableRx = true;

    UART_Init(UART4, &uartConfig, CLOCK_GetFreq(kCLOCK_BusClk));	

	// -----------------------
	
	// --------open irq-------
	UART_EnableInterrupts( UART4 ,kUART_TransmissionCompleteInterruptEnable);
	UART_EnableInterrupts( UART4 ,kUART_RxDataRegFullInterruptEnable);
	NVIC_SetPriority(UART4_RX_TX_IRQn , 7);
	EnableIRQ(UART4_RX_TX_IRQn);
	// -----------------------
	
// -----------DMA ------------------------
  /* Init DMAMUX */
    DMAMUX_Init(DMAMUX0);
    /* Set channel for UART */
    DMAMUX_SetSource(DMAMUX0, 4, kDmaRequestMux0UART4);
    
    DMAMUX_EnableChannel(DMAMUX0, 4);


    /* Init the EDMA module */
    EDMA_GetDefaultConfig(&config);
    EDMA_Init(DMA0, &config);
    EDMA_CreateHandle(&g_uart4TxEdmaHandle, DMA0, 4);

    /* Create UART DMA handle. */
    UART_TransferCreateHandleEDMA(UART4, &g_uart4EdmaHandle, UART4_UserCallback, NULL, &g_uart4TxEdmaHandle,
                                  &g_uart4RxEdmaHandle);
	NVIC_SetPriority(DMA4_IRQn , 7);
// ---------------------------------------	
}

// --------Function -------------

		// ----Send ----
			/*
				Uart0 blocking mode has some bug...
			*/
void BSP_UART_WriteBytes_Blocking(uint8_t BSP_UARTX , uint8_t *buf, uint16_t len)
{
	switch(BSP_UARTX)
	{
		case BSP_UART0 :UART_WriteBlocking(UART0, buf, len);break;
		case BSP_UART1 :UART_WriteBlocking(UART1, buf, len);break;
		case BSP_UART2 :UART_WriteBlocking(UART2, buf, len);break;
		case BSP_UART3 :UART_WriteBlocking(UART3, buf, len);break;
		case BSP_UART4 :UART_WriteBlocking(UART4, buf, len);break;
		default:break;
	}
}
		// --------------
static uint8_t uart0_send_space[2048] = {0};
static uint8_t uart1_send_space[300] = {0};
static uint8_t uart3_send_space[2048] = {0};
static uint8_t uart4_send_space[300] = {0};
void BSP_UART_WriteBytes_DMA(uint8_t BSP_UARTX , uint8_t *buf, uint16_t len)
{
	uart_transfer_t  xfer;
	switch(BSP_UARTX)
	{
		case BSP_UART0:
			{
				memcpy(uart0_send_space , buf , len);
				xfer.data = uart0_send_space;
				xfer.dataSize = len;	
				UART_EnableInterrupts( UART0 ,kUART_TransmissionCompleteInterruptEnable);
				UART_SendEDMA(UART0, &g_uart0EdmaHandle, &xfer);						
			};break;
		case BSP_UART1: 
			{
				memcpy(uart1_send_space , buf , len);
				xfer.data = uart1_send_space;
				xfer.dataSize = len;	
				UART_EnableInterrupts( UART1 ,kUART_TransmissionCompleteInterruptEnable);
				UART_SendEDMA(UART1, &g_uart1EdmaHandle, &xfer);				
			}break;
		case BSP_UART2:; break;
		case BSP_UART3:
			{
				memcpy(uart3_send_space , buf , len);
				xfer.data = uart3_send_space;
				xfer.dataSize = len;	
				UART_EnableInterrupts( UART3 ,kUART_TransmissionCompleteInterruptEnable);
				UART_SendEDMA(UART3, &g_uart3EdmaHandle, &xfer);					
			}; break;
		case BSP_UART4:
			{
				memcpy(uart4_send_space , buf , len);
				xfer.data = uart4_send_space;
				xfer.dataSize = len;	
				UART_EnableInterrupts( UART4 ,kUART_TransmissionCompleteInterruptEnable);
				UART_SendEDMA(UART4, &g_uart3EdmaHandle, &xfer);					
			}; break;			
		default:break;
	}
}

		// -------------
		// ---Rev-------

		// -------------


// ------------------------------

// -----------IRQ--------------


void UART0_RX_TX_IRQHandler(void)
{
	if(UART_GetStatusFlags(UART0) &kUART_TransmissionCompleteFlag )
	{
		//DEBUG("kUART_TransmissionCompleteFlag\r\n");
		UART_EnableTx(UART0, false);
		UART0->C2 |= 0x01;
		UART0->C2  &= (~0x01);
		UART_EnableTx(UART0, true);
		UART_DisableInterrupts( UART0 ,kUART_TransmissionCompleteInterruptEnable);
	}
	if(UART_GetStatusFlags(UART0) &kUART_RxDataRegFullFlag )
	{
//		DEBUG("kUART_RxDataRegFullFlag\r\n");

		uint8_t c = 0;
		c = UART_ReadByte(UART0);
		DEBUG("Uart0 R:%X\r\n" , c);	 
		
		BSP_UART_WriteBytes_DMA(BSP_UART3 , &c , 1);
	}
	
}

void UART1_ERR_IRQHandler(void)
{
	DEBUG("UART1_ERR_IRQHandler\r\n");
}

void UART1_RX_TX_IRQHandler(void)
{
	if(UART_GetStatusFlags(UART1) &kUART_TransmissionCompleteFlag )
	{
		//DEBUG("kUART_TransmissionCompleteFlag\r\n");
		UART_EnableTx(UART1, false);
		UART1->C2 |= 0x01;
		UART1->C2  &= (~0x01);
		UART_EnableTx(UART1, true);
		UART_DisableInterrupts( UART1 ,kUART_TransmissionCompleteInterruptEnable);
	}
	if(UART_GetStatusFlags(UART1) &kUART_RxDataRegFullFlag )
	{
//		DEBUG("kUART_RxDataRegFullFlag\r\n");
//		DEBUG("Uart R:%X\r\n" , c);	
//		uint8_t c = 0;
//		c = UART_ReadByte(UART1);

	}
}

void UART3_RX_TX_IRQHandler(void)
{
	if(UART_GetStatusFlags(UART3) &kUART_TransmissionCompleteFlag )
	{
		//DEBUG("kUART_TransmissionCompleteFlag\r\n");
		UART_EnableTx(UART3, false);
		UART3->C2 |= 0x01;
		UART3->C2  &= (~0x01);
		UART_EnableTx(UART3, true);
		UART_DisableInterrupts( UART3 ,kUART_TransmissionCompleteInterruptEnable);
	}
	if(UART_GetStatusFlags(UART3) &kUART_RxDataRegFullFlag )
	{
//		DEBUG("kUART_RxDataRegFullFlag\r\n");

		uint8_t c = 0;
		c = UART_ReadByte(UART3);
		DEBUG("Uart3 R:%X\r\n" , c);	
		BSP_UART_WriteBytes_DMA(BSP_UART0 , &c , 1);
	}
}

void UART4_RX_TX_IRQHandler(void)
{
	if(UART_GetStatusFlags(UART4) &kUART_TransmissionCompleteFlag )
	{
		//DEBUG("kUART_TransmissionCompleteFlag\r\n");
		UART_EnableTx(UART4, false);
		UART4->C2 |= 0x01;
		UART4->C2  &= (~0x01);
		UART_EnableTx(UART4, true);
		UART_DisableInterrupts( UART4 ,kUART_TransmissionCompleteInterruptEnable);
	}
	if(UART_GetStatusFlags(UART4) &kUART_RxDataRegFullFlag )
	{
//		DEBUG("kUART_RxDataRegFullFlag\r\n");

		uint8_t c = 0;
		c = UART_ReadByte(UART4);
		DEBUG("Uart4 R:%X\r\n" , c);	
		//BSP_UART_WriteBytes_DMA(BSP_UART0 , &c , 1);
	}
}

void UART2_IRQHandler(void)
{
	DEBUG("UART2_IRQHandler\r\n");
	
	if((UART_GetStatusFlags(UART2) & kUART_RxDataRegFullFlag )== kUART_RxDataRegFullFlag)
	{
		UART_ClearStatusFlags(UART2, kUART_RxDataRegFullFlag);
		
		uint8_t c = 0;
		c = UART_ReadByte(UART2);
		DEBUG("Uart R:%X\r\n" , c);
	}
	
	if((UART_GetStatusFlags(UART2) & kUART_RxActiveEdgeInterruptEnable )== kUART_RxActiveEdgeInterruptEnable)
	{
		UART2->S2 |= UART_S2_RXEDGIF(1);  
	}	
	
}

void DMA0_IRQHandler(void)
{
	DEBUG("DMA0_IRQHandler\r\n");
	EDMA_HandleIRQ(&g_uart0TxEdmaHandle);
}
void DMA3_IRQHandler(void)
{
	DEBUG("DMA3_IRQHandler\r\n");
	EDMA_HandleIRQ(&g_uart3TxEdmaHandle);
}
void DMA4_IRQHandler(void)
{
	DEBUG("DMA4_IRQHandler\r\n");
	EDMA_HandleIRQ(&g_uart4TxEdmaHandle);
}

// ----------------------------

// ------------Test-------------
uint8_t test_bud[] = {0x00,0x12,0x32,0xff,0x00,0x11,0xcd,0x45};
void BSP_Uart_Test_Send(void)
{
	DEBUG("BSP_Uart_Test_Send\r\n");
	BSP_UART_WriteBytes_DMA(BSP_485COM , test_bud, sizeof(test_bud));
}

// -----------------------------

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

