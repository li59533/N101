/**
 **************************************************************************************************
 * @file        bsp_bc25.c
 * @author
 * @version
 * @date        5/28/2016
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */

#include "bsp_bc25.h"
#include "clog.h"
/**
 * @addtogroup    XXX 
 * @{  
 */
#include "rtos_tools.h"
#include "bsp_conf.h"
#include "bsp_uart.h"

/**
 * @addtogroup    bsp_bc25_Modules 
 * @{  
 */

/**
 * @defgroup      bsp_bc25_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_bc25_Macros_Defines 
 * @brief         
 * @{  
 */
typedef enum
{
	CMD_REQ = 0 ,
	CMD_RESP = 1,
	CMD_MODULE_IDLE = 2, 
	CMD_MODULE_REST = 3,
	CMD_AT_NULL = 4,
	CMD_AT_REQ = 5,   /* AT*/
	CMD_AT_RESP = 6,
	CMD_ATE0_REQ = 7,   /* ATE0*/
	CMD_ATE0_RESP = 8 ,	
	CMD_AT_QSCLK0_REQ = 9, /*关闭深度睡眠*/
	CMD_AT_QSCLK0_RESP = 10, 
	CMD_AT_QSCLK1_REQ = 11, 
	CMD_AT_QSCLK1_RESP = 12, 
	CMD_AT_CFUN1_REQ = 13, 
	CMD_AT_CFUN1_RESP = 14,
	CMD_AT_CPIN_REQ = 15, 
	CMD_AT_CPIN_RESP = 16, 
	CMD_AT_QCCID_REQ = 17,
	CMD_AT_QCCID_RESP = 18,
	CMD_AT_CIMI_REQ = 19, 
	CMD_AT_CIMI_RESP = 20, 
	CMD_AT_CGSN1_REQ = 21, 
	CMD_AT_CGSN1_RESP = 22, 
	CMD_AT_CSQ_REQ = 23, 
	CMD_AT_CSQ_RESP = 24,
	CMD_AT_QBAND15_REQ = 25, 
	CMD_AT_QBAND15_RESP = 26,
	CMD_AT_CEREG1_REQ = 27, 
	CMD_AT_CEREG1_RESP = 28, 
	CMD_AT_CGATT1_REQ = 29, 
	CMD_AT_CGATT1_RESP = 30, 
	CMD_AT_CGATT_REQ = 31, 
	CMD_AT_CGATT_RESP = 32,
	CMD_AT_CGACT11_REQ = 33,
	CMD_AT_CGACT11_RESP = 34, 
	CMD_AT_CGPADDR_REQ = 35, 
	CMD_AT_CGPADDR_RESP  = 36, 
	CMD_AT_NCFG0_REQ = 37, 
	CMD_AT_NCFG0_RESP = 38,
	CMD_AT_NCDPOPEN_REQ = 39, 
	CMD_AT_NCDPOPEN_RESP = 40, 
	CMD_AT_NMSTATUS_REQ  = 41, 
	CMD_AT_NMSTATUS_RESP = 42, 
	CMD_AT_NNMI2_REQ = 43, 
	CMD_AT_NNMI2_RESP = 44,
	CMD_AT_NCDPCLOSE_REQ = 45, /*断开模块与平台的连接*/
	CMD_AT_NCDPCLOSE_RESP = 46, 
	
	CMD_AT_PSM_WAKEUP = 47,
	 
}BSP_BC25_ATcmd_e;

typedef enum
{
	MODULE_CONNECTED,
	MODULE_DISCONNECT,
}BSP_BC25_Module_e;

/**
 * @}
 */

/**
 * @defgroup      bsp_bc25_Constants_Defines 
 * @brief         
 * @{  
 */
