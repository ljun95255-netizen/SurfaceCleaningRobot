#include "openmv.h"
int openmv[8];            //stm32接收数据数组
int16_t OpenMV_RX;          /*OPENMV X 轴反馈坐标*/
int16_t OpenMV_RY;          /*OPENMV X 轴反馈坐标*/
int8_t OpenMV_L;
int8_t OpenMV_GY;
int8_t OpenMV_add;


int i=0;
//起始帧开始，发送数据 data （b3 b3 02 03 04 05 06  5b ） 八个字节 0-7
void Openmv_Receive_Data(int16_t data)    //接收Openmv传过来的数据
{
	static u8 state = 0;  //定义一个局部变量起始为0，用来表示接收的第几个数据
	if(state==0&&data==0xb3)//当接收第0个数据的时候，如果数据为0xb3
	{
		
		state=1;//state自加1
		openmv[0]=data;//再把data数据赋值给openmv[0]数组第0个
	}
	else if(state==1&&data==0xb3)//当接收第1个数据的时候，如果数据为0xb3
	{
		state=2;//state自加1
		openmv[1]=data;//再把data数据赋值给openmv[1]数组第1个
	}
	else if(state==2)
	{
		state=3;
		openmv[2]=data;
	}
	else if(state==3)
	{
		state = 4;
		openmv[3]=data;
	}
	else if(state==4)
	{
		state = 5;
	
		openmv[4]=data;
	}
		else if(state==5)
	{
		state = 6;
		openmv[5]=data;
	}
		else if(state==6)
	{
		state = 7;
		openmv[6]=data;
	}
	else if(state==7)		//检测是否接受到结束标志，也就是接收的第8个数据
	{
        if(data == 0x5B) //如果接收到的数据是0x5b结束标志的  （5b作为发送数据的结束帧）
        {
            state = 0;//对定义的state接收标志进行清零 等待下一次数据来进行接收
            openmv[7]=data;//把0x5b的数据赋值给openmv数组第7位
            Openmv_Data();//对openmv数据处理进行初始化 数据判断 然后将对应的数据取出来
			data=0;//再把接收到的数据进行清0
        }
        else if(data != 0x5B)//如果接收到的数据不是0x5B
        {
            state = 0;//数据接收标志位清0
            for(i=0;i<8;i++)
            {
                openmv[i]=0x00;//数组清0，重新接收
            }           
        }
	}
    
	else			//如果接收的数据错误
		{
			state = 0; //数据接收标志位清0重新接收
            data=0;//数据清0
			for(i=0;i<8;i++)
            { 
                openmv[i]=0x00;//数组清0，重新接收
            }
		}
		
}

void Openmv_Data(void)
{
	
		
    OpenMV_RX=openmv[2]; //数据处理后的数组第2位赋值给 OpenMV_RX
    OpenMV_RY=openmv[3];//数据处理后的数组第3位赋值给 OpenMV_RY
	OpenMV_L=openmv[4];
    OpenMV_GY=openmv[5];
	OpenMV_add=openmv[6];

} 
