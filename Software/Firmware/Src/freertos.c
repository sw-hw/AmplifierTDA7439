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
#include "adc.h"
#include <stdlib.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
	int16_t last_right;
	int16_t avg_right;
	int16_t max_right;
	int16_t signal_right_db;
	int16_t last_left;
	int16_t avg_left;
	int16_t max_left;
	int16_t signal_left_db;
} ADC_Signals_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define ADC_K_IIR			0.001f   // coef. IIR filters
#define ADC_REF_0DB 		1024.0f	 // reference value corresponding to 0 dB
#define	ADC_CONST_OFFSET	1988.0f	 // init value for IIR filters
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
__IO EncoderRotate_t EncoderRotate = ENCODER_ROTATE_NO;
__IO ADC_Signals_t ADC_Signals;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId TaskILI9488Handle;
osThreadId TaskVUHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
int16_t realToInt(float value);
int16_t ConvertTo_dB(int16_t value);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTaskILI9488(void const * argument);
void StartTaskVU(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationTickHook(void);

/* USER CODE BEGIN 3 */
void vApplicationTickHook( void )
{
	if(TDA7439_GetAmplifierState())
		HAL_ADCEx_InjectedStart_IT(&hadc1);
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	int16_t signal_r_abs, signal_l_abs;
	ADC_Signals.last_right = (int16_t)HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
	signal_r_abs = abs(ADC_Signals.last_right - ADC_Signals.avg_right);
	if(signal_r_abs > ADC_Signals.max_right)
		ADC_Signals.max_right = signal_r_abs;
	ADC_Signals.last_left = (int16_t)HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_2);
	signal_l_abs = abs(ADC_Signals.last_left - ADC_Signals.avg_left);
	if(signal_l_abs > ADC_Signals.max_left)
		ADC_Signals.max_left = signal_l_abs;
}

int16_t realToInt(float value)
{
	return (int16_t)(value < 0 ? value - 0.5f : value + 0.5f);
}

int16_t ConvertTo_dB(int16_t value)
{
	float res;
	if(value == 0)
		value = 1;
    res = -20.0f * (float)log10(ADC_REF_0DB / value);
    if(res < 0.0f)
        return ((int16_t)res) - 1;
    else
        return (int16_t)res;
}

/* USER CODE END 3 */

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

  /* definition and creation of TaskVU */
  osThreadDef(TaskVU, StartTaskVU, osPriorityNormal, 0, 128);
  TaskVUHandle = osThreadCreate(osThread(TaskVU), NULL);

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
	//HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
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
	  taskENTER_CRITICAL();
	  TDA7439_DisplaySignal(ADC_Signals.signal_left_db, ADC_Signals.signal_right_db);
	  taskEXIT_CRITICAL();
	  osDelay(1);
  }
  /* USER CODE END StartTaskILI9488 */
}

/* USER CODE BEGIN Header_StartTaskVU */
/**
* @brief Function implementing the TaskVU thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskVU */
void StartTaskVU(void const * argument)
{
  /* USER CODE BEGIN StartTaskVU */
  float avg_left  = ADC_CONST_OFFSET;
  float avg_right = ADC_CONST_OFFSET;
  /* Infinite loop */
  for(;;)
  {
	  taskENTER_CRITICAL();
	  if(TDA7439_GetAmplifierState())
	  {
		  avg_right = avg_right * (1.0f - ADC_K_IIR) + ADC_Signals.last_right * ADC_K_IIR;
		  ADC_Signals.avg_right = realToInt(avg_right);
		  ADC_Signals.signal_right_db = ConvertTo_dB(ADC_Signals.max_right);
		  ADC_Signals.max_right = 0;
		  avg_left = avg_left * (1.0f - ADC_K_IIR) + ADC_Signals.last_left * ADC_K_IIR;
		  ADC_Signals.avg_left = realToInt(avg_left);
		  ADC_Signals.signal_left_db = ConvertTo_dB(ADC_Signals.max_left);
		  ADC_Signals.max_left = 0;
	  }
	  taskEXIT_CRITICAL();
	  osDelay(100);
  }
  /* USER CODE END StartTaskVU */
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
