#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f10x.h"
#include "sys.h"
#define IN1 PBout(14)
#define IN2 PBout(15)	
#define IN3 PAout(6)
#define IN4 PAout(7)
void TIM1_PWM_Init(u16 arr,u16 psc);
void PWM_GPIO_Config(void);
void PWM_INIT(void);
void moter_run(u16 pwm1,u16 pwm2);
void moter_stop(void);
void moter_back(u16 pwm1, u16 pwm2);
void moter_left(u16 pwm1, u16 pwm2);
void moter_right(u16 pwm1,u16 pwm2);
void moter_spin(u16 pwm1,u16 pwm2);

#endif

