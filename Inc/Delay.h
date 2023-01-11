/*
 * Delay.h
 *
 *  Created on: Dec 22, 2022
 *      Author: Jake Ivanov
 */

#ifndef SRC_DELAY_H_
#define SRC_DELAY_H_

#include "stm32f4xx_hal.h"

	/*
	 * @brief	Provides blocking delay in microseconds.
	 * @param	htim	TIM handle.
	 * 					Must be setup prior with microsecond per tick
	 * @param	delay	Desired delay in microseconds.
	 * 					Max delay - 65535/(2^32 - 1) microseconds (16/32 bit timer).
	 * @retval	None
	 */
	void _us_delay(TIM_HandleTypeDef htim, uint32_t delay);

#endif /* SRC_DELAY_H_ */
