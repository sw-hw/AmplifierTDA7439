/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "TDA7439/TDA7439.h"
#include "ILI9488/ILI9488_STM32_Driver.h"
#include "ILI9488/ILI9488_GFX.h"
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
__IO EncoderRotate_t EncoderRotate = ENCODER_ROTATE_NO;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId TaskILI9488Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTaskILI9488(void const * argument);

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
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of TaskILI9488 */
  osThreadDef(TaskILI9488, StartTaskILI9488, osPriorityLow, 0, 128);
  TaskILI9488Handle = osThreadCreate(osThread(TaskILI9488), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
    
    
    

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
	taskENTER_CRITICAL();
	// ---
	TDA7439_EncoderButton(HAL_GPIO_ReadPin(ENCODER_A_GPIO_Port, ENCODER_A_Pin));
	// ---
	TDA7439_EncoderRotate(EncoderRotate);
	EncoderRotate = ENCODER_ROTATE_NO;
	// ---
	HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
	// ---
	taskEXIT_CRITICAL();
	osDelay(50);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTaskILI9488 */
/**
* @brief Function implementing the TaskILI9488 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskILI9488 */
void StartTaskILI9488(void const * argument)
{
  /* USER CODE BEGIN StartTaskILI9488 */
  /* Infinite loop */
  for(;;)
  {
	  // TODO
	  osDelay(100);
  }
  /* USER CODE END StartTaskILI9488 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  UBaseType_t uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
  if(GPIO_Pin == ENCODER_C_Pin)
	  EncoderRotate = (HAL_GPIO_ReadPin(ENCODER_B_GPIO_Port, ENCODER_B_Pin) == GPIO_PIN_SET) ? ENCODER_ROTATE_R : ENCODER_ROTATE_L;
  taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
