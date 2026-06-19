#include "control.h"
#include "extern_x.h"
#include "USART.h"
#include "getpwm.h"
#include "PID.h"
#include "def.h"
#include "PWM.h"
#include "extern_x.h"

extern  float gz_data;//来自mpu6050_i2c
extern 	float yaw_input,speed_input;//来源于getpwm，输入目标
				float yaw_output,speed_output;
				u8 status=0;//机器状态，0待机，1巡航，2追踪，3归航，4人工
				extern float speed;
				u8 fangxiang=1;//1是前进，0是后退
extern  
PID 
		 PID_yaw_rata, //3
		 PID_yaw_angle,//6
		 PID_roll_rata,//9
		 PID_roll_angle,//12
		 PID_pitch_rata,//15
		 PID_pitch_angle,//18
		 PID_alttle_rata,//21
		 PID_alttle_speed//24
		 ;
void pwm_out(float yaw_in,float speed_in)//计算pid
{
	if(fabs(yaw_input-yaw)>180)//如果差角大于180
	{
		if(yaw<=0&&yaw_in>=0)
		 	yaw_in=yaw_in-360;
		if(yaw>0&&yaw_in<0)
			yaw_in=360+yaw_in;
	}
	//比较特的别一点，角度周期性转动问题
	yaw_output=pid(50,&PID_yaw_rata,
											pid(3000,&PID_yaw_angle		,yaw_in,yaw	,-120,120),//角度
											gz_data	 ,-180,180//yaw速率
									);
	speed_output=pid(50,&PID_pitch_rata,
											pid(3000,&PID_pitch_angle		,speed_in,speed	,-120,120),//角度
											gz_data	 ,-180,180//yaw速率
									);
}
void pianhang(float sudu,float jiaodu,u8 fang)//在yaw_input 和speed_input 基础上添加控制
{
	float temp,temp2;
	int l,r;
	if(fang)
	{
		temp=jiaodu+yaw_input;
		if(temp>180)///超程
			 temp=-180+(temp-180);
		if(temp<-180)
			 temp=180+(temp+180);
		temp2=sudu+speed_input;
		pwm_out(temp,temp2);
		l=(int)(speed_output+yaw_output);//差速如果想向右转，那么左加速
		r=(int)(speed_output-yaw_output);
		if(l>=999)
			l=999;
		if(l<=0)
			l=0;
		if(r>=999)
			r=999;
		if(r<=0)
			r=0;
		control(l,r);
	}
		if(fang==0)//后退模式
	{
		temp=-jiaodu+yaw_input;
		temp2=-sudu-speed_input;
		pwm_out(temp,temp2);
		l=(int)(speed_output-yaw_output);//差速如果想向右转，那么左加速
		r=(int)(speed_output+yaw_output);
		if(l<=-999)
			l=-999;
		if(l>=0)
			l=0;
		if(r<=-999)
			r=-999;
		if(r>=0)
			r=0;
		control(l,r);
	}

	
}

