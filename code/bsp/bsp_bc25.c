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
 
 #include "net_task.h"
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
	// ----- NET --
	CMD_REQ = 0 ,
	CMD_RESP = 1,
	CMD_MODULE_IDLE = 2, 
	CMD_MODULE_REST_REQ = 3,
	CMD_MODULE_REST_RESP ,
	CMD_MODULE_HAL_REST_REQ ,
	CMD_MODULE_HAL_REST_RESP ,
	CMD_AT_NULL ,
	CMD_AT_REQ ,   /* AT*/
	CMD_AT_RESP ,
	CMD_ATE0_REQ ,   /* ATE0*/
	CMD_ATE0_RESP ,	
	CMD_AT_QSCLK0_REQ , /*关闭深度睡眠*/
	CMD_AT_QSCLK0_RESP , 
	CMD_AT_QSCLK1_REQ , 
	CMD_AT_QSCLK1_RESP , 
	CMD_AT_CFUN1_REQ , 
	CMD_AT_CFUN1_RESP ,
	CMD_AT_CPIN_REQ , 
	CMD_AT_CPIN_RESP , 
	CMD_AT_QCCID_REQ ,
	CMD_AT_QCCID_RESP ,
	CMD_AT_CIMI_REQ , 
	CMD_AT_CIMI_RESP , 
	CMD_AT_CGSN1_REQ , 
	CMD_AT_CGSN1_RESP , 
	CMD_AT_CSQ_REQ , 
	CMD_AT_CSQ_RESP ,
	CMD_AT_QBAND15_REQ , 
	CMD_AT_QBAND15_RESP ,
	CMD_AT_CEREG1_REQ , 
	CMD_AT_CEREG1_RESP , 
	CMD_AT_CGATT1_REQ , 
	CMD_AT_CGATT1_RESP , 
	CMD_AT_CGATT_REQ , 
	CMD_AT_CGATT_RESP ,
	CMD_AT_CGACT11_REQ ,
	CMD_AT_CGACT11_RESP , 
	CMD_AT_CGPADDR_REQ , 
	CMD_AT_CGPADDR_RESP , 
	CMD_AT_NCFG0_REQ , 
	CMD_AT_NCFG0_RESP ,
	CMD_AT_NCDPOPEN_REQ , 
	CMD_AT_NCDPOPEN_RESP , 
	CMD_AT_NMSTATUS_REQ  , 
	CMD_AT_NMSTATUS_RESP , 
	CMD_AT_NNMI2_REQ , 
	CMD_AT_NNMI2_RESP ,
	CMD_AT_NCDPCLOSE_REQ , /*断开模块与平台的连接*/
	CMD_AT_NCDPCLOSE_RESP , 
	// -------------
	CMD_AT_PSM_WAKEUP ,
	
	// -----SEND ---


	CMD_MODULE_WAKEUP_REQ , 
	CMD_MODULE_WAKEUP_RESP , 
	CMD_SEND_AT_REQ , 
	CMD_SEND_AT_RESP , 
	CMD_SEND_QSCLK0_REQ ,
	CMD_SEND_QSCLK0_RESP , 
	CMD_SEND_NMGS_REQ , 
	CMD_SEND_NMGS_RESP ,	
	CMD_SEND_QSCLK1_REQ ,
	CMD_SEND_QSCLK1_RESP , 	
	
	
	// -------------
	

}BSP_BC25_ATcmd_e;

typedef enum
{
	MODULE_SLEEP ,
	MODULE_WAKEUP ,
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
const char nb_AT_CGACT11[] = "AT+CGACT=1,1\r\n";
const char nb_AT_CGADDR[] = "AT+CGPADDR?\r\n";
const char nb_AT_NCFG0[] = "AT+NCFG=0,20\r\n";
const char nb_AT_NCDPOPEN[] = "AT+NCDPOPEN=\"221.229.214.202\",5683\r\n";
const char nb_AT_NCDPCLOSE[] = "AT+NCDPCLOSE\r\n";
const char nb_AT_NMSTATUS[] = "AT+NMSTATUS?\r\n";
const char nb_AT_NNMI2[] = "AT+NNMI=2\r\n";
const char nb_AT_NMGR[] = "AT+NMGR\r\n";
const char nb_AT_QRST1[] = "AT+QRST=1\r\n";
const char nb_AT_NMGS[] = "AT+NMGS=";   //AT+NMGS=4,00000099




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
	
	BSP_BC25_CMD_InQueue(CMD_MODULE_REST_REQ);
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

void BSP_BC25_NB_HRest(void)
{
	GPIO_PinWrite(GPIOD, 3, 0);
	GPIO_PinWrite(GPIOD, 3, 1);
	RTOS_Delay_ms(1000);
	GPIO_PinWrite(GPIOD, 3, 0);
}

void BSP_BC25_NB_pStart(void)
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
	BSP_UART_WriteBytes_DMA(BSP_UART0 , buf,  len);
}

