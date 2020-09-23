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

/**
 * @addtogroup    XXX 
 * @{  
 */

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

/**
 * @}
 */

/**
 * @defgroup      bsp_bc25_Constants_Defines 
 * @brief         
 * @{  
 */
const char nb_AT[] = "AT\r\n";	// 300ms
const char nb_AT_QSCLK_0[] = "AT+QSCLK=0\r\n";
const char nb_AT_CPSMS_0[] = "AT+CPSMS=0\r\n";
const char nb_AT_CEDRXS  
const char nb_AT_CSQ[] = "AT+CSQ\r\n"; 	// 300ms
const char nb_ATI[] = "ATI\r\n";			// 300ms
const char nb_ATE0[] = "ATE0\r\n";		//300ms
const char nb_ATE1[] = "ATE1\r\n";
const char nb_AT_CGSN_1[] = "AT+CGSN=1\r\n"; //300ms IMEI
const char nb_AT_CPIN[] = "AT+CPIN?\r\n"; //5s check sim
const char nb_AT_CEREG_1[] = "AT+CEREG=1\r\n"; // 300ms
const char nb_AT_CGATT_1[] = "AT+CGATT=1\r\n"; //300ms
const char nb_AT_CGACT_1[] = "AT+CGACT=1,1\r\n";//300ms
const char nb_AT_CGPADDR_1[] = "AT+CGPADDR=1\r\n";//300ms

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

/**
 * @}
 */

/**
 * @defgroup      bsp_bc25_Functions 
 * @brief         
 * @{  
 */
   
typedef enum
{
	CMD_REQ = 0 ,
	CMD_RESP ,
	CMD_MODULE_REST ,
	CMD_AT_NULL ,
	CMD_AT_REQ ,   /* AT*/
	CMD_AT_RESP ,
	CMD_AT_QSCLK_REQ , /*关闭深度睡眠*/
	CMD_AT_QSCLK_RESP , 
	CMD_AT_CPSMS_REQ , /*关闭PSM*/
	CMD_AT_CPSMS_RESP , 
	CMD_AT_CEDRXS_REQ , /*关闭eDRX*/
	CMD_AT_CEDRXS_RESP , 
	CMD_AT_CGDCONT_REQ , /*查询模块是否成功注网*/
	CMD_AT_CGDCONT_RESP , 
	CMD_AT_CFUN_REQ , /*UE功能,值为1表示功能完整*/
	CMD_AT_CFUN_RESP , 
	CMD_AT_CIMI_REQ , /*查询IMSI号*/
	CMD_AT_CIMI_RESP , 
	CMD_AT_CESQ_REQ , /*查询信号质量*/
	CMD_AT_CESQ_RESP , 
	CMD_AT_QENG_REQ , /*查询模块当前网络状态*/
	CMD_AT_QENG_RESP , 
	CMD_AT_CGATT_REQ , /*查询是否激活网络,1成功0失败*/ 
	CMD_AT_CGATT_RESP , 
	CMD_AT_CEREQ_REQ , /*查询网络注册,1成功0失败*/
	CMD_AT_CEREQ_RESP , 
	CMD_AT_CSCON_REQ , /*查询信号连接，1连接0idle*/
	CMD_AT_CSCON_RESP , 
	CMD_AT_CGPADDR_REQ , /*查询是否取得IP地址*/
	CMD_AT_CGPADDR_RESP , 
	CMD_AT_NCDPOPEN_REQ ,/*将模块连接到平台的服务器*/
	CMD_AT_NCDPOPEN_RESP , 
	CMD_AT_NNMI_REQ , /*设置数据接收模式*/
	CMD_AT_NNMI_RESP , 
	CMD_AT_NMGS_REQ , /*向平台发送数据*/
	CMD_AT_NMGS_RESP , 
	CMD_AT_NMGR_REQ , /*从Buffer中读取数据*/
	CMD_AT_NMGR_RESP ,
	CMD_AT_NCDPCLOSE_REQ , /*断开模块与平台的连接*/
	CMD_AT_NCDPCLOSE_RESP , 
}BSP_BC25_ATcmd_e;

