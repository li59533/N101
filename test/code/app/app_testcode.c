/**
 **************************************************************************************************
 * @file        app_testcode.c
 * @author
 * @version
 * @date        5/28/2016
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */
 
#include "app_testcode.h"
#include "bsp_conf.h"
/**
 * @addtogroup    XXX 
 * @{  
 */
 
 
#include "bsp_uart.h"
/**
 * @addtogroup    app_testcode_Modules 
 * @{  
 */

/**
 * @defgroup      app_testcode_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_testcode_Macros_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_testcode_Constants_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_testcode_Private_Types
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_testcode_Private_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_testcode_Public_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_testcode_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      app_testcode_Functions 
 * @brief         
 * @{  
 */
void APP_Test_u0u3(uint8_t *buf , uint16_t len)
{
	
}

void APP_Test_GpioInit(void)
{
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

void APP_Test_NB_pStart(void)
{
	GPIO_PinWrite(GPIOC, 12, 0);
	GPIO_PinWrite(GPIOC, 12, 1);
	vTaskDelay(pdMS_TO_TICKS(1200));
	GPIO_PinWrite(GPIOC, 12, 0);
	GPIO_PinWrite(GPIOC, 11, 1);
	vTaskDelay(pdMS_TO_TICKS(1));
	GPIO_PinWrite(GPIOC, 11, 0);
}


void APP_Test_NB_PSMwakeup(void)
{
	GPIO_PinWrite(GPIOC, 11, 1);
	vTaskDelay(pdMS_TO_TICKS(100));
	GPIO_PinWrite(GPIOC, 11, 0);
	vTaskDelay(pdMS_TO_TICKS(100));
	GPIO_PinWrite(GPIOC, 11, 1);	
}

void APP_Test_U3send(uint8_t * buf , uint16_t len)
{
	char testbuf[] = "hello world\r\n";
	
	//BSP_UART_WriteBytes_DMA( BSP_UART3 , (uint8_t *)testbuf , strlen(testbuf));
	
	BSP_UART_WriteBytes_Blocking( BSP_UART4 ,(uint8_t *)testbuf , strlen(testbuf));

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