void BSP_BC25_Rev_Process(uint8_t * buf , uint16_t len )
{
	
}

static uint8_t bsp_bc25_reprotbuf[1024];

uint8_t * bsp_bc25_testcode(void)
{
	for(uint16_t i = 0 ; i < 1024 ; i ++)
	{
		bsp_bc25_reprotbuf[i] = 'F';
	}
	bsp_bc25_reprotbuf[1000] = 0;
	
	return bsp_bc25_reprotbuf;
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
			BSP_BC25_Info.cur_cmd_timeout_count = 3;
		}break;
		case CMD_MODULE_IDLE:
		{
			DEBUG("CMD_MODULE_IDLE\r\n");
		}break;
		case CMD_MODULE_HAL_REST_REQ:
		{
			Net_Task_Event_Start( NET_TASK_BC25_HREST_EVENT ,EVENT_FROM_TASK);
			BSP_BC25_Info.cur_cmdresp = CMD_MODULE_HAL_REST_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 1000;
			DEBUG("CMD_MODULE_REST_REQ\r\n");
		}break;
		case CMD_MODULE_HAL_REST_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;
			
			if(BSP_BC25_Info.cur_cmdtimout == 0)
			{
				BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
				BSP_BC25_CMD_InQueue(CMD_MODULE_REST_REQ);
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
			}			
		}break;
		case CMD_MODULE_REST_REQ:
		{ 
			//BSP_BC25_Send((uint8_t *)nb_AT_QRST1 ,strlen(nb_AT_QRST1));
			BSP_BC25_Send((uint8_t *)nb_AT_QRST1 ,strlen(nb_AT_QRST1));
			BSP_BC25_Info.cur_cmdresp = CMD_MODULE_REST_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 10000;
			DEBUG("CMD_MODULE_REST_REQ\r\n");
		}break;
		case CMD_MODULE_REST_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "CPIN: READY") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_HAL_REST_REQ;
			}			
		}break;
		case CMD_AT_REQ :   /* AT*/
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT ,strlen(nb_AT));
			BSP_BC25_Send((uint8_t *)nb_AT ,strlen(nb_AT));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_ATE0_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}
		}break;
		case CMD_ATE0_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_ATE0 ,strlen(nb_ATE0));
			BSP_BC25_Send((uint8_t *)nb_ATE0 ,strlen(nb_ATE0));
			BSP_BC25_Info.cur_cmdresp = CMD_ATE0_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;			
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_QSCLK0_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}			
		}break;
		case CMD_AT_QSCLK0_REQ : /*关闭深度睡眠*/
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_QSCLK0 ,strlen(nb_AT_QSCLK0));
			BSP_BC25_Send((uint8_t *)nb_AT_QSCLK0 ,strlen(nb_AT_QSCLK0));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_QSCLK0_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;		
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_CFUN1_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}			
		}break;		
		case CMD_AT_CFUN1_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CFUN1 ,strlen(nb_AT_CFUN1));
			BSP_BC25_Send((uint8_t *)nb_AT_CFUN1 ,strlen(nb_AT_CFUN1));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CFUN1_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_CPIN_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}					
		}break;			
		case CMD_AT_CPIN_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CPIN ,strlen(nb_AT_CPIN));
			BSP_BC25_Send((uint8_t *)nb_AT_CPIN ,strlen(nb_AT_CPIN));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CPIN_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 3000;		
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_QCCID_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}					
		}break;
		case CMD_AT_QCCID_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_QCCID ,strlen(nb_AT_QCCID));
			BSP_BC25_Send((uint8_t *)nb_AT_QCCID ,strlen(nb_AT_QCCID));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_QCCID_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;		
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_CIMI_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}					
		}break;
		case CMD_AT_CIMI_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CIMI ,strlen(nb_AT_CIMI));
			BSP_BC25_Send((uint8_t *)nb_AT_CIMI ,strlen(nb_AT_CIMI));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CIMI_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;		
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_CGSN1_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}					
		}break;
		case CMD_AT_CGSN1_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CGSN1 ,strlen(nb_AT_CGSN1));
			BSP_BC25_Send((uint8_t *)nb_AT_CGSN1 ,strlen(nb_AT_CGSN1));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CGSN1_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;		
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_CSQ_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}								
		}break;
		case CMD_AT_CSQ_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CSQ ,strlen(nb_AT_CSQ));
			BSP_BC25_Send((uint8_t *)nb_AT_CSQ ,strlen(nb_AT_CSQ));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CSQ_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 1000;		
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
						if(strstr( (const char *)rev_buf , "+CSQ: 99,99") != NULL)
						{
							BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
							BSP_BC25_CMD_InQueue(CMD_AT_CSQ_REQ);
							BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
							BSP_BC25_Info.cur_cmd_timeout_count = 3;							
						}
						else
						{
							BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
							BSP_BC25_CMD_InQueue(CMD_AT_QBAND15_REQ);
							BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
							BSP_BC25_Info.cur_cmd_timeout_count = 3;							
						}
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}						
		}break;
		case CMD_AT_QBAND15_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_QBAND15 ,strlen(nb_AT_QBAND15));
			BSP_BC25_Send((uint8_t *)nb_AT_QBAND15 ,strlen(nb_AT_QBAND15));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_QBAND15_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;			
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_CEREG1_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}					
		}break;
		case CMD_AT_CEREG1_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CEREG1 ,strlen(nb_AT_CEREG1));
			BSP_BC25_Send((uint8_t *)nb_AT_CEREG1 ,strlen(nb_AT_CEREG1));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CEREG1_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;			
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_CGATT1_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}				
		}break;
		case CMD_AT_CGATT1_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CGATT1 ,strlen(nb_AT_CGATT1));
			BSP_BC25_Send((uint8_t *)nb_AT_CGATT1 ,strlen(nb_AT_CGATT1));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CGATT1_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 65000;			
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_CGATT_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}				
		}break;
		case CMD_AT_CGATT_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CGATT ,strlen(nb_AT_CGATT));
			BSP_BC25_Send((uint8_t *)nb_AT_CGATT ,strlen(nb_AT_CGATT));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CGATT_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;			
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_CGACT11_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}						
		}break;
		case CMD_AT_CGACT11_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CGACT11 ,strlen(nb_AT_CGACT11));
			BSP_BC25_Send((uint8_t *)nb_AT_CGACT11 ,strlen(nb_AT_CGACT11));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CGACT11_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 65000;
							
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_CGPADDR_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}					
		}break;
		case CMD_AT_CGPADDR_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_CGADDR ,strlen(nb_AT_CGADDR));
			BSP_BC25_Send((uint8_t *)nb_AT_CGADDR ,strlen(nb_AT_CGADDR));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_CGPADDR_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;			
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_NCFG0_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}				
		}break;
		case CMD_AT_NCFG0_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			BSP_BC25_Send((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_NCFG0_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;			
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_NCDPOPEN_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}				
		}break;
		case CMD_AT_NCDPOPEN_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			BSP_BC25_Send((uint8_t *)nb_AT_NCDPOPEN ,strlen(nb_AT_NCDPOPEN));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_NCDPOPEN_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 10000;				
		}break;
		case CMD_AT_NCDPOPEN_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "+QLWEVTIND: 3") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_NMSTATUS_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
						BSP_BC25_Info.cur_module_status = MODULE_CONNECTED;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}					
		}break;
		case CMD_AT_NMSTATUS_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			BSP_BC25_Send((uint8_t *)nb_AT_NMSTATUS ,strlen(nb_AT_NMSTATUS));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_NMSTATUS_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 10000;				
		}break;
		case CMD_AT_NMSTATUS_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "REGISTERED_AND_OBSERVED") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_AT_QSCLK1_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
						BSP_BC25_Info.cur_module_status = MODULE_CONNECTED;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}				
		}break;
		case CMD_AT_NCDPCLOSE_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			BSP_BC25_Send((uint8_t *)nb_AT_NCDPCLOSE ,strlen(nb_AT_NCDPCLOSE));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_NCDPCLOSE_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 10000;				
		}break;
		case CMD_AT_NCDPCLOSE_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_ATE0_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;
						BSP_BC25_Info.cur_module_status = MODULE_DISCONNECT;
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}				
		}break;
		case CMD_AT_QSCLK1_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			BSP_BC25_Send((uint8_t *)nb_AT_QSCLK1 ,strlen(nb_AT_QSCLK1));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_QSCLK1_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 30000;	
			BSP_BC25_Info.cur_module_status = MODULE_SLEEP;				
		}break;
		case CMD_AT_QSCLK1_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
