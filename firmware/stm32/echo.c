#include "echo.h"
#include "delay.h"
#include "usart.h"
#include "bluetooth.h"

extern float juli;

//定时器4通道1输入捕获配置
TIM_ICInitTypeDef  TIM4_ICInitStructure;
void TIM4_Cap_Init(u16 arr,u16 psc)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能TIM4时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOB时钟
	//定时器TIM4CH1对应的i/o接口就是PB6
	//负责计时的也只有PB6，PB5是提供发送信号的i/o口而已，所以PB5可以随意换成其他引脚
	//若要把PB6换成其他引脚，则定时器也要改，改成其他引脚对应的定时器及通道。
	//比如说换成PD12，PD12的重定义功能也一个定时器，定时器也是TIM4CH1，不过是全映射的引脚
	//所以如果要用它的话，还要把定时器TIM4CH1的功能重定义到PD12上才行
	//而如果换成PB7的话，因为PB7都复用功能有TIM4CH2，所以只需要把设置TIM4CH1的设置
	//改为TIM4CH2就行。
	//ECOH回响信号,接受超声波	PB6
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;  //PB6 清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PB6 输入  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);						 //PB6 下拉
	//TRIG触发信号，发出超声波PB.7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 //LED0-->PB.7 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.7
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);						 //PB7 下拉	
	//初始化定时器4 TIM4	 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位 
	//初始化TIM4输入捕获参数，选用通道1
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
  TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM4_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC1,ENABLE);//允许更新中断 ,允许CC1IE捕获中断		
   TIM_Cmd(TIM4,ENABLE ); 	//使能定时器4   
}

u8  TIM4CH1_CAPTURE_STA=0;	//输入捕获状态		    				
u16	TIM4CH1_CAPTURE_VAL;	//输入捕获值
 
//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{ 

 	if((TIM4CH1_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{	  
		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
		 
		{	    
			if(TIM4CH1_CAPTURE_STA&0X40)//已经捕获到高电平了
			{
				if((TIM4CH1_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
				{
					TIM4CH1_CAPTURE_STA|=0X80;//标记成功捕获了一次
					TIM4CH1_CAPTURE_VAL=0XFFFF;
				}else TIM4CH1_CAPTURE_STA++;
			}	 
		}
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
		{	
			if(TIM4CH1_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
			{	  			
				TIM4CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一次上升沿
				TIM4CH1_CAPTURE_VAL=TIM_GetCapture1(TIM4);
		   		TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
			}else  								//还未开始,第一次捕获上升沿
			{
				TIM4CH1_CAPTURE_STA=0;			//清空
				TIM4CH1_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM4,0);
				TIM4CH1_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
		   		TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
			}		    
		}			     	    					   
 	}
 
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC1|TIM_IT_Update); //清除中断标志位
 
}

//TIM2初始化
void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM2, ENABLE);  //使能TIMx					 
}
	//u8 i=0;
//定时器2中断服务程序//在主函数中写TIM2_Int_Init(5000,7200); -->时以下中断每0.5s定时中断一次
//一次来定时发送超声波一次
/*发信号产生一个10us的TTL高电平后，模块内部循环发出8个40KHz的脉冲，并检测回波。一旦检测到有回
波信号则输出回响信号，回响信号的脉冲宽度与所测的距离成正比。由此通过发射信号到收到的回响信号
时间间隔可以计算得到距离*/
void TIM2_IRQHandler(void)   //TIM2中断
{

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
		{
		
			
			GPIO_SetBits(GPIOB,GPIO_Pin_7);						 //PB7 上拉
			delay_us(10);
			GPIO_ResetBits(GPIOB,GPIO_Pin_7);						 //PB7 下拉
			delay_us(5);
		   TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志 
		}
}
