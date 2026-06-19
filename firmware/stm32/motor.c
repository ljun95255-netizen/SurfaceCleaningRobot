#include "motor.h"

void TIM1_PWM_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE); 
	//PWM频率：	 Freq=ck_psc/（psc+1）/（ARR+1）	ck_psc：单片机频率STM32一般为72M
	//PWM占空比：Duty=ccr/（ARR+1）
	//PWM分辨率：Reso=1/（ARR+1）
	TIM_TimeBaseInitStructure.TIM_Period = arr;  //arr
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;//psc
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;//设置时钟分割:TDTS = Tck_tim 
	//这句话是设置定时器时钟(CK_INT)频率与数字滤波器(ETR，TIx)使用的采样频率之间的分频比例的（与输入捕获相关），
	//0表示滤波器的频率和定时器的频率是一样的
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM??????
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);

	//TIM1-CH1参数配置
	//！！高级定时器的结构体必须全部配置，否则输出不了PWM波！！！！

	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//设置PWM输出模式，这里设置为PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //使能输出比较状态   //使能输出到端口
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;//使能输出比较N状态
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//当 MOE=0 重置 TIM1 输出比较空闲状态
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;//当 MOE=0 重置 TIM1 输出比较 N 空闲状态
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;//TIM1 输出比较N极性高
	
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);

	//TIM1-CH4参数配置

	TIM_OC4Init(TIM1,&TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_Cmd(TIM1,ENABLE);//使能或者失能TIMx外设
	TIM_ARRPreloadConfig(TIM1, ENABLE); 
	TIM_CtrlPWMOutputs(TIM1, ENABLE); //
}


void PWM_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;				 //LED0-->PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
//	GPIO_ResetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;				 //LED0-->PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
//	GPIO_ResetBits(GPIOB,GPIO_Pin_14|GPIO_Pin_15);	
}	

//前进
void moter_run(u16 pwm1,u16 pwm2)
{
	IN1 = 0; IN2 = 1;
	IN3 = 1; IN4 = 0;
	TIM_SetCompare1(TIM1,pwm1);	//	电机1占空比  
	TIM_SetCompare4(TIM1,pwm2);	//	电机2占空
}


//暂停
void moter_stop(void)
{
	IN1 = 0; IN2 = 0;
	IN3 = 0; IN4 = 0;
	TIM_SetCompare1(TIM1,0);	//	电机1占空比  
	TIM_SetCompare4(TIM1,0);	//	电机2占空比 
}

 

//后退
void moter_back(u16 pwm1, u16 pwm2)
{
	IN1 = 1; IN2 = 0;
	IN3 = 0; IN4 = 1;
	TIM_SetCompare1(TIM1,pwm1);	//	电机1占空比  
	TIM_SetCompare4(TIM1,pwm2);	//	电机2占空比 
}
//左转
void moter_right(u16 pwm1, u16 pwm2)
{
	IN1 = 0; IN2 = 1;
	IN3 = 0; IN4 = 1;
	TIM_SetCompare1(TIM1,pwm1);	//	电机1占空比  
	TIM_SetCompare4(TIM1,pwm2);	//	电机2占空比 
}

//右转
void moter_left(u16 pwm1,u16 pwm2)
{
	IN1 = 1; IN2 = 0;
	IN3 = 1; IN4 = 0;
	TIM_SetCompare1(TIM1,pwm1);	//	电机1占空比  
	TIM_SetCompare4(TIM1,pwm2);	//	电机2占空比 

}
void moter_spin(u16 pwm1,u16 pwm2)
{
	IN1 = 1; IN2 = 0;
	IN3 = 1; IN4 = 0;
	TIM_SetCompare1(TIM1,pwm1);	//	电机1占空比  
	TIM_SetCompare4(TIM1,pwm2);	//	电机2占空比 

}

void PWM_INIT(void)
{	
//		PWM_GPIO_Config();		/*IO的初始化*/
//		/*最大255，控制PWM的输出，第一个是马达A，ENA，第二个是马达B,ENB*/
//	  /*更改PWM，以后再调用这一条就行了，就能改变速度了*/
//		TIM1_PWM_Init(899,719);

}
