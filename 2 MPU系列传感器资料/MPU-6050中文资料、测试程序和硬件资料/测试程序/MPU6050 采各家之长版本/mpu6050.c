#include "mpu6050.h"
#include "filter.h"

/**
  * ��������: д���ݵ�MPU6050�Ĵ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */ 
void MPU6050_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
	SimuI2C_Start(&SimuI2C_Handle);
	
	SimuI2C_WriteByte(&SimuI2C_Handle,MPU6050_SLAVE_ADDRESS);
	SimuI2C_WaitAck(&SimuI2C_Handle);
	
	SimuI2C_WriteByte(&SimuI2C_Handle,reg_add);
	SimuI2C_WaitAck(&SimuI2C_Handle);
	
	SimuI2C_WriteByte(&SimuI2C_Handle,reg_dat);
	SimuI2C_WaitAck(&SimuI2C_Handle);
	
	SimuI2C_Stop(&SimuI2C_Handle);
}

/**
  * ��������: ��MPU6050�Ĵ�����ȡ����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */ 
void MPU6050_ReadData(uint8_t reg_add,unsigned char *Read,uint8_t num)
{
	unsigned char i;
	
	SimuI2C_Start(&SimuI2C_Handle);
	
	SimuI2C_WriteByte(&SimuI2C_Handle,MPU6050_SLAVE_ADDRESS);
	SimuI2C_WaitAck(&SimuI2C_Handle);
	
	SimuI2C_WriteByte(&SimuI2C_Handle,reg_add);
	SimuI2C_WaitAck(&SimuI2C_Handle);
	
	SimuI2C_Start(&SimuI2C_Handle);
	
	SimuI2C_WriteByte(&SimuI2C_Handle,MPU6050_SLAVE_ADDRESS + 1);
	SimuI2C_WaitAck(&SimuI2C_Handle);
	
	for(i=0;i<(num-1);i++)
	{
		*Read=SimuI2C_ReadByte(&SimuI2C_Handle,1);
		Read++;
	}
	*Read=SimuI2C_ReadByte(&SimuI2C_Handle,0);
	SimuI2C_Stop(&SimuI2C_Handle);
}

unsigned char MPU6050_ID;
/**
  * ��������: ��ʼ��MPU6050оƬ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */ 
void MPU6050_Init(void)
{
	//�ڳ�ʼ��֮ǰҪ��ʱһ��ʱ�䣬��û����ʱ����ϵ�����ϵ����ݿ��ܻ����
	SimuI2C_Init(&SimuI2C_Handle);
	HAL_Delay(50);
	
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1,0X80);	//��λMPU6050
	HAL_Delay(100);
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1,0X00);	//����MPU6050 
	
	//MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG,3<<3);		//���������������̷�Χ ����2000dps
	MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);     	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)

	//MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG ,0<<3);		//���ü��ٶȴ����������̷�Χ����2g
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG , 0x00);		//���ü��ٶȴ�����������2Gģʽ�����Լ�
	
	MPU6050_WriteReg(MPU6050_RA_INT_ENABLE,0X00);			//�ر������ж�
	MPU6050_WriteReg(MPU6050_RA_USER_CTRL,0X00);			//I2C��ģʽ�ر�
	MPU6050_WriteReg(MPU6050_RA_FIFO_EN,0X00);				//�ر�FIFO
	MPU6050_WriteReg(MPU6050_RA_INT_PIN_CFG,0X80);			//INT���ŵ͵�ƽ��Ч
	
	MPU6050_ReadData(MPU6050_RA_WHO_AM_I,&MPU6050_ID,1);    //��������ַ
	if(MPU6050_ID != MPU6050_ADDRESS)
	{		
		FaultASSERT("MPU6050 dectected error!\r\n");
	}else
	{
//		MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);	   	 	//�����ǲ����ʣ�1KHz
//		MPU6050_WriteReg(MPU6050_RA_CONFIG , 0x06);	        	//��ͨ�˲��������ã���ֹƵ����1K��������5K
		
		MPU_Set_Rate(50);										//���ò�����Ϊ50Hz��LPFΪ�����ʵ�һ��
		
		MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1,0X01);			//����CLKSEL,PLL X��Ϊ�ο�
		MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_2,0X00);			//���ٶ��������Ƕ�����
	}
}

/**
  * ��������: ��ȡMPU6050�ļ��ٶ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */ 
void MPU6050ReadAcc(short *accData)
{
    uint8_t buf[6];
    MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6);
    accData[0] = ((u16)buf[0] << 8) | buf[1];
    accData[1] = ((u16)buf[2] << 8) | buf[3];
    accData[2] = ((u16)buf[4] << 8) | buf[5];
}

/**
  * ��������: ��ȡMPU6050�Ľ��ٶ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */ 
void MPU6050ReadGyro(short *gyroData)
{
    uint8_t buf[6];
    MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
    gyroData[0] = ((u16)buf[0] << 8) | buf[1];
    gyroData[1] = ((u16)buf[2] << 8) | buf[3];
    gyroData[2] = ((u16)buf[4] << 8) | buf[5];
}


//����MPU6050�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
void MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	MPU6050_WriteReg(MPU6050_RA_CONFIG,data);//�������ֵ�ͨ�˲���  
}
//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
void MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV,data);
 	MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}

