/*
 *  DHTemp.h
 *
 *  Created on: Dec 23, 2022
 *  Author: Yaakov (Jake) Ivanov
 *
 *  This is a library/driver for the DHT temperature and humidity sensor
 *  (AM2302) for the STM32. The library makes use of STM32's HAL definitions,
 *  and therefore should be fairly portable between platforms within their line
 *  of products.
 *
 *  This library also makes use of a function defined is "Delay.h" to attain
 *  delay in micro seconds
 */

#ifndef SRC_DHTEMP_H_
#define SRC_DHTEMP_H_

#include "stm32f4xx_hal.h" // must be modified according to target platform
#include "Delay.h"

	/*
	 * @brief	Receive RH, Temp, and Check-sum data and update parameters
	 * @param	RH variable for humidity x10 as per DHT documentation
	 * @param	temp variable for temperature x10 as per DHT documentation
	 * @retval	None
	 */
	void DHTreceive_data(int16_t *RH, int16_t *temp);

	/*
	 * @brief	Define library GPIO pin and timer parameters, initialize GPIO pin
	 * @param	GPIOx where x can be (A, B, C, etc.) to select the GPIO peripheral
	 * @param	GPIO_Pin specifies the port bit to serve as communication line
	 * 			of the form GPIO_PIN_x (where x = 0, 1, 2, etc.)
	 * @param	htim TIM handle for Delay library
	 * 			Must be setup prior with microsecond per tick
	 * @retval 	None
	 */
	void DHTinit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, TIM_HandleTypeDef htim);

	/*
	 * @brief	Begin communication between MCU and AM2302.
	 * 			The function performs preparation for communication pattern
	 * 			between the MCU and AM2302.
	 * @param	None
	 * @retval	comStatus boolean value signifying the success of establishing
	 * 			communication between the two devices
	 */
	uint8_t begin_com();

	/*
	 * @brief	Receive one bit of data
	 * @param	None
	 * @retval	dataBit RH/Temp/Check-sum bit of data
	 */
	uint8_t receive_bit();

	/*
	 * @brief	Set communication line in idle state
	 * @param	None
	 * @retval	None
	 */
	void com_set_idle();


#endif /* SRC_DHTEMP_H_ */
