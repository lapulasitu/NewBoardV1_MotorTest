#ifndef __MOTOR_H__
#define __MOTOR_H__

//Left
#define LeftFootMove(Speed)					TIM_SetCompare2(TIM5, Speed)
#define LeftKneeMove(Speed)		    	TIM_SetCompare1(TIM5, Speed)
#define LeftArmMove(Speed)					TIM_SetCompare2(TIM1, Speed)
#define	LeftWristMove(Speed)				TIM_SetCompare1(TIM1, Speed)

//Middle
#define	HeadMove(Speed)							TIM_SetCompare4(TIM4, Speed)
#define WaistMove(Speed)						TIM_SetCompare3(TIM4, Speed)

//Right
#define RightFootMove(Speed)				TIM_SetCompare2(TIM3, Speed)
#define RightKneeMove(Speed)		    TIM_SetCompare1(TIM3, Speed)
#define RightArmMove(Speed)				  TIM_SetCompare4(TIM1, Speed)
#define	RightWristMove(Speed)			  TIM_SetCompare3(TIM1, Speed)

#endif

