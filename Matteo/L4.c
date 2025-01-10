HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef* huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef* huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
// Direct Memory Access functions:
HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef* huat, uint8_t *pData, uint16_t Size)
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef* huart, uint8_t *pData, uint16_t Size)
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)

// USART
// Set USARTx BaudRate and DMA TX
char buffer[64]; // empty buffer with predefined maximum capacity
float voltage = 1.5;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim2) {
		int length = snprintf(buffer, sizeof(buffer), "%.3f\n", voltage *= 1.2); // Project -> Settings -> MCU Settings -> Use float with printf
		HAL_UART_Transmit_DMA(&huart2, buffer, length); 
	}
}

// LCD
/*
Copy the “PMDB16_LCD.c” file to your project/Core/Src folder
Copy the “PMDB16_LCD.h” file to your project/Core/Inc folder
Add #include "PMDB16_LCD.h“ in your main.c
*/

char *names[] = {
	"Andre Maffezzini",
	"Giulio Lotto",
    "Marco La Barbera",
    "Matteo Pompilio",
    "Tommaso Majocchi"
};

uint8_t MEMBERS = sizeof(names)/sizeof(names[0]);
uint8_t row = 0;
uint8_t flag_first_round = 0;
int8_t name_index = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim2) {
		lcd_clear(); // Useless
		if (flag_first_round == 0) {
			lcd_println(names[name_index], row + 1); // initialized with member 1 on the bottom row
			flag_first_round = 1;
		}
		else {
			lcd_println(names[name_index], row); // print old names on top
			if (name_index == MEMBERS - 1) 	// if LCD has reached the last name,
				name_index = -1; 			// reset name_index to -1 to allow next print to work properly and put first name at the bottom row
			lcd_println(names[name_index + 1], row + 1); // print new names on bottom
			name_index++;
		}
	}
}

In main:
    lcd_initialize();
    //lcd_backlight_ON(); // Useless, done by lcd_initialize()
    HAL_TIM_Base_Start_IT(&htim2);