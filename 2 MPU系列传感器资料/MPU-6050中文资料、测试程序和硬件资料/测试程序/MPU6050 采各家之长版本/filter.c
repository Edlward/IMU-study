#include "filter.h"	

/**************************************************************************
�������ܣ����׿������˲�
��ڲ��������ٶȡ����ٶȺ��ϴμ�����ĽǶ� ���ż�������angleAnddot��angleAnddot[0]Ϊ���μ���õ��ĽǶȣ�angleAnddot[1]Ϊ���μ���õ��Ľ��ٶ�
����  ֵ����
**************************************************************************/
void Kalman_Filter(float Accel,float Gyro, float angle, float angleAnddot[2])		
{
	float Q_angle=0.001f;// ����������Э����
	float Q_gyro=0.003f;//0.03 ����������Э���� ����������Э����Ϊһ��һ�����о���
	float R_angle=0.5f;// ����������Э���� �Ȳ���ƫ��
	float dt=0.005f;//                 
	float  C_0 = 1.0f;
	float Q_bias, Angle_err;
	float PCt_0, PCt_1, E;
	float K_0, K_1, t_0, t_1;
	float Pdot[4] ={0.0f,0.0f,0.0f,0.0f};
	float PP[2][2] = { { 1.0f, 0.0f },{ 0.0f, 1.0f } };
	
	angle+=(Gyro - Q_bias) * dt; //�������
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-����������Э�����΢��

	Pdot[1]=-PP[1][1];
	Pdot[2]=-PP[1][1];
	Pdot[3]=Q_gyro;
	PP[0][0] += Pdot[0] * dt;   // Pk-����������Э����΢�ֵĻ���
	PP[0][1] += Pdot[1] * dt;   // =����������Э����
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
	
	Angle_err = Accel - angle;	//zk-�������
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //����������Э����
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	angleAnddot[0]	+= K_0 * Angle_err;	 //�������
	Q_bias	+= K_1 * Angle_err;	 //�������
	angleAnddot[1]   = Gyro - Q_bias;	 //���ֵ(�������)��΢��=���ٶ�
	
}

/**************************************************************************
�������ܣ�һ�׻����˲�
��ڲ��������ٶȡ����ٶȺ��ϴμ�����ĽǶ�
����  ֵ�����μ���ó��ĽǶ�
**************************************************************************/
float first_order_filter_for_mpu(float* angle_m, float* gyro_m, float* angle)
{
	float K1 = 0.02; 
	*angle = K1 * (*angle_m) + (1.0f - K1) * ((*angle) + (*gyro_m) * 0.005f);
	return *angle;
}

/**************************************************************************
�������ܣ������ں�-�廪�˲�����
��ڲ��������ٶȡ����ٶȺ��ϴμ�����ĽǶ�
����  ֵ�����μ���ó��ĽǶ�
**************************************************************************/
float QingHua_AngleCal(float* angle_m, float* gyro_m, float* angle)
{
	float Gt = 1.0f,Td = 10.0f; //TODO:�ϸ���˵��TdӦ�õ������α�����������֮���ʱ���������gyro_m�ĵ�λΪ��/ms����Td�ĵ�λҲ��ms
	*angle += (((*gyro_m) + ((*angle_m) - (*angle)) / Gt) * Td);
	return *angle;
}