const char nb_AT[] = "AT\r\n";		// 300ms
const char nb_ATE0[] = "ATE0\r\n";
const char nb_AT_QSCLK0[] = "AT+QSCLK=0\r\n";
const char nb_AT_QSCLK1[] = "AT+QSCLK=1\r\n";
const char nb_AT_CFUN1[] = "AT+CFUN=1\r\n";
const char nb_AT_CPIN[] = "AT+CPIN?\r\n";
const char nb_AT_QCCID[] = "AT+QCCID\r\n";
const char nb_AT_CIMI[] = "AT+CIMI\r\n";
const char nb_AT_CGSN1[] = "AT+CGSN=1\r\n";
const char nb_AT_CSQ[] = "AT+CSQ\r\n";
const char nb_AT_QBAND15[] = "AT+QBAND=1,5\r\n";
const char nb_AT_CEREG1[] = "AT+CEREG=1\r\n";
const char nb_AT_CGATT1[] = "AT+CGATT=1\r\n";
const char nb_AT_CGATT[] = "AT+CGATT?\r\n";
const char nb_AT_CGACT11[] = "AT+CGACT=1,1";
const char nb_AT_CGADDR[] = "AT+CGPADDR?\r\n";
const char nb_AT_NCFG0[] = "AT+NCFG=0,20\r\n";
const char nb_AT_NCDPOPEN[] = "AT+NCDPOPEN=\"221.229.214.202\",5683\r\n";
const char nb_AT_NCDPCLOSE[] = "AT+NCDPCLOSE\r\n";
const char nb_AT_NMSTATUS[] = "AT+NMSTATUS?\r\n";
const char nb_AT_NNMI2[] = "AT+NNMI=2\r\n";
const char nb_AT_NMGR[] = "AT+NMGR\r\n";
const char nb_AT_QRST1[] = "AT+QRST=1\r\n";





/**
 * @}
 */

/**
 * @defgroup      bsp_bc25_Private_Types
 * @brief         
 * @{  
 */
typedef struct
{
	uint8_t space[200];
	uint8_t len;
}bsp_bc25_buf_t;

typedef struct
{
	bsp_bc25_buf_t buf[3];
	uint8_t in ;
	uint8_t out;
	uint8_t count;
	uint8_t size;
}bsp_bc25_queue_t;

typedef struct
{
	uint8_t cmd[10];
	uint8_t in ;
	uint8_t out ; 
	uint8_t count;
	uint8_t size   ;
}BSP_BC25_CMDQueue_t;



/**
 * @}
 */

/**
 * @defgroup      bsp_bc25_Private_Variables 
 * @brief         
 * @{  
 */
static bsp_bc25_queue_t bsp_bc25_R_queue = 
{
	.in = 0,
	.out = 0,
	.count = 0,
	.size = sizeof(bsp_bc25_R_queue.buf) / sizeof(bsp_bc25_R_queue.buf[0]),
};
static bsp_bc25_queue_t bsp_bc25_T_queue = 
{
	.in = 0,
	.out = 0,
	.count = 0,
	.size = sizeof(bsp_bc25_T_queue.buf) / sizeof(bsp_bc25_T_queue.buf[0]),	
};

static BSP_BC25_CMDQueue_t BSP_BC25_CMDQueue = 
{
	.in = 0 ,
	.out = 0,
	.count = 0,
	.size = 10,
};


/**
 * @}
 */

/**
 * @defgroup      bsp_bc25_Public_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_bc25_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */
static void BSP_BC25_GpioInit(void);
static void BSP_BC25_NB_pStart(void);
/**
 * @}
 */

/**
 * @defgroup      bsp_bc25_Functions 
 * @brief         
 * @{  
 */
   
typedef struct
{
	uint8_t cur_cmdreq ;
	uint8_t cur_cmdresp ; 
	uint8_t cur_cmdstatus;
	uint16_t cur_cmdtimout;
	uint8_t cur_cmd_timeout_count;
	uint8_t cur_module_status;
}BSP_BC25_Info_t;

BSP_BC25_Info_t BSP_BC25_Info = 
{
	.cur_cmdreq = CMD_AT_NULL , 
	.cur_cmdresp = CMD_AT_NULL ,
	.cur_cmdstatus = CMD_REQ,
	.cur_cmdtimout = 0,
	.cur_module_status = MODULE_DISCONNECT,
};

void BSP_BC25_Init(void)
{
	DEBUG("bsp_bc25 init\r\n");
	BSP_UART_Init(BSP_UART0);
	BSP_UART_Init(BSP_UART3);
	
	BSP_BC25_GpioInit();
	BSP_BC25_NB_pStart();
	
	BSP_BC25_CMD_InQueue(CMD_AT_REQ);
}

