#include "PID.h"
#include "usart.h"
 PID PID_yaw_rata, //3
		 PID_yaw_angle,//6
		 PID_roll_rata,//9
		 PID_roll_angle,//12
		 PID_pitch_rata,//15
		 PID_pitch_angle,//18
		 PID_alttle_rata,//21
		 PID_alttle_speed//24
		 ;
	/*
struct PID1
{
	float P;
	float I; 
	float D;
	float setpoint;//目标点
	float sum_error;//总误差
	float last_error;//上一次误差
	float prev_error;//前一次误差
	float position;//输出值
	int T;
}; */
float pid(int hz,PID * PIDx,float Input,float point,float xiaxian,float shangxian)//普通pid
	
{
  
	PIDx->setpoint=Input;
  PIDx->last_error=PIDx->setpoint-point;
	PIDx->sum_error+=PIDx->last_error;
	PIDx->position=PIDx->P*PIDx->last_error+PIDx->I*PIDx->sum_error+PIDx->D*(PIDx->last_error-PIDx->prev_error);
	PIDx->prev_error=PIDx->last_error;
	PIDx->T++;//频率
	if(PIDx->position<xiaxian)//超过下限不积分且输出值为下限值
	{
		PIDx->position=xiaxian;
	PIDx->sum_error-=PIDx->last_error;
	}
	if(PIDx->position>shangxian)//超过上限不积分且输出值为上限值
	{
		PIDx->position=shangxian;
	PIDx->sum_error-=PIDx->last_error;
	}
	if(hz>=0)
	if(PIDx->T>=hz)//防止积分数据溢出，以hz频率为周期清除
	{
		PIDx->T=0;
		PIDx->sum_error=0;
	}
	
	return PIDx->position;
}
