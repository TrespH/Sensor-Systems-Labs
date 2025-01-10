// Keyboard

// Problem: shifting of the printing operation
// Pin PC13 is connected to both Push Button and Row 2!
// Solution: do the write first, so it has time (4ms) to convert the read value

#define TEMPO 4
#define TEMPO2 20

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
} Pin_Struct;

const Pin_Struct ROWS[] = {
    {GPIOC, GPIO_PIN_3},  // R0
    {GPIOC, GPIO_PIN_2},  // R1
    {GPIOC, GPIO_PIN_13}, // R2
    {GPIOC, GPIO_PIN_12}  // R3
};

const Pin_Struct COLS[] = {
    {GPIOC, GPIO_PIN_11},  // C0
    {GPIOC, GPIO_PIN_10},  // C1
    {GPIOC, GPIO_PIN_9}, // C2
    {GPIOC, GPIO_PIN_8}  // C3
};

char buttons[16] = "FEDCBA9876543210"; // Keyboard chars to be indexed by 'c' and printed
int scan = 0; // Current scanning column (0 to 3)
int c = 0, c_old = -1; // New and previous indexes in 'buttons[]'
int pressed_flag = 0; // Whether a button has been pressed and not released yet
int row = 0; // Row to be scanned at debouncing timeout
char string[3]; // UART buffer
int string_length = 0; // UART buffer length

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim2) { // Timeout to scan next keyboard's column
		if (pressed_flag == 0) { // If no button has been presssed (and not released yet)
			for (int i = 0; i < 4; i++) { // Scan all four rows to check any button pressing
				if (HAL_GPIO_ReadPin(ROWS[i].port, ROWS[i].pin) == GPIO_PIN_RESET) { // RESET <-> button pressed
					pressed_flag = 1; // Block future columns' and rows' writes and reads
					row = i; // Save the index of the row to be scanned at debouncing timeout
					c = scan + (4*i); // Expression to calculate the index in 'buttons[]'
					HAL_TIM_Base_Start_IT(&htim3); // Start the debouncing timer
				}
			}
		}

		if (pressed_flag == 0) { // If no read has been detected by the previous block of code
			scan++; // Increment scanning column
			if (scan == 4) scan = 0; // Reset 'scan' when reaching the last column
			for (int i = 0; i < 4; i++) { // Set the column indexed by 'scan' to 1, all others to 0
				if (i == scan) HAL_GPIO_WritePin(COLS[i].port, COLS[i].pin, GPIO_PIN_SET);
				else HAL_GPIO_WritePin(COLS[i].port, COLS[i].pin, GPIO_PIN_RESET);
			}
		}
	}

	else if (htim == &htim3) { // Timeout to check persistence after debouncing
		if (HAL_GPIO_ReadPin(ROWS[row].port, ROWS[row].pin) == GPIO_PIN_RESET) { // Re-read the button pressing
			if (c != c_old) { // If new 'c' differs from old one, to avoid repetitions in printing
				string_length = snprintf(string, sizeof(string), "%c\n", buttons[c]); // Parse buffer for UART
				HAL_UART_Transmit_DMA(&huart2, (uint8_t*)string, string_length); // Send buffer via UART
				c_old = c; // Update old value of 'c'
			}
		}
		else { // Button released either before or after timeout
			HAL_TIM_Base_Stop_IT(&htim3); // Stop timeout callbacks
			c_old = -1; // Reset to a value which 'c' will never hold
			pressed_flag = 0; // Allow for future writes on columns, and reads on rows
		}
	}
}

In main: 
    HAL_TIM_Base_Start_IT(&htim2);


// Encoder
HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef * htim, uint32_t Channel)
__HAL_TIM_GET_COUNTER(TIM_HandleTypeDef * htim)

In TIM3 -> Combined Channels -> Encoder Mode

#define TEMPO 1000
#define STEPS 65535 // Timer3 counter period

int16_t new_position = 0; // The newest acquired encoder position value
int16_t old_position = 0; // Old acquired encoder position value
int16_t delta = 0;
float rpm = 0;		   // Round per Minutes, Final result of encoder velocity in rotations per minute
char string[64] = {0}; // String that has to be sent via UART
uint8_t string_length = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim2) {
		old_position = new_position; // Save last position into the "old" variable
		new_position = (uint16_t) __HAL_TIM_GET_COUNTER(&htim3); // Get the new position
		delta = new_position - old_position;
		rpm = (delta / 24.0) * 60; // Compute the rpm value
		string_length = snprintf(string, sizeof(string), "position: %d, rpm: %.1f \n", new_position, rpm);
		HAL_UART_Transmit_DMA(&huart2, string, string_length);	// Send the string via UART to PC;
	}
}

In main:
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);