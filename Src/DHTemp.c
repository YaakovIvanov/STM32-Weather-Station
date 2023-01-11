/*
 * DHTemp.c
 *
 *  Created on: Dec 23, 2022
 *      Author: Jake Ivanov
 */

#include "DHTemp.h"

	GPIO_TypeDef *gpio;
	uint16_t pin;
	TIM_HandleTypeDef timer;


	/*
	 * @brief	Receive RH, Temp, and Check-sum data and update parameters
	 * @param	RH variable for humidity x10 as per DHT documentation
	 * @param	temp variable for temperature x10 as per DHT documentation
	 * @retval	None
	 */
	void DHTreceive_data(int16_t *RH, int16_t *temp)
	{
		uint8_t checkSum = 0;
		uint16_t localRH = 0;
		uint16_t localTemp = 0;

		if (begin_com() == 1)
		{
			for (int i = 0; i < 16; i++) // Receive RH data
			{
				localRH |= receive_bit() << (15 - i);
			}
			for (int j = 0; j < 16; j++) // Receive temp data
			{
				localTemp |= receive_bit() << (15 - j);
			}
			for (int k = 0; k < 8; k++) // Receive check-sum data
			{
				checkSum |= receive_bit() << (8 - k);
			}
		}
		else //communication establishment has failed
		{
			localRH = 0;
			localTemp = 0;
		}
		if (localTemp & 0x8000) // Check if temperature is negative
		{
			*temp = -(localTemp & 0x7FFF);
		}
		else
		{
			*temp = localTemp;
		}
		*RH = localRH;

		/* Set communication line in idle for next update */
		com_set_idle();
	}

	/*
	 * @brief	Define library GPIO pin and timer parameters, initialize GPIO pin
	 * @param	GPIOx where x can be (A, B, C, etc.) to select the GPIO peripheral
	 * @param	GPIO_Pin specifies the port bit to serve as communication line
	 * 			of the form GPIO_PIN_x (where x = 0, 1, 2, etc.)
	 * @param	htim TIM handle for Delay library
	 * 			Must be setup prior with microsecond per tick
	 * @retval 	None
	 */
	void DHTinit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, TIM_HandleTypeDef htim)
	{
		/* Define library GPIO pin and timer parameters */
		gpio = GPIOx;
		pin = GPIO_Pin;
		timer = htim;

		/* Set communication line in idle state before starting communication */
		com_set_idle();
	}

	/*
	 * @brief	Begin communication between MCU and AM2302.
	 * 			The function performs preparation for communication pattern
	 * 			between the MCU and AM2302.
	 * @param	None
	 * @retval	comStatus boolean value signifying the success of establishing
	 * 			communication between the two devices
	 */
	uint8_t begin_com()
	{
		uint8_t comStatus = 0;

		/* Pull line low for the minimum of 10 ms */
		HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_RESET);
		_us_delay(timer, 10000);

		/* Pull line up for a duration of 20 - 40 us */
		HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_SET);
		_us_delay(timer, 30); // 30us delay

		/* Release line by setting the pin as input */
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(gpio, &GPIO_InitStruct);

		/* Check that sensor pulled line low after 40 us */
		_us_delay(timer, 40);
		if (HAL_GPIO_ReadPin(gpio, pin) == 0)
		{
			/* Check that sensor pulled line up after 80 us */
			_us_delay(timer, 80);
			if (HAL_GPIO_ReadPin(gpio, pin) == 1)
			{
				comStatus = 1; // Communication has been established
				while (HAL_GPIO_ReadPin(gpio, pin) != 0) // Wait until the end of preparation period
				{
					// Empty loop
				}
			}
			return comStatus;
		}
		else
		{
			return comStatus; // 0 by default - communication has not been established
		}
	}

	/*
	 * @brief	Receive one bit of data
	 * @param	None
	 * @retval	dataBit RH/Temp/Check-sum bit of data
	 */
	uint8_t receive_bit()
	{
		while (HAL_GPIO_ReadPin(gpio, pin) == 0) // Wait for sensor to pull line high
		{
			// Empty loop
		}
		_us_delay(timer, 40); // Skip 40 us
		uint8_t dataBit = HAL_GPIO_ReadPin(gpio, pin);
		if (dataBit == 1)
		{
			while (HAL_GPIO_ReadPin(gpio, pin) != 0) /* If the bit was 1, we have to wait for
			another ~30us for the sensor pull down the line */
			{
				// Empty loop
			}
		}
		return dataBit;
	}

	/*
	 * @brief	Set communication line in idle state
	 * @param	None
	 * @retval	None
	 */
	void com_set_idle()
	{
		/* Initialize GPIO pin communication line as output */
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_RESET);
		GPIO_InitStruct.Pin = pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(gpio, &GPIO_InitStruct);

		/* Pull up communication line */
		HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_SET);
	}
