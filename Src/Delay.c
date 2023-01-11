/*
 * Delay.c
 *
 *  Created on: Dec 22, 2022
 *      Author: Jake Ivanov
 */

#include "Delay.h"

	/*
	 * @brief	Provides blocking delay in microseconds.
	 * @param	htim	TIM handle.
	 * 					Must be setup prior with microsecond per tick
	 * @param	delay	Desired delay in microseconds.
	 * 					Max delay - 65535/(2^32 - 1) microseconds (16/32 bit timer).
	 * @retval	None
	 */
	void _us_delay(TIM_HandleTypeDef htim, uint32_t delay)
	{
		HAL_TIM_Base_Start(&htim);
		uint32_t timer_val = __HAL_TIM_GET_COUNTER(&htim);
		while ((__HAL_TIM_GET_COUNTER(&htim) - timer_val) != delay)
		{
			//Empty loop
		}
		HAL_TIM_Base_Stop(&htim);
	}