//�õ��¶�ֵ
//����ֵ:�¶�ֵ
float MPU_Get_Temperature(void)
{
    u8 buf[2]; 
    short raw;
	float temp;
	MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=36.53f+((float)raw)/340.0f;  
    return temp;
}


/*һ�׵�ͨ�˲����������˲����=(1-a)*���β���ֵ+a*�ϴ��˲������aԽ���˲��̶�Խ�ߣ�a������0~1֮��*/
/*a��һ�׵�ͨ��ʱ�䳣��T��ϵ��a = T/(T+1),TԽ���˲��̶�Խ��*/
#define LPF_A (0.69f)
float __FirstOrderLPF(float NewValue)
{
	static float LastValue,output;
	output = (1.0f - LPF_A) * NewValue + LPF_A * LastValue;
	LastValue = NewValue;
	return output;
}


#define PI_F  3.14159265358979323846f /*ʹ��PIֵ*/
#include <math.h>  /*ʹ��atan2f����*/


/**
  * ��������: ����õ�������ǶȺͽ��ٶ�
  * �������: ǰ���������ӵõ��ĽǶȺͽǶ�
			  ��������������ʹ����һ�ֻ����˲��㷨��
				1��Kalman_Filter
				2��first_order_filter_for_mpu
				3��QingHua_AngleCal
  * �� �� ֵ: ��
  * ˵    ��: �����������10msѭ�����ã����ٶȵ�λΪ g�������ǵ�λΪ ��/ms
  */
void MPU6050_GetAngle(float* Angle[3],float* AngleSpeed[3],u8 which_filter)
{
	u8 i;
	
	short MPU6050_Gyro_origin[3];	//��MPU6050������������ԭʼ����
	short MPU6050_Accel_origin[3];	//��MPU6050�����ļ��ٶȼ�ԭʼ����
	
	float MPU6050_Gyro[3];			//��MPU6050������������ת��Ϊ��λg����ֵ
	float MPU6050_Accel[3];			//��MPU6050�����ļ��ٶȼ�ת��Ϊ��λ��/ms����ֵ
	
	static float MPU6050_Angle[3];			//����õ�������Ƕ�
	static float MPU6050_AngleSpeed[3];		//����õ���������ٶȣ����ڷǿ������˲���Ϊֱ�������������ݾ����˲���ֵ��
	
	/*����ԭʼ����*/
	MPU6050ReadAcc(MPU6050_Accel_origin);
	MPU6050ReadGyro(MPU6050_Gyro_origin);
	
	/*ԭʼ�����޷�*/
	for(i = 0;i < 3;i++)
	{
//		if(MPU6050_Gyro_origin[i] > )
//		{
//			
//		}
//		
//		if(MPU6050_Accel_origin[i] > )
//		{
//			
//		}
	}
	
	/*��λת�������ٶȵ�λΪ g�������ǵ�λΪ ��/ms*/
	for(i = 0;i < 3;i++)
	{
//		MPU6050_Gyro[i]  =	((float)MPU6050_Gyro_origin[i]) 	*	xx.xxf;
//		MPU6050_Accel[i] =	((float)MPU6050_Accel_origin[i])	*	xx.xxf;
	}
	
	/*�Ѽ��ٶȼƵ�ֵ(Gx/Gy/Gz (g)) תΪ��������ͬλ�õ� �Ƕ�ֵ(��x/��y/��z (��))*/
	MPU6050_Accel[0] = atan2f(MPU6050_Accel[1], MPU6050_Accel[2])*180.0f/PI_F;	//tan(��x) = tan(��yz) = Ry/Rz
	MPU6050_Accel[1] = atan2f(MPU6050_Accel[0], MPU6050_Accel[2])*180.0f/PI_F;	//tan(��y) = tan(��xz) = Rx/Rz
	MPU6050_Accel[2] = atan2f(MPU6050_Accel[0], MPU6050_Accel[1])*180.0f/PI_F;	//tan(��z) = tan(��xy) = Rx/Ry
	
	
	switch(which_filter)
	{
		case 1:
			float angleAnddot[3][2];
			for(i = 0;i < 3;i++)
			{
				Kalman_Filter(MPU6050_Accel[i], MPU6050_Gyro[i], MPU6050_Angle[i],angleAnddot[i]);
				MPU6050_Angle[i] = 		angleAnddot[i][0];
				MPU6050_AngleSpeed[i] = angleAnddot[i][1];
			}
			break;
		case 2:
			for(i = 0;i < 3;i++)
			{
				MPU6050_Angle[i] = first_order_filter_for_mpu(&MPU6050_Accel[i], &MPU6050_Gyro[i],&MPU6050_Angle[i]);
				MPU6050_AngleSpeed[i] = __FirstOrderLPF(MPU6050_GyroF[i]); //���ڷǿ������˲���Ϊֱ�������������ݾ����˲���ֵ
			}
			break;
		case 3:
			for(i = 0;i < 3;i++)
			{
				MPU6050_Angle[i] = QingHua_AngleCal(&MPU6050_Accel[i], &MPU6050_Gyro[i],&MPU6050_Angle[i]);
				MPU6050_AngleSpeed[i] = __FirstOrderLPF(MPU6050_GyroF[i]); //���ڷǿ������˲���Ϊֱ�������������ݾ����˲���ֵ
			}
			break;
		default:
			break;
	}
	
	for(i = 0;i < 3;i++)
	{
		Angle[i] 		= MPU6050_Angle[i];
		AngleSpeed[i]	= MPU6050_AngleSpeed[i];
	}
}



