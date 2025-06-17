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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
#include "elog.h"
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
osThreadId_t Task_ShowHandle, Task_AHandle, Task_BHandle;
const osThreadAttr_t Task_Show_attributes = {
    .name = "Task_Show",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 256 * 4
};

const osThreadAttr_t Task_A_attributes = {
    .name = "Task_A",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 256 * 4
};

const osThreadAttr_t Task_B_attributes = {
    .name = "Task_B",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 256 * 4
};

uint8_t cnt = 0;
uint32_t ADCtemp;
//uint32_t ulConversionResult;
extern uint32_t *buffer;
extern uint32_t *buffer1;
extern uint32_t *buffer2;
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void AppTask_Show(void *argument);
void AppTask_A(void *argument);
void AppTask_B(void *argument);
void MX_FREERTOS_Init(void);

void MX_FREERTOS_Init(void)
{
//    Task_ShowHandle = osThreadNew(AppTask_Show, NULL, &Task_Show_attributes);
    Task_AHandle = osThreadNew(AppTask_A, NULL, &Task_A_attributes);
    Task_BHandle = osThreadNew(AppTask_B, NULL, &Task_B_attributes);
}
/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void AppTask_Show(void *argument)
{
    uint32_t notifyValue = 0;
    
    for(;;)
    {
        uint32_t ulBitsToClearOnEntry = 0x00;
        uint32_t ulBitsToClearOnExit = 0xFFFFFFFF;
        BaseType_t result = xTaskNotifyWait(ulBitsToClearOnEntry, ulBitsToClearOnExit,
                                            &notifyValue, portMAX_DELAY);
        
        if(result == pdTRUE)
        {
            uint32_t tmpValue = notifyValue;
            uint32_t Volt = 3300 * tmpValue;
            Volt = Volt >> 12;
        }
    }
}

void AppTask_A(void *argument)
{
    uint32_t notifyValue = 0;
    
    for(;;)
    {
        uint32_t ulBitsToClearOnEntry = 0x00;
        uint32_t ulBitsToClearOnExit = 0xFFFFFFFF;
        BaseType_t result = xTaskNotifyWait(ulBitsToClearOnEntry, 
                                            ulBitsToClearOnExit,
                                            &notifyValue, 
                                            portMAX_DELAY);               
        
        if(result == pdTRUE)
        {
            ADCtemp = notifyValue;
            // change buffer to buffer2
            if(++cnt%2 == 0)
            {                  
                buffer = buffer2;
                elog_a(TAG,"buffer = buffer2");  
            }
            else
            {
                buffer = buffer1;
                elog_a(TAG,"buffer = buffer1");
            }
            
            xTaskNotify(Task_BHandle, 
                        notifyValue,
                        eSetValueWithOverwrite);
        }
    }
}

void AppTask_B(void *argument)
{
    uint32_t notifyValue = 0;
    
    for(;;)
    {
        uint32_t ulBitsToClearOnEntry = 0x00;
        uint32_t ulBitsToClearOnExit = 0xFFFFFFFF;
        BaseType_t result = xTaskNotifyWait(ulBitsToClearOnEntry, 
                                            ulBitsToClearOnExit,
                                            &notifyValue, 
                                            portMAX_DELAY);        
        
        if(result == pdTRUE)
        {
            uint32_t Volt;            
            ADCtemp = notifyValue;
            Volt = 3300*notifyValue;
            ADCtemp = Volt;
            Volt = Volt>>12;       
            ADCtemp = Volt;
            elog_a(TAG,"Volt = %d", Volt);      
        }
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{        
    uint32_t ulConversionResult;
    
    if(hadc->Instance != ADC1)
        return;

    ulConversionResult = HAL_ADC_GetValue(hadc);        
    
    if(Task_AHandle != NULL)
    {        
        BaseType_t taskWoken = pdFALSE;
        xTaskNotifyFromISR(Task_AHandle, 
                            ulConversionResult,
                            eSetValueWithOverwrite, 
                            &taskWoken);        
        portYIELD_FROM_ISR(taskWoken);
    }     
}
/* USER CODE END Application */

