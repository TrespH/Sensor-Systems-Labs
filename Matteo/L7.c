HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
// DevAddress is the device address
// pData is the data to be sent/received
// Size is the number of bytes to be sent/received

In I2C1 -> Std Mode, 7-bit Address, DMA RX, Event Interrupt enabled
#define TEMPO 1000

uint8_t LM75_WR_ADDRESS = 0b10010000; //LM75 write address
uint8_t LM75_RD_ADDRESS = 0b10010001; //LM75 read address
uint8_t LM75_TEMP_REGISTER   = 0x00;       //Address where the LM75 temperature value can be found
uint8_t data[2]  = {0}; //I2C returned data
char string[32];		 //it contains the string that will be sent via UART
int  string_length = 0;  //initial string length
int16_t concat_data = 0; //Variable to store the arranged value of the 11 bit temperature
float   out_temperature = 0; //Saving the final computed value of temperature

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		HAL_StatusTypeDef status_rx = HAL_I2C_Master_Receive_DMA(&hi2c1, LM75_RD_ADDRESS, data, 2); //LM75A
		//HAL_I2C_Master_Receive_DMA(&hi2c1, LM75_RD_ADDRESS, data, 6); //LM75B
		if (status_rx != HAL_OK) {
			string_length = snprintf(string, sizeof(string), "I2C DMA Error\n");
			HAL_UART_Transmit_DMA(&huart2, string, string_length);
		}
	}
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C1) {
		// ---------------- LM75A Sensor code ----------------- //
		concat_data = data[0] << 8 | data[1];
		// ---------------- LM75B Sensor code ----------------- //
		/*
		if (data[0] == data[2] && data[1] == data[3]) concat_data = (data[0] << 8) | data[1];
		else concat_data = (data[4] << 8) | data[5];
		*/
		// --------------------------------------------------- //
		out_temperature = concat_data / 256.0;
		string_length = snprintf(string, sizeof(string), "Temperature: %.3f C \n", out_temperature);
		HAL_UART_Transmit_DMA(&huart2, string, string_length);
	}
}

In main:
    HAL_I2C_Master_Transmit(&hi2c1, LM75_WR_ADDRESS, &LM75_TEMP_REGISTER, 2, 10);
    HAL_TIM_Base_Start_IT(&htim2);