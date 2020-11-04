/**
 **************************************************************************************************
 * @file        bsp_led.c
 * @author
 * @version   v0.1.0
 * @date        
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */

#include "bsp_conf.h"
#include "bsp_led.h"
/**
 * @addtogroup    XXX 
 * @{  
 */

#include "clog.h"
/**
 * @addtogroup    bsp_led_Modules 
 * @{  
 */

/**
 * @defgroup      bsp_led_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_led_Macros_Defines 
 * @brief         
 * @{  
 */
#define LED_ON      1
#define LED_OFF     0

typedef enum
{
	LED_Status_ON = 1,
	LED_Status_OFF,
}BSP_LED_Status_e;

typedef enum
{
	Blink_limit = 1,
	Blink_nolimit ,
	NO_Blink ,
}BSP_LED_Blinks_Mode_e;

/**
 * @}
 */
 
/**
 * @defgroup      bsp_led_Constants_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_led_Private_Types
 * @brief         
 * @{  
 */
typedef struct
{
	clock_ip_name_t clk_name;
	PORT_Type *PORT_base;
	uint32_t pin;
	port_mux_t mux;
	GPIO_Type *GPIO_base ;
	uint8_t Begin_status;
}BSP_GPIO_Instance_t;


typedef struct
{
	BSP_LED_Blinks_Mode_e blink_mode;
	BSP_LED_Status_e cur_status;
	BSP_LED_Status_e next_status;
	
	BSP_LED_Status_e status;
	uint8_t blink_times;
	
	
	uint16_t time_lost ; 
	uint16_t next_time_count ; 
	
	uint8_t blink_count ;
	uint8_t cur_on_count;
	uint8_t cur_off_count;
	
	
	
	uint16_t check_on_count;
	uint16_t check_off_count;


}BSP_LED_Blink_Handle_t;
/**
 * @}
 */
 
/**
 * @defgroup      bsp_led_Private_Variables 
 * @brief         
 * @{  
 */
static BSP_LED_Blink_Handle_t bsp_led_blink_handle[BSP_COUNT] = 
{
	{
		.blink_mode = NO_Blink ,
		.blink_times = 0,
		.time_lost = 0,
	}
};




static BSP_GPIO_Instance_t bsp_led_instance[BSP_COUNT] =
{
    {
        .clk_name = kCLOCK_PortD , 
		.PORT_base = PORTD , 
		.pin = 2 , 
		.mux = kPORT_MuxAsGpio , 
		.GPIO_base = GPIOD,
		.Begin_status = LED_ON,
    },

};
/**
 * @}
 */
 
/**
 * @defgroup      bsp_led_Public_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_led_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */
static BSP_LED_Blinks_Mode_e bsp_led_checkMode(uint8_t bsp_ledx);
static void bsp_led_blinklimit_process(uint8_t bsp_ledx);
static void bsp_led_blinknolimit_process(uint8_t bsp_ledx);
static void bsp_led_noblink_process(uint8_t bsp_ledx);

/**s
 * @}
 */


/**
 * @defgroup      bsp_led_Functions 
 * @brief         
 * @{  
 */

void Bsp_LedInit(void)
{
    uint8_t i = 0 ;
	gpio_pin_config_t config =
	{
		kGPIO_DigitalOutput,
		0,
	};
	
    for( i = 0 ; i < BSP_COUNT ; i ++ )
    {
		
		CLOCK_EnableClock(bsp_led_instance[i].clk_name);
		PORT_SetPinMux( bsp_led_instance[i].PORT_base , bsp_led_instance[i].pin, bsp_led_instance[i].mux  );
		GPIO_PinInit( bsp_led_instance[i].GPIO_base , bsp_led_instance[i].pin, &config);		
		
    }
    for( i = 0 ; i < BSP_COUNT ; i ++ )
    {
		GPIO_PinWrite(bsp_led_instance[i].GPIO_base , bsp_led_instance[i].pin , bsp_led_instance[i].Begin_status);
    }
}

void Bsp_LedOpen(uint8_t bsp_ledx)
{
	GPIO_PinWrite(bsp_led_instance[bsp_ledx].GPIO_base , bsp_led_instance[bsp_ledx].pin , LED_ON);
	bsp_led_blink_handle[bsp_ledx].cur_status = LED_Status_ON;	
}

void Bsp_LedClose(uint8_t bsp_ledx)
{
	GPIO_PinWrite(bsp_led_instance[bsp_ledx].GPIO_base , bsp_led_instance[bsp_ledx].pin , LED_OFF);
	bsp_led_blink_handle[bsp_ledx].cur_status = LED_Status_OFF;
}


void Bsp_LedToggle(uint8_t bsp_ledx)
{
	GPIO_WritePinToggle(bsp_led_instance[bsp_ledx].GPIO_base , bsp_led_instance[bsp_ledx].pin );
}

