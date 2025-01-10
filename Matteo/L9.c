HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)

#define TEMPO 2
uint8_t i = 0; 					//It indicates the current column that we are controlling
uint8_t size = 2;				//The number of bytes we send through the SPI
uint8_t letter_flag = 0; 		// It is used to write different letter: A = 0, H = 1
uint16_t letter_timer = 0; 		//This timer will keep track of the total time a letter has been showed
uint16_t showing_time = 4000; 	//You can put here in millisecond your preferred showing time

struct rowcolumn {
	uint8_t row;
	uint8_t column;
};

struct rowcolumn A_letter[5] = {
	{31, 16},
	{36, 8},
	{68, 4},
	{36, 2},
	{31, 1}
};

struct rowcolumn H_letter[5] = {
	{127, 16},
	{8, 8},
	{8, 4},
	{8, 2},
	{127, 1}
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim2) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET); // Do not copy the values in the output LATCHES yet
		if (letter_flag == 0)
			HAL_SPI_Transmit_DMA(&hspi1, &A_letter[i], size); // Send the column of letter A
		else
			HAL_SPI_Transmit_DMA(&hspi1, &H_letter[i], size); // Send the column of letter H

		letter_timer++;	// Increase the timer to keep track how much time has passed
		if (letter_timer == showing_time / TEMPO) { // If we reach the max showing time, we change letter
			letter_flag = !letter_flag;
			letter_timer = 0;
		}
	}
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); // When the communication is finished, the
														// values are allowed to be written in the LED Matrix
	if (i == 4)  // if we have reached the last column
		i = 0;  // return to the first column
	else
		i++;	// switch to the next column
}

In main:
    HAL_TIM_Base_Start_IT(&htim2);