static void BSP_BC25_GpioInit(void)
{
	DEBUG("bc25 gpio init\r\n");
	gpio_pin_config_t config =
	{
		kGPIO_DigitalOutput,
		0,
	};
	CLOCK_EnableClock(kCLOCK_PortC);
	PORT_SetPinMux( PORTC , 12, kPORT_MuxAsGpio  );
	GPIO_PinInit( GPIOC , 12, &config);

	PORT_SetPinMux( PORTC , 11, kPORT_MuxAsGpio  );
	GPIO_PinInit( GPIOC , 11, &config);
	
	CLOCK_EnableClock(kCLOCK_PortD);
	PORT_SetPinMux( PORTD , 3, kPORT_MuxAsGpio  );
	GPIO_PinInit( GPIOD , 3, &config);	
	
	PORT_SetPinMux( PORTD , 2, kPORT_MuxAsGpio  );
	GPIO_PinInit( GPIOD , 2, &config);	
	
	GPIO_PinWrite(GPIOC, 12, 0);
	GPIO_PinWrite(GPIOC, 11, 1);
	GPIO_PinWrite(GPIOD, 3, 0);
	GPIO_PinWrite(GPIOD, 2, 1);
}

static void BSP_BC25_NB_pStart(void)
{
	DEBUG("bc25 power start\r\n");
	GPIO_PinWrite(GPIOC, 12, 0);
	GPIO_PinWrite(GPIOC, 12, 1);
	RTOS_Delay_ms(1200);
	GPIO_PinWrite(GPIOC, 12, 0);
	GPIO_PinWrite(GPIOC, 11, 1);
	RTOS_Delay_ms(1);;
	GPIO_PinWrite(GPIOC, 11, 0);
}


void BSP_BC25_NB_PSMwakeup(void)
{
	DEBUG("bc25 PSM wakeup\r\n");
	GPIO_PinWrite(GPIOC, 11, 1);
	RTOS_Delay_ms(100);
	GPIO_PinWrite(GPIOC, 11, 0);
	RTOS_Delay_ms(100);
	GPIO_PinWrite(GPIOC, 11, 1);	
}

void BSP_BC25_Send(uint8_t *buf , uint16_t len )
{
	DEBUG("send:%s\r\n" , (char *)buf);
	BSP_UART_WriteBytes_DMA(BSP_UART3 , buf,  len);
}

void BSP_BC25_Rev_Process(uint8_t * buf , uint16_t len )
{
	
}