void BSP_Led_UpdateStatus(void) // let this func in 5ms
{
	uint8_t led_mode = 0;
	
	for(uint8_t i = 0 ; i < BSP_COUNT ; i ++)
	{
		led_mode = (uint8_t )bsp_led_checkMode(i);
		switch( led_mode )
		{
			case Blink_limit: bsp_led_blinklimit_process(i) ; break;
			case Blink_nolimit : bsp_led_blinknolimit_process(i) ; break;
			case NO_Blink : bsp_led_noblink_process(i) ; break;
			default :break;
		}
	}
}

void BSP_Led_Blink(uint8_t bsp_ledx , uint8_t blink_count , uint8_t duty_cycle , uint16_t period) // period is ms
{
	bsp_led_blink_handle[bsp_ledx].blink_times = blink_count * 2;
	
	bsp_led_blink_handle[bsp_ledx].check_on_count = (uint16_t )(period * duty_cycle / 100);
	bsp_led_blink_handle[bsp_ledx].check_off_count = period - bsp_led_blink_handle[bsp_ledx].check_on_count;
	
	if(blink_count == 0)
	{
		bsp_led_blink_handle[bsp_ledx].blink_mode = Blink_nolimit;
	}
	else
	{
		bsp_led_blink_handle[bsp_ledx].blink_mode = Blink_limit;
	}
	
}



static BSP_LED_Blinks_Mode_e bsp_led_checkMode(uint8_t bsp_ledx)
{
	return bsp_led_blink_handle[bsp_ledx].blink_mode;
}



static void bsp_led_blinklimit_process(uint8_t bsp_ledx)
{
	
	if(bsp_led_blink_handle[bsp_ledx].blink_times > 0) 
	{
		if( bsp_led_blink_handle[bsp_ledx].time_lost > 0 )
		{
			if(bsp_led_blink_handle[bsp_ledx].time_lost >= 10)
			{
				bsp_led_blink_handle[bsp_ledx].time_lost -= 10;
			}				
			else
			{
				bsp_led_blink_handle[bsp_ledx].time_lost = 0;
			}			
		}
		else
		{
			if(bsp_led_blink_handle[bsp_ledx].cur_status == LED_Status_ON)
			{
				//DEBUG("LED_Status_OFF\r\n");
				bsp_led_blink_handle[bsp_ledx].time_lost = bsp_led_blink_handle[bsp_ledx].check_off_count;
				Bsp_LedClose(bsp_ledx);
				bsp_led_blink_handle[bsp_ledx].blink_times --;
			}
			else
			{
				//DEBUG("LED_Status_ON\r\n");
				Bsp_LedOpen(bsp_ledx);
				bsp_led_blink_handle[bsp_ledx].time_lost = bsp_led_blink_handle[bsp_ledx].check_on_count;
				bsp_led_blink_handle[bsp_ledx].blink_times --;
			}
			
			
			
		}
	}
	else
	{
		Bsp_LedClose(bsp_ledx);
		bsp_led_blink_handle[bsp_ledx].blink_mode = NO_Blink;
	}
}

static void bsp_led_blinknolimit_process(uint8_t bsp_ledx)
{
	if( bsp_led_blink_handle[bsp_ledx].time_lost > 0 )
	{
		if(bsp_led_blink_handle[bsp_ledx].time_lost >= 10)
		{
			bsp_led_blink_handle[bsp_ledx].time_lost -= 10;
		}				
		else
		{
			bsp_led_blink_handle[bsp_ledx].time_lost = 0;
		}			
	}
	else
	{
		if(bsp_led_blink_handle[bsp_ledx].cur_status == LED_Status_ON)
		{
			//DEBUG("LED_Status_OFF\r\n");
			bsp_led_blink_handle[bsp_ledx].time_lost = bsp_led_blink_handle[bsp_ledx].check_off_count;
			Bsp_LedClose(bsp_ledx);
			//bsp_led_blink_handle[bsp_ledx].blink_times --;
		}
		else
		{
			//DEBUG("LED_Status_ON\r\n");
			Bsp_LedOpen(bsp_ledx);
			bsp_led_blink_handle[bsp_ledx].time_lost = bsp_led_blink_handle[bsp_ledx].check_on_count;
			//bsp_led_blink_handle[bsp_ledx].blink_times --;
		}
		
		
		
	}	
}

static void bsp_led_noblink_process(uint8_t bsp_ledx)
{
	
}


void BSP_LED_BlinkStandard(uint8_t bsp_ledx ,uint8_t count , BSP_LED_FuncBlink_e funcblink)
{
	switch((uint8_t )funcblink)
	{
		case Blink_HighSpeed : BSP_Led_Blink(bsp_ledx , count , 30 ,100) ; break;
		case Blink_MidSpeed: BSP_Led_Blink(bsp_ledx , count , 30 ,500) ;break;
		case Blink_LowSpeed: BSP_Led_Blink(bsp_ledx , count , 50 ,1000) ;break;
		default:break;
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

