/**
 **************************************************************************************************
 * @file        app_dataprocess.c
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
#include "app_dataprocess.h"

#include "clog.h"
/**
 * @addtogroup    XXX 
 * @{  
 */
#include "cJSON.h"
#include "rtos_tools.h"
#include "app_calc.h"
#include "app_sample.h"
#include "bsp_bc25.h"
/**
 * @addtogroup    app_dataprocess_Modules 
 * @{  
 */

/**
 * @defgroup      app_dataprocess_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_dataprocess_Macros_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_dataprocess_Constants_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_dataprocess_Private_Types
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_dataprocess_Private_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_dataprocess_Public_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_dataprocess_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */
static void app_datap_cjsoninit(void);
/**
 * @}
 */

/**
 * @defgroup      app_dataprocess_Functions 
 * @brief         
 * @{  
 */
void APP_Dataprocess_Init(void)
{
	app_datap_cjsoninit();
	
}

static void app_datap_cjsoninit(void)
{
	DEBUG("APP Cjson Version:%s\r\n" ,cJSON_Version() );
	
	cJSON_Hooks hooks;

	hooks.malloc_fn = pvPortMalloc;
	hooks.free_fn = vPortFree;

	cJSON_InitHooks(&hooks);
}

/*
careful use cjson , free mem especially
*/
void APP_DataP_MakeReportPac(void)
{
	cJSON * root =  cJSON_CreateObject();
//	cJSON * x_axis = cJSON_CreateObject();
//	cJSON * y_axis = cJSON_CreateObject();
	cJSON * z_axis = cJSON_CreateObject();
	
	cJSON_AddItemToObject(root, "sn", cJSON_CreateString("FF010001"));
	cJSON_AddItemToObject(root, "type", cJSON_CreateNumber(1));
	cJSON_AddItemToObject(root, "rssi", cJSON_CreateNumber(0));
	/*
	cJSON_AddItemToObject(root, "x", x_axis);
	cJSON_AddItemToObject(x_axis, "basefreq", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(x_axis, "acc_p", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(x_axis, "acc_rms", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(x_axis, "velocity_rms", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(x_axis, "displace_pp", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(x_axis, "displace_rms", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(x_axis, "Envelope", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(x_axis, "Kurtosis", cJSON_CreateNumber(1.5f));	
	
	cJSON_AddItemToObject(root, "y", y_axis);
	cJSON_AddItemToObject(y_axis, "basefreq", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(y_axis, "acc_p", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(y_axis, "acc_rms", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(y_axis, "velocity_rms", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(y_axis, "displace_pp", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(y_axis, "displace_rms", cJSON_CreateNumber(1.5f));	
	cJSON_AddItemToObject(y_axis, "Envelope", cJSON_CreateNumber(1.5f));
	cJSON_AddItemToObject(y_axis, "Kurtosis", cJSON_CreateNumber(1.5f));	
	*/
	
	cJSON_AddItemToObject(root, "z", z_axis);
	cJSON_AddItemToObject(z_axis, "basefreq", cJSON_CreateNumber(APP_CalcValue[APP_SAMPLE_X_INDEX].BaseFreq));
	cJSON_AddItemToObject(z_axis, "acc_p", cJSON_CreateNumber(APP_CalcValue[APP_SAMPLE_X_INDEX].ACC_P));
	cJSON_AddItemToObject(z_axis, "acc_rms", cJSON_CreateNumber(APP_CalcValue[APP_SAMPLE_X_INDEX].ACC_RMS));
	cJSON_AddItemToObject(z_axis, "velocity_rms", cJSON_CreateNumber(APP_CalcValue[APP_SAMPLE_X_INDEX].Velocity_RMS));
	cJSON_AddItemToObject(z_axis, "displace_pp", cJSON_CreateNumber(APP_CalcValue[APP_SAMPLE_X_INDEX].Displace_PP));
	cJSON_AddItemToObject(z_axis, "displace_rms", cJSON_CreateNumber(APP_CalcValue[APP_SAMPLE_X_INDEX].Displace_RMS));	
	cJSON_AddItemToObject(z_axis, "Envelope", cJSON_CreateNumber(APP_CalcValue[APP_SAMPLE_X_INDEX].Envelope));
	cJSON_AddItemToObject(z_axis, "Kurtosis", cJSON_CreateNumber(APP_CalcValue[APP_SAMPLE_X_INDEX].Kurtosis_Coefficient));		
	
	char * str_buf ;
	str_buf =  cJSON_Print(root);
	DEBUG("%s\n", str_buf);	
	
	DEBUG("----Strlen:%d ---\r\n" , strlen(str_buf));
	
	if(BSP_BC25_Report_Status() == MODULE_SEND_BUSY)
	{
		return;
	}	
	
	BSP_BC25_Report((uint8_t *)str_buf ,strlen(str_buf)); 	
	
	cJSON_Delete(root);
	cJSON_free(str_buf);	
	

	
}


/*
	Clog_Float("X ACC_P:" , APP_CalcValue[APP_SAMPLE_X_INDEX].ACC_P);
	Clog_Float("X ACC_RMS:" , APP_CalcValue[APP_SAMPLE_X_INDEX].ACC_RMS);
	Clog_Float("X BaseFreq:" , APP_CalcValue[APP_SAMPLE_X_INDEX].BaseFreq);
	Clog_Float("X Displace_PP:" , APP_CalcValue[APP_SAMPLE_X_INDEX].Displace_PP);
	Clog_Float("X Displace_RMS:" , APP_CalcValue[APP_SAMPLE_X_INDEX].Displace_RMS);
	Clog_Float("X Envelope:" , APP_CalcValue[APP_SAMPLE_X_INDEX].Envelope);
	Clog_Float("X Kurtosis_Coefficient:" , APP_CalcValue[APP_SAMPLE_X_INDEX].Kurtosis_Coefficient);
	Clog_Float("X Velocity_RMS:" , APP_
*/


// ------- Test Code -----
void APP_DataP_TestCode(void)
{
	/*
    cJSON * root =  cJSON_CreateObject();
    cJSON * item =  cJSON_CreateObject();
    cJSON * next =  cJSON_CreateObject();
    cJSON_AddItemToObject(root, "rc", cJSON_CreateNumber(0));//根节点下添加
    cJSON_AddItemToObject(root, "operation", cJSON_CreateString("CALL"));
    cJSON_AddItemToObject(root, "service", cJSON_CreateString("telephone"));
    cJSON_AddItemToObject(root, "text", cJSON_CreateString("打电话给张三"));
    cJSON_AddItemToObject(root, "semantic", item);//root节点下添加semantic节点
    cJSON_AddItemToObject(item, "slots", next);//semantic节点下添加item节点
    cJSON_AddItemToObject(next, "name", cJSON_CreateString("张三"));//添加name节点
 
	char * str_buf ;
	str_buf =  cJSON_Print(root);
    DEBUG("%s\n", str_buf);	
	
	cJSON_Delete(root);
	cJSON_free(str_buf);
	
	*/
	cJSON * root =  cJSON_CreateObject();
	cJSON * x_axis = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "X", x_axis);
	cJSON_AddItemToObject(x_axis, "Acc", cJSON_CreateNumber(1.5f));
	
	char * str_buf ;
	str_buf =  cJSON_Print(root);
	DEBUG("%s\n", str_buf);	
	
	cJSON_Delete(root);
	cJSON_free(str_buf);
	
	
}

// -----------------------


/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

