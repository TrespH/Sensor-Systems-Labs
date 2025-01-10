// ADC Potentiometer, Internal Temperature and Vref
In ADC -> Scan Conversion Mode Enabled, DMA Continuous Request Enabled, #Conversions 3, DMA Circular
#define CHANNEL_NUMBER 3

uint16_t in_data[3] = {0};
char string[64] = {0};
char string_length = 0;
float v_pot = 0;
float v_temp = 0;
float v_ref = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	v_pot  = in_data[0]*3.3/4096.0;
	v_temp = (in_data[1]*3.3/4096.0-0.76)/0.0025+25;
	v_ref  = in_data[2]*3.3/4096.0;
	string_length = snprintf(string, sizeof(string), "Potentiometer: %.3f V; Temperature: %.3f C; Vref: %.3f V\n", v_pot, v_temp, v_ref);
	HAL_UART_Transmit_DMA(&huart2, string, string_length);
}

In main:
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_ADC_Start_DMA(&hadc1, in_data, CHANNEL_NUMBER); // Circular mode -> no need to call it again (&in_data if not an array!)

// ADC LDR
In Analog: ADC1 -> IN0 Enabled -> DMA Continuous Conversion Enabled, DMA Circular
#define VDDA 3.3
#define VSSA 0
#define resolution_bits 12
#define TEMPO 1				 //1 = 1ms, 10 = 10ms ...
#define buffer_length 1000   //buffer dimension

float FSR = VDDA - VSSA;					//full scale range
int resolution_steps = pow(2, resolution_bits);		//resolution_step is 2^resolution_bits
uint16_t in0_data[buffer_length];   //buffer that contains ADC sampled values
float    sum = 0;					//it contains the sum of all the in0_data, it will be used to compute the average
float    average;					//it contains the average of all the in0_data
float    average_volt = 0; 			//average of the data expressed in volt
float ldr;					//light detection resistance
float lux;					//brightness measurements units
char string[100];			//it contains the string that will be sent via UART
int  string_length;

/*void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
	for (int i = 0; i < (buffer_length / 2); i++) { //process the first half of the data
		sum = sum + in0_data[i]; //sum of the first half of the data
	}
}*/

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	for (int i = 0; i < buffer_length; i++) {
		sum = sum + in0_data[i]; //sum of the data
	}
	average = sum/buffer_length;							  //average of all the data
	average_volt = average*FSR/resolution_steps;	  //average value converted in volt
	ldr = (average_volt*100000)/(FSR - average_volt); //light detection resistance computation
	lux = 10 * pow((100000 / ldr), 1.25);				  //brightness measurements units computation
	string_length = snprintf(string, sizeof(string), "LDR %.3f, LUX %.3f \n", ldr, lux); //converts the values into strings
	sum = 0;	//reset sum to 0, ready to be processed in a new ADC callback
	HAL_UART_Transmit_DMA(&huart2, string, string_length); //send the string to the PC via UART
}

In main:
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_ADC_Start_DMA(&hadc1, in0_data, buffer_length);