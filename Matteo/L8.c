uint16_t MEMS_WR_ADDRESS = 0b01010000; //LISDE write and read addresses
uint16_t MEMS12_WR_ADDRESS = 0b00110000; //LISDE12 write and read addresses
uint8_t CTRL_REG1[] = {0x20, 0b00010111}; //reg address, 1Hz + normal mode + XYZ enabled
uint8_t CTRL_REG2[] = {0x21, 0b00000000}; //reg address, no HPF (default value at startup)
uint8_t CTRL_REG4[] = {0x23, 0b00000000}; //reg address, continuos update + 2g FSR + self test disabled (default value at st)
uint16_t MEMS_REGISTER_X_AUTO_INCREMENT = 0x29 | 0x80; // Setting MSB to 1 to enable auto-increment
uint16_t size = 1; // #Bytes of initial I2C transmissions
uint16_t multiple_size = 5; // #Bytes to read (X, Y, Z)
uint32_t timeout = 10; // Timeout for initial I2C transmissions
char string[32];
int string_length = 0;
int8_t xyz_data[5]; // Array to hold X, Y, Z data

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	xyz_data[0] = 0;
	xyz_data[2] = 0;
	xyz_data[4] = 0;
    // Transmit complete, now initiate a receive to read X, Y, Z
	HAL_I2C_Master_Receive_DMA(&hi2c1, (MEMS_WR_ADDRESS + 1), (uint8_t*)xyz_data, multiple_size);
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	float acc_g_x = xyz_data[0] / 32.0;
	float acc_g_y = xyz_data[2] / 32.0;
	float acc_g_z = xyz_data[4] / 32.0;
	string_length = snprintf(string, sizeof(string), "X:%.2fg, Y:%.2fg, Z:%.2fg\n", acc_g_x, acc_g_y, acc_g_z);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)string, string_length);
}

In main:
    if (HAL_I2C_Master_Transmit(&hi2c1, MEMS_WR_ADDRESS, CTRL_REG1, sizeof(CTRL_REG1), timeout) == HAL_OK) {
	  string_length = snprintf(string, sizeof(string), "LIS2DE found!\n");
    }
    else {
        if (HAL_I2C_Master_Transmit(&hi2c1, MEMS12_WR_ADDRESS, CTRL_REG1, sizeof(CTRL_REG1), timeout) == HAL_OK) {
            string_length = snprintf(string, sizeof(string), "LIS2DE12 found!\n");
            MEMS_WR_ADDRESS = MEMS12_WR_ADDRESS;
        }
        else {
            string_length = snprintf(string, sizeof(string), "Error! No device found!\n");
        }
    }
    HAL_UART_Transmit_DMA(&huart2, (uint8_t*)string, string_length);

    HAL_I2C_Master_Transmit(&hi2c1, MEMS_WR_ADDRESS, CTRL_REG2, sizeof(CTRL_REG2), timeout);
    HAL_I2C_Master_Transmit(&hi2c1, MEMS_WR_ADDRESS, CTRL_REG4, sizeof(CTRL_REG4), timeout);
    // Transmit the X register address with auto-increment enabled
    HAL_I2C_Master_Transmit_DMA(&hi2c1, MEMS_WR_ADDRESS, (uint8_t*)&MEMS_REGISTER_X_AUTO_INCREMENT, size);

    HAL_TIM_Base_Start_IT(&htim2);