typedef enum
{
	MODULE_CONNECTED,
	MODULE_DISCONNECT,
}BSP_BC25_Module_e;

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
	
}

void BSP_BC25_Send(uint8_t *buf , uint16_t len )
{
	
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
		case CMD_MODULE_REST:
		{
			
		}break;
		case CMD_AT_REQ :   /* AT*/
		{
			BSP_BC25_T_InQueue((uint8_t *)nb_AT ,strlen(nb_AT));
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
						BSP_BC25_Info.cur_cmdreq = CMD_AT_QSCLK_REQ;
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
		}break;
		case CMD_AT_QSCLK_REQ : /*关闭深度睡眠*/
		{
			BSP_BC25_T_InQueue((uint8_t *)nb_AT ,strlen(nb_AT));
			BSP_BC25_Info.cur_cmdresp = CMD_AT_RESP;
			BSP_BC25_Info.cur_cmdstatus = CMD_RESP;
			BSP_BC25_Info.cur_cmdtimout = 300;
			BSP_BC25_Info.cur_cmd_timeout_count = 3;			
		}break;
		case CMD_AT_QSCLK_RESP:
		{}break;			
		case CMD_AT_CPSMS_REQ: /*关闭PSM*/
		{}break;
		case CMD_AT_CPSMS_RESP:
		{}break;			
		case CMD_AT_CEDRXS_REQ:/*关闭eDRX*/
		{}break;
		case CMD_AT_CEDRXS_RESP:
		{}break;			
		case CMD_AT_CGDCONT_REQ:/*查询模块是否成功注网*/
		{}break;
		case CMD_AT_CGDCONT_RESP:
		{}break;			
		case CMD_AT_CFUN_REQ: /*UE功能,值为1表示功能完整*/
		{}break;
		case CMD_AT_CFUN_RESP:
		{}break;			
		case CMD_AT_CIMI_REQ: /*查询IMSI号*/
		{}break;
		case CMD_AT_CIMI_RESP:
		{}break;			
		case CMD_AT_CESQ_REQ:/*查询信号质量*/
		{}break;
		case CMD_AT_CESQ_RESP:
		{}break;			
		case CMD_AT_QENG_REQ:/*查询模块当前网络状态*/
		{}break;
		case CMD_AT_QENG_RESP:
		{}break;			
		case CMD_AT_CGATT_REQ:/*查询是否激活网络,1成功0失败*/ 
		{}break;
		case CMD_AT_CGATT_RESP:
		{}break;			
		case CMD_AT_CEREQ_REQ:/*查询网络注册,1成功0失败*/
		{}break;
		case CMD_AT_CEREQ_RESP:
		{}break;			
		case CMD_AT_CSCON_REQ:/*查询信号连接，1连接0idle*/
		{}break;
		case CMD_AT_CSCON_RESP:
		{}break;			
		case CMD_AT_CGPADDR_REQ:/*查询是否取得IP地址*/
		{}break;
		case CMD_AT_CGPADDR_RESP:		
		{}break;
		case CMD_AT_NCDPOPEN_REQ:/*将模块连接到平台的服务器*/
		{}break;
		case CMD_AT_NCDPOPEN_RESP:
		{}break;			
		case CMD_AT_NNMI_REQ:/*设置数据接收模式*/
		{}break;
		case CMD_AT_NNMI_RESP:
		{}break;			
		case CMD_AT_NMGS_REQ: /*向平台发送数据*/
		{}break;
		case CMD_AT_NMGS_RESP:	
		{}break;
		case CMD_AT_NMGR_REQ:/*从Buffer中读取数据*/
		{}break;
		case CMD_AT_NMGR_RESP:
		{}break;			
		case CMD_AT_NCDPCLOSE_REQ:	/*断开模块与平台的连接*/
		{}break;			
		case CMD_AT_NCDPCLOSE_RESP:
		{}break;
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