void BSP_BC25_Loop(void)
{
	uint8_t at_cmd = 0;
	uint8_t rev_buf[200]; 
	uint16_t rev_len = 0;
	
	switch(BSP_BC25_Info.cur_cmdstatus)
	{
		case CMD_REQ:at_cmd = BSP_BC25_Info.cur_cmdreq ; break;
		case CMD_RESP:at_cmd = BSP_BC25_Info.cur_cmdresp ; break;
		default:break;
	}
	
	/*
	CMD_AT_NULL ,
	CMD_AT_REQ ,
	CMD_AT_RESP ,	
	*/
	
	switch(at_cmd)
	{
		case CMD_AT_NULL:
		{

			BSP_BC25_Info.cur_cmdreq = BSP_BC25_CMD_OutQueue();
			
		}break;
		case CMD_MODULE_IDLE:
		{
			
			DEBUG("CMD_MODULE_IDLE\r\n");
		}break;
		case CMD_MODULE_REST:
		{
			DEBUG("CMD_MODULE_REST\r\n");
		}break;
		case CMD_AT_REQ :   /* AT*/
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT ,strlen(nb_AT));
			BSP_BC25_Send((uint8_t *)nb_AT ,strlen(nb_AT));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;
		}break;
		case CMD_AT_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_ATE0_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}
		}break;
		case CMD_ATE0_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_ATE0 ,strlen(nb_ATE0));
			BSP_BC25_Send((uint8_t *)nb_ATE0 ,strlen(nb_ATE0));
			BSP_BC25_Info.cur_cmdresp = CMD_ATE0_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;				
		}break;
		case CMD_ATE0_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_QSCLK0_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}			
		}break;
		case CMD_AT_QSCLK0_REQ : /*关闭深度睡眠*/
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_QSCLK0 ,strlen(nb_AT_QSCLK0));
			BSP_BC25_Send((uint8_t *)nb_AT_QSCLK0 ,strlen(nb_AT_QSCLK0));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_QSCLK0_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;			
		}break;
		case CMD_AT_QSCLK0_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_CFUN1_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}			
		}break;		
		case CMD_AT_CFUN1_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CFUN1 ,strlen(nb_AT_CFUN1));
			BSP_BC25_Send((uint8_t *)nb_AT_CFUN1 ,strlen(nb_AT_CFUN1));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CFUN1_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;
		}break;
		case CMD_AT_CFUN1_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_CPIN_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}					
		}break;			
		case CMD_AT_CPIN_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CPIN ,strlen(nb_AT_CPIN));
			BSP_BC25_Send((uint8_t *)nb_AT_CPIN ,strlen(nb_AT_CPIN));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CPIN_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;			
		}break;
		case CMD_AT_CPIN_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "+CPIN: READY") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_QCCID_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}					
		}break;
		case CMD_AT_QCCID_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_QCCID ,strlen(nb_AT_QCCID));
			BSP_BC25_Send((uint8_t *)nb_AT_QCCID ,strlen(nb_AT_QCCID));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_QCCID_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;				
		}break;
		case CMD_AT_QCCID_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "+QCCID:") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_CIMI_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}					
		}break;
		case CMD_AT_CIMI_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CIMI ,strlen(nb_AT_CIMI));
			BSP_BC25_Send((uint8_t *)nb_AT_CIMI ,strlen(nb_AT_CIMI));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CIMI_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;				
		}break;
		case CMD_AT_CIMI_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_CGSN1_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}					
		}break;
		case CMD_AT_CGSN1_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CGSN1 ,strlen(nb_AT_CGSN1));
			BSP_BC25_Send((uint8_t *)nb_AT_CGSN1 ,strlen(nb_AT_CGSN1));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CGSN1_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;				
		}break;
		case CMD_AT_CGSN1_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "+CGSN:") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_CSQ_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}								
		}break;
		case CMD_AT_CSQ_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CSQ ,strlen(nb_AT_CSQ));
			BSP_BC25_Send((uint8_t *)nb_AT_CSQ ,strlen(nb_AT_CSQ));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CSQ_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;				
		}break;
		case CMD_AT_CSQ_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "+CSQ:") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_QBAND15_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}						
		}break;
		case CMD_AT_QBAND15_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_QBAND15 ,strlen(nb_AT_QBAND15));
			BSP_BC25_Send((uint8_t *)nb_AT_QBAND15 ,strlen(nb_AT_QBAND15));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_QBAND15_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;				
		}break;
		case CMD_AT_QBAND15_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_CEREG1_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}					
		}break;
		case CMD_AT_CEREG1_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CEREG1 ,strlen(nb_AT_CEREG1));
			BSP_BC25_Send((uint8_t *)nb_AT_CEREG1 ,strlen(nb_AT_CEREG1));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CEREG1_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;				
		}break;
		case CMD_AT_CEREG1_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_CGATT1_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}				
		}break;
		case CMD_AT_CGATT1_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CGATT1 ,strlen(nb_AT_CGATT1));
			BSP_BC25_Send((uint8_t *)nb_AT_CGATT1 ,strlen(nb_AT_CGATT1));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CGATT1_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;				
		}break;
		case CMD_AT_CGATT1_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_CGATT_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}				
		}break;
		case CMD_AT_CGATT_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CGATT ,strlen(nb_AT_CGATT));
			BSP_BC25_Send((uint8_t *)nb_AT_CGATT ,strlen(nb_AT_CGATT));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CGATT_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;				
		}break;
		case CMD_AT_CGATT_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "+CGATT: 1") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_CGACT11_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}						
		}break;
		case CMD_AT_CGACT11_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CGACT11 ,strlen(nb_AT_CGACT11));
			BSP_BC25_Send((uint8_t *)nb_AT   _CGACT11 ,strlen(nb_AT_CGACT11));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CGACT11_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;				
		}break;
		case CMD_AT_CGACT11_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_CGPADDR_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}					
		}break;
		case CMD_AT_CGPADDR_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CGADDR ,strlen(nb_AT_CGADDR));
			BSP_BC25_Send((uint8_t *)nb_AT_CGADDR ,strlen(nb_AT_CGADDR));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CGPADDR_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;				
		}break;
		case CMD_AT_CGPADDR_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "+CGPADDR:") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NCFG0_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}				
		}break;
		case CMD_AT_NCFG0_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			BSP_BC25_Send((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_NCFG0_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;				
		}break;
		case CMD_AT_NCFG0_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NCDPOPEN_REQ;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
					}
				}
			}

			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				if(BSP_BC25_Info.cur_cmd_timeout_count > 0)
				{
					BSP_BC25_Info.cur_cmd_timeout_count --;	
				}
			}	
			
			if(BSP_BC25_Info.cur_cmd_timeout_count == 0)
			{
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST;
			}				
		}break;
		case CMD_AT_NCDPOPEN_REQ:
		{
			
		}break;
		case CMD_AT_NCDPOPEN_RESP:
		{
			
		}break;
		
		
		default:break;
	}
	
	
	
	
}




