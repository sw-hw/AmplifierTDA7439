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
#include "VU/vu.h"
#include "ILI9488/ILI9488_STM32_Driver.h"
#include "ILI9488/ILI9488_GFX.h"
#include "adc.h"
#include <stdlib.h>
#include <math.h>
#include "IR/NEC.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define ADC_K_IIR			0.0001f	// coef. IIR filters (it needs to be calculated according ADC_FREQ_DIF)
#define ADC_REF_0DB 		1024.0f	// reference value corresponding to 0 dB
#define	ADC_CONST_OFFSET	2042.0f	// init value for IIR filters
#define	ADC_FREQ_DIV		30

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
__IO EncoderRotate_t EncoderRotate = ENCODER_ROTATE_NO;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId TaskVUHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
int16_t realToInt(float value);
int16_t ConvertTo_dB(int16_t value);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTaskVU(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationTickHook(void);

/* USER CODE BEGIN 3 */
void vApplicationTickHook(void)
{
	/*
	 * vApplicationTickHook not calling while task in critical section
	 * */
	// ===
	NEC_Tick();
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

  /* definition and creation of TaskVU */
  osThreadDef(TaskVU, StartTaskVU, osPriorityLow, 0, 128);
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
	  TDA7439_EncoderButton(HAL_GPIO_ReadPin(ENCODER_A_GPIO_Port, ENCODER_A_Pin));
	  // ---
	  TDA7439_EncoderRotate(EncoderRotate);
	  EncoderRotate = ENCODER_ROTATE_NO;
	  // ---
	  TDA7439_ButtonCode(NEC_GetCommand());
	  // ---
	  osDelay(25); // should be less than period of commands NEC protocol
  }
  /* USER CODE END StartDefaultTask */
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
  /* Infinite loop */
  for(;;)
  {
	  taskENTER_CRITICAL();
	  if(TDA7439_GetAmplifierState())
	  {
		  static int16_t db_left  = 0x8000;
		  static int16_t db_right = 0x8000;
		  static float avg_left_f  = ADC_CONST_OFFSET;
		  static float avg_right_f = ADC_CONST_OFFSET;
		  static int16_t adc_counter = -1;
		  static int16_t left_max = 0;
		  static int16_t right_max = 0;
		  int16_t adc_left, adc_right, cur_left, cur_right;
		  // ---
		  if(adc_counter != -1)
		  {
			  adc_left = (int16_t)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
			  adc_right = (int16_t)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
		  }
		  else
		  {
			  adc_left = ADC_CONST_OFFSET;
			  adc_right = ADC_CONST_OFFSET;
			  adc_counter = 0;
		  }
		  HAL_ADCEx_InjectedStart(&hadc1);
		  // ---
		  cur_left = abs(adc_left - realToInt(avg_left_f));
		  cur_right = abs(adc_right - realToInt(avg_right_f));
		  // ---
		  avg_left_f = avg_left_f * (1.0f - ADC_K_IIR) + adc_left * ADC_K_IIR;
		  avg_right_f = avg_right_f * (1.0f - ADC_K_IIR) + adc_right * ADC_K_IIR;
		  // ---
		  if(cur_left > left_max)
			  left_max = cur_left;
		  if(cur_right > right_max)
			  right_max = cur_right;
		  // ---
		  adc_counter++;
		  if(0 == (adc_counter % ADC_FREQ_DIV))
		  {
			  db_left  = ConvertTo_dB(left_max);
			  db_right = ConvertTo_dB(right_max);
			  // ---
			  left_max = 0;
			  right_max = 0;
			  adc_counter = 0;
		  }
		  // ---
		  // ===
		  /*
		  if(db_left >= -45 || db_right >= -45)
		  {
			  volatile uint32_t just_for_debug = 0;
		  }//*/
		  // ===
		  VU_DisplaySignal(db_left, db_right);
	  }
	  taskEXIT_CRITICAL();
	  osDelay(1);
  }
  /* USER CODE END StartTaskVU */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		case ENCODER_C_Pin:
			if(HAL_GPIO_ReadPin(ENCODER_B_GPIO_Port, ENCODER_B_Pin) == GPIO_PIN_RESET)
				EncoderRotate = ENCODER_ROTATE_R;
			break;
		case ENCODER_B_Pin:
			if(HAL_GPIO_ReadPin(ENCODER_C_GPIO_Port, ENCODER_C_Pin) == GPIO_PIN_RESET)
				EncoderRotate = ENCODER_ROTATE_L;
			break;
		case IR_Pin:
			NEC_SignalEdge();
			break;
		default:
			break;
	}
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