//					if(strstr( (const char *)rev_buf , "OK") != NULL)
//					{
//						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
//						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
//						BSP_BC25_Info.cur_cmd_timeout_count = 3;
//					}
					if(strstr( (const char *)rev_buf , "+QATSLEEP") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;	
						BSP_BC25_Info.cur_module_status = MODULE_SLEEP;		
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}					
		}break;
		case CMD_MODULE_WAKEUP_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			//BSP_BC25_Send((uint8_t *)nb_AT_QSCLK1 ,strlen(nb_AT_QSCLK1));
			//BSP_BC25_NB_PSMwakeup();
			Net_Task_Event_Start(NET_TASK_BC25_WAKEUP_EVENT, EVENT_FROM_TASK);
			BSP_BC25_Info.cur_cmdresp = CMD_MODULE_WAKEUP_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 5000;					
		}break;
		case CMD_MODULE_WAKEUP_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				static uint8_t wakeup_flag = 0;
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "+QATWAKEUP") != NULL)
					{
						wakeup_flag ++;
					}
					if(strstr( (const char *)rev_buf , "+QLWEVTIND: 6") != NULL)
					{
						wakeup_flag ++;
					}	

					if(wakeup_flag == 2)
					{
						wakeup_flag = 0;
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_SEND_AT_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;	
						BSP_BC25_Info.cur_module_status = MODULE_CONNECTED;						
					}
					else
					{
						
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}					
		}break;
		case CMD_SEND_AT_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			BSP_BC25_Send((uint8_t *)nb_AT ,strlen(nb_AT));
			BSP_BC25_Info.cur_cmdresp = CMD_SEND_AT_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;						
		}break;
		case CMD_SEND_AT_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_SEND_QSCLK0_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;	
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}				
			
		}break;
		case CMD_SEND_QSCLK0_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			BSP_BC25_Send((uint8_t *)nb_AT_QSCLK0 ,strlen(nb_AT_QSCLK0));
			BSP_BC25_Info.cur_cmdresp = CMD_SEND_QSCLK0_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;				
		}break;
		case CMD_SEND_QSCLK0_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_SEND_NMGS_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;	
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}				
		}break;
		case CMD_SEND_NMGS_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			char *send_buf;
			send_buf = pvPortMalloc(sizeof(char) * 1200);
			snprintf(send_buf , 1200 , "%s500,%s\r\n" , nb_AT_NMGS,bsp_bc25_testcode());
			BSP_BC25_Send((uint8_t *)send_buf ,strlen(send_buf));
			vPortFree(send_buf);
			
			BSP_BC25_Info.cur_cmdresp = CMD_SEND_NMGS_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 1000;					
		}break;
		case CMD_SEND_NMGS_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "OK") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_CMD_InQueue(CMD_SEND_QSCLK1_REQ);
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;	
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}				
		}break;
		case CMD_SEND_QSCLK1_REQ:
		{
			//BSP_BC25_T_InQueue((uint8_t *)nb_AT_NCFG0 ,strlen(nb_AT_NCFG0));
			BSP_BC25_Send((uint8_t *)nb_AT_QSCLK1 ,strlen(nb_AT_QSCLK1));
			BSP_BC25_Info.cur_cmdresp = CMD_SEND_QSCLK1_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 20000;				
			BSP_BC25_Info.cur_module_status = MODULE_SLEEP;	
		}break;
		case CMD_SEND_QSCLK1_RESP:
		{
			BSP_BC25_Info.cur_cmdtimout -= 20;

			if(BSP_BC25_R_GetQueueCount() > 0)
			{
				BSP_BC25_R_OutQueue( rev_buf , &rev_len);
				if(rev_len > 0)
				{
					if(strstr( (const char *)rev_buf , "+QATSLEEP") != NULL)
					{
						BSP_BC25_Info.cur_cmdreq = CMD_AT_NULL;
						BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
						BSP_BC25_Info.cur_cmd_timeout_count = 3;	
						BSP_BC25_Info.cur_module_status = MODULE_SLEEP;		
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
				BSP_BC25_Info.cur_cmd_timeout_count = 3;
				BSP_BC25_Info.cur_cmdstatus = CMD_REQ;
				BSP_BC25_Info.cur_cmdreq = CMD_MODULE_REST_REQ;
			}				
		}break;
		default:break;
	}
}





void BSP_BC25_Report(uint8_t *buf ,uint16_t len)
{
	if(BSP_BC25_Info.cur_module_status == MODULE_SLEEP)
	{
		BSP_BC25_CMD_InQueue(CMD_MODULE_WAKEUP_REQ);
	}
	
	if(BSP_BC25_Info.cur_module_status == MODULE_CONNECTED)
	{
		BSP_BC25_CMD_InQueue(CMD_SEND_NMGS_REQ);
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

