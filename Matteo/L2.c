GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinStatePinState)
void HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)

#define BluePB GPIOC, GPIO_PIN_13
#define LD2 GPIOA, GPIO_PIN_5
#define Mic GPIOC, GPIO_PIN_8

//int flag = 0;
int last_tick, tick, delta;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
		case GPIO_PIN_13: {
			//if (HAL_GPIO_ReadPin(BluePB) == GPIO_PIN_RESET) flag = !flag;
			//HAL_GPIO_WritePin(LD2, flag);
			HAL_GPIO_TogglePin(LD2);
			// NVIC -> Enable EXTI
			// EXTI -> Interrupt mode with Rising/Falling Edges
			break;
		}
		case GPIO_PIN_8: {
			tick = HAL_GetTick();
			delta = tick - last_tick;
			if (delta >= 100) {
				HAL_GPIO_TogglePin(LD2);
				last_tick = tick;
			}
			break;
		}
		default: break;
	}
}