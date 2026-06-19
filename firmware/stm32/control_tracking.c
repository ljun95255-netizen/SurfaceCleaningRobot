#include "control.h"	
#include "motor.h"
#include "openmv.h"

CAR_STATUS_e car_mode=stop;  //状态     extern volatile
int16 x_error=0;
int16 last_x_error=0; 
int16 ser_duty=0;
uint8 out_edge=0;//出界
float C_P=4; //cameraP
float C_D=0; //cameraD
moty_duty run_duty={0};//两轮ftm输出速度

void Car_mode_control(void)
{
		if(OpenMV_RX>200 && OpenMV_RX!=0) //左出界  如果接收到的X轴的坐标超过 130  表示左边出界了
		{
		out_edge=Right;//1右

		}
		else if(OpenMV_RX<115 && OpenMV_RX!=0) //右出界  如果接收到的X轴的坐标小于 30  表示右边出界
		{
			LED=0;
			out_edge=Left;//0左  左转
		}
		else if(OpenMV_RX>115 && OpenMV_RX<200 ) //垃圾没有出界，在画面中间 
		{
			LED=1;
			out_edge=run;//run=2   
		}
		else //没有找到目标垃圾
		{
			LED=1;
			out_edge=spin;//spin=5   机器人自转寻找
		}		
		//车位状态判断
		if(out_edge==Left && OpenMV_L>20)
		{
			
				moter_left(60,100);			
		}
		else if(out_edge==Right && OpenMV_L>20)  //垃圾到机器人距离大于10cm
		{
			
				moter_right(100,60);
		}
		else if(out_edge==run&& OpenMV_L>20)  //垃圾到机器人距离大于10cm
		{
			
				moter_run(100,100);	
		}
		else if(out_edge==spin && OpenMV_L>20)  //垃圾到机器人距离大于10cm
		{
			
				moter_spin(100,100);	
		}		
		else if(OpenMV_L<=20 && OpenMV_L>=10)  //垃圾距离机器人小于10cm，大于5cm  在5到10 之间 小车停止
		{	
			
				moter_stop();			
		}
		

		
}

void move_updata(void)
{
			moter_run(300,300);
			moter_stop();
			moter_back(300, 300);
			moter_left(300, 0);
			moter_right(0,300);				
}

