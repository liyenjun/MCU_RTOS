/******************************************************************************
 * Copyright (C) 2025 xxx, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file bsp_key.c
 * 
 * @par dependencies 
 * - bsp_key.h
 * - stdio.h
 * - stdint.h
 * 
 * @author YC | R&D Dept. | xxx, Inc.
 * 
 * @brief Provide the HAL APIs of AHT21 and corresponding opetions.
 * 
 * Processing flow:
 * 
 * call directly.
 * 
 * @version V1.0 2025-01-22
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/
 
#include "bsp_key.h"

osThreadId_t key_TaskHandle;

QueueHandle_t key_queue;

const osThreadAttr_t key_Task_attributes = {
  .name = "key_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};

/* *
 * @brief detect key button
 * 
 * Steps:
 *  1.doing the specific operations
 *  
 * @param[in] void 
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
 
 key_status_t key_scan(key_press_status_t * key_value)
 {

     key_press_status_t key_status_value = KEY_NOT_PRESSED;
     

     if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin))
     {
         key_status_value = KEY_PRESSED;
         *key_value = key_status_value;
         
         return KEY_OK;
     }
     
     *key_value = key_status_value;
     
     return KEY_ERRORTIMEOUT;     
 }
 
 /* *
 * @brief check the time of press is long or short
 * 
 * Steps:
 *  1.doing the specific operations
 *  
 * @param[in] void 
 * 
 * @return  KEY_SHORT_PRESSED : key has been pressed with short time.
 *          KEY_LONG_PRESSED  : key has been pressed with long time.
 * */
 key_status_t key_scan_short_long_press(key_press_status_t * key_value,
                                        uint32_t short_press_time)
 {
     key_status_t ret_key_status = KEY_OK;
     key_press_status_t key_value_temp = KEY_NOT_PRESSED;
     uint32_t counter_tick = 0;
     
     // 1. check if the key is pressed
     ret_key_status = key_scan(&key_value_temp);
     
     // 1.1 if the key is pressed, then check if it is short pressed
     if(KEY_OK == ret_key_status)
     {
         // get the timestamp of the first key press
         counter_tick = HAL_GetTick(); 
         
         // 1.1.1 check the short press time
         while( HAL_GetTick() <  counter_tick + short_press_time )
         {
             ;
         }
         
         // 1.1.2 now could check if the user pressed the key 
         ret_key_status = key_scan(&key_value_temp);
         
         if ( KEY_NOT_PRESSED == key_value_temp )
         {
             // 1.1.3 the key is pressed with short press
             *key_value = KEY_SHORT_PRESSED;
             
             return KEY_OK;
         }
         else
         {
             // 1.2 the key is pressed with long press
             *key_value = KEY_LONG_PRESSED;
             
             // 1.2.1 keep focuing on the status of key to avoid the short
            // press
             while( KEY_OK == key_scan(&key_value_temp) ) // key is pressed
             {
                 ;
             }
             
             return KEY_OK;
         }
     }
     
     return ret_key_status;
 }
 /* *
 * @brief key_task_func for thread to run.
 * 
 * Steps:
 *  1.doing the specific operations
 *  
 * @param[in] void 
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
 void key_task_func(void *argument)
 {
     key_status_t key_ret = KEY_OK;
     key_press_status_t key_status = KEY_NOT_PRESSED;
     key_queue = xQueueCreate(10, sizeof(uint32_t));
     uint32_t counter_tick = 0;
     
     if(NULL==key_queue)
     {
         printf("key_queue created failed \r\n");
     }
     else
     {
         printf("key_queue created successfully \r\n");
     }
     
     for(;;)
     {
         counter_tick++;
         
         key_ret = key_scan(&key_status);
         if(KEY_OK == key_ret)
         {
             if(KEY_PRESSED == key_status)
             {
                 printf("Key_Pressed\r\n");
                 if(pdTRUE == xQueueSendToFront(key_queue,&counter_tick,0))
                 {
                     printf("send successfully\r\n");
                 }
             }
         }
         
         if(KEY_OK != key_ret)
         {
              printf("Key_not_Pressed\r\n");
         }
         osDelay(100);
     }
 }
 