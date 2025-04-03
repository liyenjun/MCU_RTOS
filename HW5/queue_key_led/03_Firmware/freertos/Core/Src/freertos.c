/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "bsp_key.h"
#include "bsp_led.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
//  key_TaskHandle = osThreadNew(key_task_func, NULL, &key_Task_attributes);
  led_TaskHandle = osThreadNew(led_task_func, NULL, &led_Task_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
    key_status_t ret_key_status = KEY_OK;
    key_press_status_t key_value = KEY_NOT_PRESSED;
    led_operation_t    led_ops_event =   LED_INITED_VALUE;
    
  /* Infinite loop */
    for(;;)
    {
        printf("APP task is living\r\n");
        
        // 1. detect key is long pressed or short pressed
        ret_key_status = key_scan_short_long_press(&key_value, 1000);
        
        if(KEY_OK == ret_key_status)
        {
            // if key is shot pressed
            if( KEY_SHORT_PRESSED == key_value )
            {
                printf("short pressed at [%d] tick \r\n", HAL_GetTick());
                
                led_ops_event  =  LED_TOGGLE;
                if (pdTRUE == xQueueSendToFront(led_queue,&led_ops_event,0))
                {
                    printf("LED_TOGGLE send successfully at [%d] tick \r\n", 
                                                                HAL_GetTick());
                }
                printf("after send the queue to led\r\n");                
            }
            
            // if key is long pressed
            if( KEY_LONG_PRESSED == key_value )
            {
                printf("long pressed at [%d] tick \r\n", HAL_GetTick());
                
                led_ops_event  =  LED_BLINK_3_TIMES;
                if (pdTRUE == xQueueSendToFront(led_queue,&led_ops_event,0))
                {
                    printf("LED_BLINK_3_TIMES send successfully at [%d] tick \r\n",
                                                                    HAL_GetTick());
                }
                printf("after send the queue to led\r\n"); 
            }
        }
        HAL_Delay(100);
        osDelay(100);
    }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

