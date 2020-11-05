/**
 **************************************************************************************************
 * @file        second_task.c
 * @author
 * @version   v0.1.0
 * @date        
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */
#include "second_task.h"
#include "clog.h"



/**
 * @addtogroup    XXX 
 * @{  
 */
#include "bsp_uart.h"
#include "bsp_adc.h"

#include "first_task.h"
#include "second_task.h"

#include "sample_task.h"
#include "refresh_task.h"
#include "hal_task.h"
#include "system_param.h"


#include "crc.h"
#include "bsp_flash.h"

#include "bsp_bc25.h"

#include "bsp_spi.h"

#include "bsp_adxl345.h"

#include "bsp_tim.h"

#include "app_calc.h"
/**
 * @addtogroup    second_task_Modules 
 * @{  
 */

/**
 * @defgroup      second_task_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      second_task_Macros_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */
 
/**
 * @defgroup      second_task_Constants_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      second_task_Private_Types
 * @brief         
 * @{  
 */

/**
 * @}
 */
 
/**
 * @defgroup      second_task_Private_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */
 
/**
 * @defgroup      second_task_Public_Variables 
 * @brief         
 * @{  
 */
TaskHandle_t  Second_Task_Handle = NULL;
/**
 * @}
 */

/**
 * @defgroup      second_task_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      second_task_Functions 
 * @brief         
 * @{  
 */
//#define 		SECOND_STK_SIZE 		512  							//任务堆栈大小	
//StackType_t 	Second_TaskStack[SECOND_STK_SIZE];			//任务堆栈
//StaticTask_t 	Second_TaskTCB;												//任务控制块



uint32_t Second_Task_Init(void)
{
	BaseType_t basetype = { 0 };
	basetype = xTaskCreate(Second_Task,\
							"Second_Task",\
							1024,
							NULL,
							1,
							&Second_Task_Handle);
	
//	Second_Task_Handle=xTaskCreateStatic((TaskFunction_t	)Second_Task,		//任务函数
//										(const char* 	)"Second Task",		//任务名称
//										(uint32_t 		)SECOND_STK_SIZE,	//任务堆栈大小
//										(void* 		  	)NULL,				//传递给任务函数的参数
//										(UBaseType_t 	) 4, 	//任务优先级
//										(StackType_t*   )Second_TaskStack,	//任务堆栈
//										(StaticTask_t*  )&Second_TaskTCB);	//任务控制块              
//		
	
	
	return basetype;
}
#include "cmsis_armcc.h"
void Second_Task(void * pvParameter)
{
	UBaseType_t secondtask_ramainheap = 0;
	//UBaseType_t ramainheap = 0;
	RTOS_Delay_ms(1000);
	DEBUG("Second Task Enter\r\n");
		
	//BSP_Adxl345_Init();
	
	
	
	while(1)
	{
//		secondtask_ramainheap = uxTaskGetStackHighWaterMark(NULL);
//		DEBUG("Second Task ramain heap:%d %%\r\n",secondtask_ramainheap);		
		
		DEBUG("Second Task Looping\r\n");
//		DEBUG("Free Heap:%d\r\n" , RTOS_Get_FreeHeapSize());
		
//		ramainheap = uxTaskGetStackHighWaterMark(First_Task_Handle);
//		DEBUG("First Task ramain heap:%d\r\n",ramainheap);
//		
//		ramainheap = uxTaskGetStackHighWaterMark(Second_Task_Handle);
//		DEBUG("Second Task ramain heap:%d\r\n",ramainheap);
//		
//		ramainheap = uxTaskGetStackHighWaterMark(Sample_Task_Handle);
//		DEBUG("Sample Task ramain heap:%d\r\n",ramainheap);
//		
//		ramainheap = uxTaskGetStackHighWaterMark(Modbus_Task_Handle);
//		DEBUG("Modbus Task ramain heap:%d\r\n",ramainheap);

//		
//		ramainheap = uxTaskGetStackHighWaterMark(Refresh_Task_Handle);
//		DEBUG("Refresh Task ramain heap:%d\r\n",ramainheap);	
//		
//		ramainheap = uxTaskGetStackHighWaterMark(Hal_Task_Handle);
//		DEBUG("Hal Task ramain heap:%d\r\n",ramainheap);
		
		
		RTOS_Delay_ms(3000);
		//APP_Calc_TestCode();
		//BSP_Adxl345_TestCode();

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

