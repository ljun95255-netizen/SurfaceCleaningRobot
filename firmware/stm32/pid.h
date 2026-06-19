
typedef struct 
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
}PID;         
float pid(int hz,PID * PIDx,float Input,float point,float xiaxian,float shangxian);
float alt_pid(float input,float point,float az_data);///高度pid
extern PID 
		 PID_yaw_rate, //3
		 PID_yaw_angle,//6
		 PID_roll_rata,//9
		 PID_roll_angle,//12
		 PID_pitch_rata,//15
		 PID_pitch_angle,//18
		 PID_alttle_rata,//21
		 PID_alttle_speed//24
		 ;
