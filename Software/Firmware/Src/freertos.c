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
#define	ADC_CONST_OFFSET	2047	 // init value for IIR filters
#define	VU_DIVIDER			30
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
__IO EncoderRotate_t EncoderRotate = ENCODER_ROTATE_NO;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
int16_t realToInt(float value);
int16_t ConvertTo_dB(int16_t value);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

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
	// TODO try to use rings
	static float avg_left_f  = ADC_CONST_OFFSET;
	static float avg_right_f = ADC_CONST_OFFSET;
	static int16_t avg_left  = ADC_CONST_OFFSET;
	static int16_t avg_right = ADC_CONST_OFFSET;
	static uint16_t vu_counter = 0;
	static int16_t max_signal_left = 0;
	static int16_t max_signal_right = 0;
	static int16_t signal_left_db = 0x8000;
	static int16_t signal_right_db = 0x8000;
	int16_t signal_l_abs, signal_r_abs;
	int16_t last_left, last_right;
	UBaseType_t uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	// ===
	last_left = (int16_t)HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
	signal_l_abs = abs(last_left - avg_left);
	if(signal_l_abs > max_signal_left)
		max_signal_left = signal_l_abs;
	// ---
	last_right = (int16_t)HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_2);
	signal_r_abs = abs(last_right - avg_right);
	if(signal_r_abs > max_signal_right)
		max_signal_right = signal_r_abs;
	// ---
	vu_counter++;
	if(vu_counter == VU_DIVIDER)
	{
		vu_counter = 0;
		// ===
		avg_left_f = avg_left_f * (1.0f - ADC_K_IIR) + last_left * ADC_K_IIR;
		avg_left = realToInt(avg_left_f);
		signal_left_db = ConvertTo_dB(max_signal_left);
		max_signal_left = 0;
		// ---
		avg_right_f = avg_right_f * (1.0f - ADC_K_IIR) + last_right * ADC_K_IIR;
		avg_right = realToInt(avg_right_f);
		signal_right_db = ConvertTo_dB(max_signal_right);
		max_signal_right = 0;
	}
	VU_DisplaySignal(signal_left_db, signal_right_db);
	taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
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
	taskEXIT_CRITICAL();
	osDelay(50);
  }
  /* USER CODE END StartDefaultTask */
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
