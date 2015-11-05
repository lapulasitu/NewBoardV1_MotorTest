#include "sys.h"
#include "motor.h"




u32 getHclkFrequency(void)
{
	RCC_ClocksTypeDef rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	return (u32)rcc_clocks.HCLK_Frequency;
}



//Sleep1(PE2) Sleep2(PE3) Sleep3(PD1) Sleep4(PB13) Sleep5(PD0)
void chipEnable(void)
{
	
	//Enable DRV8848
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;			 //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//LED5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//Enable
	GPIO_SetBits(GPIOD,GPIO_Pin_0);
	GPIO_SetBits(GPIOD,GPIO_Pin_1);
	GPIO_SetBits(GPIOE,GPIO_Pin_2);
	GPIO_SetBits(GPIOE,GPIO_Pin_3);
	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	
	//LED
	GPIO_SetBits(GPIOC,GPIO_Pin_7);
}


/*
 *************************************************************************************
 *	                           Overview
 *____________________________________________________________________________________
 *	TIM5(CH1~CH2)						Motor1~2    
 *  TIM1(CH1~CH4)     			Motor3~4   Motor10~9
 *  TIM3(CH1~CH2 Remap)			Motor6~5
 *  TIM4(CH3~CH4 Remap)     Motor8~7
 *
 *************************************************************************************
 *
 *                              Detail
 *____________________________________________________________________________________
 *
 *	  Motor		   Part			   |		  TIM       |      Pin        |       Remap
 * -----------------------------------------------------------------
 *	M1(PWM1)		 Left Knee 		   TIM5_CH1            PA0                 NO
 *	M2(PWM2)     Left Foot       TIM5_CH2            PA1                 NO
 *
 *	M3(PWM3)		 Left Wirst 	   TIM1_CH1            PA8                 NO
 *	M4(PWM4)     Left Arm        TIM1_CH2            PA9                 NO
 *
 *	M5(PWM5)		 Right Foot 	   TIM3_CH2            PB5                 YES
 *	M6(PWM6)     Right Wrist     TIM3_CH1            PB4                 YES
 *
 *	M7(PWM7)		 Head 		       TIM4_CH4            PD15                YES
 *	M8(PWM8)     Waist           TIM4_CH3            PD14                YES
 *
 *	M9(PWM9)		 Right Arm 		   TIM1_CH4            PA11                NO
 *	M10(PWM10)   Right Wrist     TIM1_CH3            PA10                NO
 * 	
 */

void motorCfg(void)
{
	/* -1-  */
	GPIO_InitTypeDef Gpio_InitStr;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	/* -2-  */
	//TIM1 Clock Enable
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	//TIM3 TIM4 TIM5 Clock Enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM5, ENABLE);
	
	/* -3-  */
	// GPIOA GPIOB GPIOD clock enable
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	
	
  //TIM1 CH1(PA8) CH2(PA9) CH3(PA10) CH4(PA11)
	//TIM1 No Remap
	Gpio_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;
	Gpio_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
 	Gpio_InitStr.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOA, &Gpio_InitStr);
	
	//TIM3 CH1(PB4) CH2(PB5)
	//TIM3 Partial Remap TIM3_CH1&TIM3_CH2
	//TIM3_CH1: PA6 --- PB4 
	//TIM3_CH2: PA7 --- PB5
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	//¹Ø±Õjtag
	
	//TIM2 Partial Remap
	//PA15
	Gpio_InitStr.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &Gpio_InitStr);
	//PB3
	Gpio_InitStr.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &Gpio_InitStr);
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
	
	
	Gpio_InitStr.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOB, &Gpio_InitStr);
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	
	//TIM4 CH3(PD14) CH4(PD15)
	//TIM4 Full Remap (Motor use CH3~CH4  Ultrasonic use CH1~CH2)
	//TIM4_CH1: PB6 --- PD12
	//TIM4_CH2: PB7 --- PD13
	//TIM4_CH3: PB8 --- PD14
	//TIM4_CH4: PB9 --- PD15
	Gpio_InitStr.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOD, &Gpio_InitStr);
	GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);
	
	//TIM5 CH1(PA0) CH2(PA1)
	//TIM1 No Remap
	Gpio_InitStr.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOA, &Gpio_InitStr);
	
	/* -4-  */
	// Time Base configuration
	TIM_TimeBaseStructure.TIM_Prescaler = getHclkFrequency()/2000000-1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 100-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  
	//TIM1
 	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	//TIM2
 	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	//TIM3
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	//TIM4
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	//TIM5
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	//PWM set
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;//Init with duty cycle zero.
	//High active
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  //TIM1 
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	
	//TIM2 (PWM CH1~CH2)
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	//TIM3(PWM CH1~CH2) 
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	
	//TIM4(PWM CH3~CH4) 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	
	//TIM5
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);
	
	//TIM1
	TIM_Cmd(TIM1, ENABLE);
	//TIM1
	TIM_Cmd(TIM2, ENABLE);
	//TIM3
	TIM_Cmd(TIM3, ENABLE);
	//TIM4
	TIM_Cmd(TIM4, ENABLE);
	//TIM5
	TIM_Cmd(TIM5, ENABLE);

	//TIM1 Main Output Enable
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	//TIM2 Main Output Enable
	TIM_CtrlPWMOutputs(TIM2, ENABLE);
	
	//TIM3 Main Output Enable
	TIM_CtrlPWMOutputs(TIM3, ENABLE);
	
	//TIM4 Main Output Enable
	TIM_CtrlPWMOutputs(TIM4, ENABLE);
	
	//TIM5 Main Output Enable
	TIM_CtrlPWMOutputs(TIM5, ENABLE);
	
	/* -5-  */
	

	TIM_SetCompare1(TIM2, 50);
	TIM_SetCompare2(TIM2, 50);
	
//	//TIM1(PWM CH1~CH4) 
//	TIM_SetCompare1(TIM1, 50);
//	TIM_SetCompare2(TIM1, 50);
//  TIM_SetCompare3(TIM1, 50);
//	TIM_SetCompare4(TIM1, 50);
//	
//	//TIM3(PWM CH1~CH2) 
//	TIM_SetCompare1(TIM3, 50);
//	TIM_SetCompare2(TIM3, 50);
//	
//	//TIM4(PWM CH3~CH4) 
//	TIM_SetCompare3(TIM4, 50);
//	TIM_SetCompare4(TIM4, 50);
//	
//	//TIM5(PWM CH1~CH2) 
//	TIM_SetCompare1(TIM5, 50);
//	TIM_SetCompare2(TIM5, 50);
	
	
	LeftFootMove(50);
	LeftKneeMove(50);
	LeftArmMove(50);
	LeftWristMove(50);
	HeadMove(50);
	WaistMove(50);
	RightFootMove(50);
	RightKneeMove(70);
	RightArmMove(50);
	RightWristMove(50);
	
}



