// Starts(stops) the PWM signal generation
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef* htim, uint32_t Channel)
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef* htim, uint32_t Channel)
// Starts(stops) the TIM Base generation (x_IT with interrupt generation)
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef* htim)
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef* htim)
HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef* htim)
HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef* htim)
// Timer callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)

Blinking LED -> Pin as TIM, TIM Channel as PWM Generation, [Prescaler, Period, Pulse] as [8399, 9999, 4999]
In main -> HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

//Homework
#define PRESCALER 100
#define TIM_F 84000000/PRESCALER
#define _DO4 TIM_F/262
#define _DOD4 TIM_F/277
#define _RE4 TIM_F/294
#define _RED4 TIM_F/311
#define _MI4 TIM_F/330
#define _FA4 TIM_F/349
#define _FAD4 TIM_F/370
#define _SOL4 TIM_F/392
#define _SOLD4 TIM_F/415
#define _LA4 TIM_F/440
#define _LAD4 TIM_F/466
#define _SI4 TIM_F/494
#define PAUSA 999999	//PAUSA has been added to add pause and compose more complex songs,
						//it has been added also for terminate a song without any sound not to trigger the microphone in loop
#define TEMPO 100

struct note {
	int tone;
	int duration;
};

struct note score[] = {
		// --- London Bridge is Falling Down --- //
		{_SOL4, 6},
		{_LA4, 2},
		{_SOL4, 4},
		{_FA4, 4},
		{_MI4, 4},
		{_FA4, 4},
		{_SOL4, 8},
		{_RE4, 4},
		{_MI4, 4},
		{_FA4, 8},
		{_MI4, 4},
		{_FA4, 4},
		{_SOL4, 8},
		{_SOL4, 6},
		{_LA4, 2},
		{_SOL4, 4},
		{_FA4, 4},
		{_MI4, 4},
		{_FA4, 4},
		{_SOL4, 8},
		{_RE4, 8},
		{_SOL4, 8},
		{_MI4, 4},
		{_DO4, 12},
		{PAUSA, 2} //This terminates the song, we put 2 but whatever value is enough to avoid loops
};

int note_playing_flag;
int current_note_index = sizeof(score)/sizeof(score[0]);

void playnote(struct note note_playing) {
	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};
	  TIM_OC_InitTypeDef sConfigOC = {0};
	  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

	  htim1.Instance = TIM1;
	  htim1.Init.Prescaler = PRESCALER-1;
	  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	  htim1.Init.Period = note_playing.tone;
	  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htim1.Init.RepetitionCounter = 0;
	  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	  if (HAL_TIM_Base_Init(&htim1) != HAL_OK) Error_Handler();
	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) Error_Handler();
	  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) Error_Handler();
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) Error_Handler();
	  sConfigOC.OCMode = TIM_OCMODE_PWM1;
	  sConfigOC.Pulse = note_playing.tone/2; // 50% Duty Cycle for correct sound generation
	  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) Error_Handler();
	  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	  sBreakDeadTimeConfig.DeadTime = 0;
	  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK) Error_Handler();

	  /* USER CODE BEGIN TIM1_Init 2 */
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	  note_playing_flag = note_playing.duration;
	  /* USER CODE END TIM1_Init 2 */
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim2) {
		note_playing_flag--;
		if (note_playing_flag == 0) {	// If note duration has been entirely played
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
			current_note_index++;
			if (current_note_index < sizeof(score)/sizeof(score[0]))
				playnote(score[current_note_index]);
		}
	}
}

void playsong() {
	note_playing_flag = 0;
	current_note_index = 0;
	playnote(score[current_note_index]);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
		case GPIO_PIN_8:
			if (current_note_index >= sizeof(score)/sizeof(score[0])) playsong();
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
			break;
		default: break;
	}
}

In main -> HAL_TIM_Base_Start_IT(&htim2);