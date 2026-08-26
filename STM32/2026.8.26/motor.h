#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"

/* 电机方向控制引脚：PB14、PB13。 */
#define AIN   PBout(14)
#define BIN   PBout(13)

/* PWM 输出：TIM4_CH1(PB6)、TIM4_CH2(PB7)。 */
#define PWMA  TIM4->CCR1
#define PWMB  TIM4->CCR2

void Motor_Init(void);
void PWM_Init(u16 arr, u16 psc);
void Set_Pwm(int motor1, int motor2);

#endif
