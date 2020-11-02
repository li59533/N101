/**
 **************************************************************************************************
 * @file        net_task.c
 * @author
 * @version   v0.1.0
 * @date        
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */

#include "limits.h"
#include "clog.h"
#include "FreeRTOS.h"

#include "timers.h"
#include "net_task.h"
#include "rtos_tools.h"
/**
 * @addtogroup    XXX 
 * @{  
 */
#include "bsp_bc25.h"

/**
 * @addtogroup    net_task_Modules 
 * @{  
 */

/**
 * @defgroup      net_task_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      net_task_Macros_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */
 
/**
 * @defgroup      net_task_Constants_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      net_task_Private_Types
 * @brief         
 * @{  
 */

/**
 * @}
 */
 
/**
 * @defgroup      net_task_Private_Variables 
 * @brief         
 * @{  
 */
TimerHandle_t net_task_tim;
TaskHandle_t  Net_Task_Handle = NULL;

/**
 * @}
 */
 
/**
 * @defgroup      net_task_Public_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      net_task_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */

static void net_task_tim_callback(TimerHandle_t xTimer);
/**
 * @}
 */

/**
 * @defgroup      net_task_Functions 
 * @brief         
 * @{  
 */
//#define 		NET_STK_SIZE 		256  							//任务堆栈大小	
//StackType_t 	Net_TaskStack[NET_STK_SIZE];			//任务堆栈
//StaticTask_t 	Net_TaskTCB;												//任务控制块

uint32_t Net_Task_Init(void)
{
	BaseType_t basetype = { 0 };
	basetype = xTaskCreate(Net_Task,\
							"Net Task",\
							4096,
							NULL,
							3,
							&Net_Task_Handle);
	
//	Net_Task_Handle=xTaskCreateStatic((TaskFunction_t	)Net_Task,		//任务函数
//										(const char* 	)"Net Task",		//任务名称
//										(uint32_t 		)NET_STK_SIZE,	//任务堆栈大小
//										(void* 		  	)NULL,				//传递给任务函数的参数
//										(UBaseType_t 	) 1, 	//任务优先级
//										(StackType_t*   )Net_TaskStack,	//任务堆栈
//										(StaticTask_t*  )&Net_TaskTCB);	//任务控制块              
//			
	
	return basetype;
}


void Net_Task(void * pvParameter)
{
	uint32_t event_flag = 0;
	
	DEBUG("Net Task Enter\r\n");
	UBaseType_t nettask_ramainheap = 0;

	while(1)
	{
		xTaskNotifyWait(0x00,ULONG_MAX,&event_flag , portMAX_DELAY);
		
		if((event_flag & NET_TASK_TEST_EVENT) != 0x00)
		{
			DEBUG("Net Task Looping\r\n");
			nettask_ramainheap = uxTaskGetStackHighWaterMark(NULL);
			DEBUG("Net Task ramain heap:%d %%\r\n",nettask_ramainheap);
		}
		if((event_flag & NET_TASK_TEST2_EVENT) != 0x00)
		{
			DEBUG("Net Task NET_TASK_TEST2_EVENT\r\n");
		}	
		if((event_flag & NET_TASK_BC25_LOOP_EVENT) != 0x00)
		{
			//DEBUG("NET_TASK_BC25_LOOP_EVENT\r\n");
			
			BSP_BC25_Loop();
			
		}	
		if((event_flag & NET_TASK_BC25_WAKEUP_EVENT) != 0x00)
		{
			BSP_BC25_NB_PSMwakeup();
		}		
		if((event_flag & NET_TASK_BC25_HREST_EVENT) != 0x00)
		{
			BSP_BC25_NB_HRest();	
		}			
		
	}
}


void Net_Task_Event_Start(uint32_t events, uint8_t event_from)
{
	switch(event_from)
	{
		case EVENT_FROM_TASK:
		{
			xTaskNotify(Net_Task_Handle , events , eSetBits);
		}
		break;
		case EVENT_FROM_ISR:
		{ 
			xTaskNotifyFromISR(Net_Task_Handle, events, eSetBits , NULL);
		}
		break;
		default:break;
	}
}




void Net_Task_Tim_Init(void)
{
	net_task_tim = xTimerCreate(	"NetTimOUT",			/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
									pdMS_TO_TICKS(1000),
									pdTRUE,
									NULL,
									net_task_tim_callback );
}

void Net_Task_StartTim(uint16_t time_count)
{
	xTimerChangePeriod( net_task_tim,  pdMS_TO_TICKS(time_count) , 0 );
	xTimerStart( net_task_tim,0);
}
static void net_task_tim_callback(TimerHandle_t xTimer)
{
	//Net_Task_Event_Start(Net_TASK_SEND_AT_EVENT, EVENT_FROM_TASK);
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

