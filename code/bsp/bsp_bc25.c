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
#include "self_def.h"
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


typedef enum
{
	CMD_AT_NULL = 0,
	CMD_AT_REQ = 1,
	CMD_AT_RESP = 2,
	
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
	uint8_t cur_module_status;
}BSP_BC25_Info_t;

typedef struct
{
	uint8_t cmd[10];
	uint8_t in ;
	uint8_t out ; 
	uint8_t size   ;
}BSP_BC25_CMDQueue_t;


BSP_BC25_Info_t BSP_BC25_Info = 
{
	.cur_cmdreq = CMD_AT_NULL , 
	.cur_cmdresp = CMD_AT_NULL ,
	.cur_cmdstatus = 0,
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

