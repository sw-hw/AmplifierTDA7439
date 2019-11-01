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
#define ADC_K_IIR			0.0001f  // coef. IIR filters
#define ADC_REF_0DB 		1024.0f	 // reference value corresponding to 0 dB
#define	ADC_CONST_OFFSET	2043.0f	 // init value for IIR filters
#define	VU_RING_LEN			30		 // length of rings ADC data (sampling frequency = 1 KHz)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
__IO EncoderRotate_t EncoderRotate = ENCODER_ROTATE_NO;
__IO int16_t VU_left_db  = 0x8000;
__IO int16_t VU_right_db = 0x8000;
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
void vApplicationTickHook( void )
{
	if(TDA7439_GetAmplifierState())
	{
		static float avg_left_f  = ADC_CONST_OFFSET;
		static float avg_right_f = ADC_CONST_OFFSET;
		static int16_t L[VU_RING_LEN] = { [0 ... (VU_RING_LEN - 1)] = 0 };
		static int16_t R[VU_RING_LEN] = { [0 ... (VU_RING_LEN - 1)] = 0 };
		static uint16_t vu_counter = 0;
		int16_t last_left = 0, last_right = 0, l_max = 0, r_max = 0;
		// ---
		last_left = (int16_t)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
		last_right = (int16_t)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
		// ---
		L[vu_counter] = abs(last_left - realToInt(avg_left_f));
		avg_left_f = avg_left_f * (1.0f - ADC_K_IIR) + last_left * ADC_K_IIR;
		R[vu_counter] = abs(last_right - realToInt(avg_right_f));
		avg_right_f = avg_right_f * (1.0f - ADC_K_IIR) + last_right * ADC_K_IIR;
		vu_counter++;
		if(vu_counter == VU_RING_LEN)
			vu_counter = 0;
		for(uint16_t i = 0; i < VU_RING_LEN; i++)
		{
			if(L[i] > l_max)
				l_max = L[i];
			if(R[i] > r_max)
				r_max = R[i];
		}
		VU_left_db  = ConvertTo_dB(l_max);
		VU_right_db = ConvertTo_dB(r_max);
		/* TODO
		 * if VU_left_db > 0 or VU_right_db > 0 duration > 3000 ms, turn OFF power amplifier for 5000ms
		 * */
		// ===
		HAL_ADCEx_InjectedStart(&hadc1);
	}
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
	  taskENTER_CRITICAL();
	  // ---
	  TDA7439_EncoderButton(HAL_GPIO_ReadPin(ENCODER_A_GPIO_Port, ENCODER_A_Pin));
	  // ---
	  TDA7439_EncoderRotate(EncoderRotate);
	  EncoderRotate = ENCODER_ROTATE_NO;
	  // ---
	  // TODO NEC_Command
	  // ---
	  taskEXIT_CRITICAL();
	  osDelay(25);
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
		  VU_DisplaySignal(VU_left_db, VU_right_db);
	  taskEXIT_CRITICAL();
  }
  /* USER CODE END StartTaskVU */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	UBaseType_t uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
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
			NEC_SetEdge(HAL_GPIO_ReadPin(IR_GPIO_Port, IR_Pin) == GPIO_PIN_SET);
			break;
		default:
			break;
	}
	taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