// --------- queue ----------
void BSP_BC25_CMD_InQueue(uint8_t at_cmd)
{
	BSP_BC25_CMDQueue.cmd[BSP_BC25_CMDQueue.in] = at_cmd ; 
	BSP_BC25_CMDQueue.in ++;
	BSP_BC25_CMDQueue.count ++ ; 
	BSP_BC25_CMDQueue.in %= BSP_BC25_CMDQueue.size;
}

uint8_t BSP_BC25_CMD_OutQueue(void)
{
	uint8_t cmd = 0;
	if(BSP_BC25_CMDQueue.count > 0 )
	{
		cmd = BSP_BC25_CMDQueue.cmd[BSP_BC25_CMDQueue.out];
		BSP_BC25_CMDQueue.out ++;
		BSP_BC25_CMDQueue.count -- ;
		BSP_BC25_CMDQueue.out %= BSP_BC25_CMDQueue.size;
	}
	else
	{
		cmd = CMD_AT_NULL;
	} 
	return cmd;
}

void BSP_BC25_T_InQueue(uint8_t * buf ,uint16_t len)
{
	memcpy(bsp_bc25_T_queue.buf[bsp_bc25_T_queue.in].space , buf , len);
	bsp_bc25_T_queue.buf[bsp_bc25_T_queue.in].len = len;
	bsp_bc25_T_queue.in ++;
	bsp_bc25_T_queue.count ++;
	bsp_bc25_T_queue.in %= bsp_bc25_T_queue.size;
	
}

uint8_t BSP_BC25_T_OutQueue(uint8_t * buf , uint16_t * len)
{
	if(bsp_bc25_T_queue.count > 0 )
	{
		memcpy( buf , bsp_bc25_T_queue.buf[bsp_bc25_T_queue.out].space , bsp_bc25_T_queue.buf[bsp_bc25_T_queue.out].len);
		*len = bsp_bc25_T_queue.buf[bsp_bc25_T_queue.out].len;
		bsp_bc25_T_queue.out ++;
		bsp_bc25_T_queue.count -- ;
		
		bsp_bc25_T_queue.out %= bsp_bc25_T_queue.size;	
	}
	
	return bsp_bc25_T_queue.count;
}

void BSP_BC25_R_InQueue(uint8_t * buf , uint16_t len)
{
	memcpy(bsp_bc25_R_queue.buf[bsp_bc25_R_queue.in].space , buf , len);
	bsp_bc25_R_queue.buf[bsp_bc25_R_queue.in].len = len;
	bsp_bc25_R_queue.in ++;
	bsp_bc25_R_queue.count ++;
	bsp_bc25_R_queue.in %= bsp_bc25_R_queue.size;	
}

uint8_t BSP_BC25_R_OutQueue(uint8_t * buf , uint16_t * len)
{
	if(bsp_bc25_R_queue.count > 0 )
	{
		memcpy( buf , bsp_bc25_R_queue.buf[bsp_bc25_R_queue.out].space , bsp_bc25_R_queue.buf[bsp_bc25_R_queue.out].len);
		*len = bsp_bc25_R_queue.buf[bsp_bc25_R_queue.out].len;
		bsp_bc25_R_queue.out ++;
		bsp_bc25_R_queue.count -- ;
		
		bsp_bc25_R_queue.out %= bsp_bc25_R_queue.size;	
	}
	return bsp_bc25_R_queue.count;	
}

uint8_t BSP_BC25_R_GetQueueCount(void)
{
	return bsp_bc25_R_queue.count;
}
// ------------------------------
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

