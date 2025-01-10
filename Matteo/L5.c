HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef* hadc)
HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef* hadc) // (needed at the beginning of the code also to setup the peripheral)
HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length)
HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef* hadc) // (simply aborts the conversion)
HAL_StatusTypeDef HAL_ADC_Stop_IT(ADC_HandleTypeDef* hadc)
HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef* hadc)
// Don't need to poll for conversion inside the ADC Conv Completed callback!
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout)
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef* hadc)
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)


// USART Rx
#define LENGTH 16		// RxData dimension, it is set as the LCD maximum characters in a line
#define CHAR_SIZE 2		// The number of characters that can be send as information size (ex: min = 01, max = 99)

uint8_t RxData[LENGTH]; // Array that contains the received data
uint8_t	size[CHAR_SIZE];     // Array that contains the ASCII information of the data transfer size
uint8_t packet_size;         // The size of the packet that has to be received, converted into integer
int rx_flag = 0;             // Packet Size Received Flag
char c;			    		 // It stores the char of RxData[i]
char string[LENGTH] = {0};   // It stores the entire string received
int row = 0;		 // LCD line selector

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (rx_flag == 0) { // The size information has been just received
		rx_flag = 1;	// Set the received size information flag to 1 (1 = received)
		packet_size = (size[0] - 48)*10 + (size[1] - 48); // Integer values in ASCII table correspond to the received ASCII value - 48
		HAL_UART_Receive_DMA(&huart2, RxData, packet_size); // Then proceed with data transfer
	}
	else { // Printing the old data transfer string into LCD, and getting the size information of the new data transfer
	    for (int i = 0; i < packet_size; i++) { //Convert the received integer data into a string data
			sprintf(&c, "%c", RxData[i]);	//convert data into a char
	        strcat(string, &c);				//Add to the string the new letter (or number/symbol)
	        RxData[i] = 0;					//Empty the RxData[i]
	    }
	    lcd_clear();						// Clear the display
	    lcd_println(string, row);			// Print the new word
	    memset(string, 0, sizeof(string));	// Delete the string, ready to store the new data burst
	    rx_flag = 0;						// Ready to receive
		HAL_UART_Receive_DMA(&huart2, size, CHAR_SIZE); // Proceed with getting the length of the new data transmission
	}
}

In main:
    HAL_UART_Receive_DMA(&huart2, RxData, CHAR_SIZE);

In Matlab:
    stringToSendLength = "11";
    stringToSend = "Bravissimo!"; % This is the only part you need to modify
    %write(s, stringToSendLength, "string");
    write(s, stringToSend, "string");

// ADC IT
In Analog -> PCLK2 divided by 4, 12 bits, Right alignment, EOC flag at end of single channel -> Rank 1 & Sampling Rate 480 Cycles
char buffer[64]; // empty buffer with predefined maximum capacity
float voltage;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	if (hadc == &hadc1) {
		int reading = HAL_ADC_GetValue(&hadc1);
		voltage = reading*3.3/4096.0;
		int length = snprintf(buffer, sizeof(buffer), "%.3f\n", voltage);
		HAL_UART_Transmit_DMA(&huart2, buffer, length);
        // Or on LCD:
        int bar = (int) (reading*80/4096);
        snprintf(lcd_buffer, sizeof(lcd_buffer), "Voltage: %.3f V", voltage);
		lcd_println(lcd_buffer, 0);
		lcd_drawBar(bar);
	}
}
In main: (TIM2 -> TRGO -> Update Event; ADC -> External Trigger Conversion Source: TIM2 TRGO)
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_ADC_Start_IT(&hadc1);