/*	ͷ�ļ�������*/
#include <stc15f2k60s2.h>
#include <key.h>
#include <seg.h>
#include <su_common_typedef.h>
#include <onewire.h>
#include <ds1302.h>
#include <led.h>
/* 	����������*/
uint16_t Seg_Mode;//0�¶���ʾ 1ʱ����ʾ 2 ��������
uint8_t Key_Slow_Down;
uint8_t Key_Old,Key_Val,Key_Down,Key_Up;
uint8_t Seg_Pos;
uint16_t Seg_Slow_Down;
uint8_t Seg_Buf[8] = {16,16,16,16,16,16,16,16};
uint8_t Seg_Point[8] = {0,0,0,0,0,0,0,0};
uint8_t ucRtc[3] ={23,25,0};
float Tp;
uint8_t Tp_Canshu = 23;
bit Work_Mode = 0;//0�¶ȿ��� 1ʱ�����
bit Time_Disp = 0;
uint16_t Time_5s;
bit a = 0;


/* ��������*/
void Key_Process(){
		if(Key_Slow_Down) return;
	Key_Slow_Down =1;
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val ^ Key_Old);
	Key_Up = ~Key_Val & (Key_Val ^ Key_Old);
	Key_Old = Key_Val;
	switch(Key_Down){
		case 9://S12
		Seg_Mode++;
		if(Seg_Mode == 3) Seg_Mode = 0;		
		break;
		
		case 10: //S13
		Work_Mode =~Work_Mode;
		
		break;
		
		case 13://S16 ��
		
		if(Seg_Mode == 2&&(Tp_Canshu>=10&&Tp_Canshu<=98))
		++Tp_Canshu;
		break;
		case 14://S17 ��
		if(Seg_Mode == 2&&(Tp_Canshu>=11&&Tp_Canshu<=99)){
		--Tp_Canshu;
		return;}
		if(Seg_Mode == 1)
		Time_Disp = 1;//����
		break;
	}
	if(Key_Up == 14) Time_Disp = 0;
}

/*��Ϣ����*/
void Seg_Process(){
	Read_Rtc(ucRtc);
if(Seg_Slow_Down) return;
	switch(Seg_Mode){
		case 0://�¶���ʾ
			Tp = rd_temperature();
		Seg_Point[6] = 1;
		Seg_Buf[0] = 18;//U
		Seg_Buf[1] = 1;//1
		Seg_Buf[2] = 16;
		Seg_Buf[3] = 16;
		Seg_Buf[4] = 16;
		Seg_Buf[5] = (uint8_t)Tp/10%10;
		Seg_Buf[6] = (uint8_t)Tp%10;
		Seg_Buf[7] = (uint16_t)(Tp*10)%10;
		
		break;
		
		case 1://ʱ����ʾ
		Seg_Point[6] = 0;
		Seg_Buf[0] = 18;//U
		Seg_Buf[1] = 2;//2
		Seg_Buf[2] = 16;
		if(Time_Disp == 0){//������ʱ ʱ��
		Seg_Buf[3] = ucRtc[0]/10%10;//hour
		Seg_Buf[4] = ucRtc[0]%10;
		Seg_Buf[5] = 17;
		Seg_Buf[6] = ucRtc[1]/10%10;
		Seg_Buf[7] = ucRtc[1]%10;
		}
		else{//����
		Seg_Buf[3] = ucRtc[1]/10%10;//min
		Seg_Buf[4] = ucRtc[1]%10;
		Seg_Buf[5] = 17;
		Seg_Buf[6] = ucRtc[2]/10%10;
		Seg_Buf[7] = ucRtc[2]%10;
		}
		break;
		
		
		case 2://��������
		Seg_Point[6] = 0;
		Seg_Buf[0] = 18;
		Seg_Buf[1] = 3;	
		Seg_Buf[2] = 16;
		Seg_Buf[3] = 16;
		Seg_Buf[4] = 16;
		Seg_Buf[5] = 16;
		Seg_Buf[6] = Tp_Canshu/10%10;
		Seg_Buf[7] = Tp_Canshu%10;		
		break;
		
		
	}
}
/* ������ʾ����*/
void Led_Process(){
	if((ucRtc[1] ==0)&& (ucRtc[2] == 0 )){//����
	Led_Disp(0,1);//LED1
	if(Time_5s==0) 	Led_Disp(0,0);
	}
	if(Work_Mode ==0) 	Led_Disp(1,1);
	else Led_Disp(1,0);
}
/*�̵���������*/
void Relay_Process(){
	if(Work_Mode){//�¶ȿ���
	if(Tp>=Tp_Canshu) Relay(1);
		else Relay(0);
	
	}
	else{//ʱ�����
	if((ucRtc[1] ==0)&& (ucRtc[2] == 0 )){
	Relay(1);
	a= 1;
	if(Time_5s == 0) 	Relay(0);
	}
	}
}
/*��ʱ��0��ʼ��*/
void Init_Time0(){
		TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 64536%256;		//���ö�ʱ��ֵ
	TH0 = 64536/256;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	EA = 1;
	ET0 = 1;
}
/*�ر�BEEP*/
void Init(){
	P0 &=  0xbf;
	P2 = P2 &0x1f | 0xb0;
	P2 &=  0x1f;
	
	P0 |= 0xff;
	P2 = P2 &0x1f | 0x80;
	P2 &=  0x1f;
	


}
/*��ʱ��0������*/
void Time0_Serve() interrupt 1{
	TL0 = 64536%256;		//���ö�ʱ��ֵ
	TH0 = 64536/256;		//���ö�ʱ��ֵ
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Pos == 8) Seg_Pos = 0;
	if(++Seg_Slow_Down == 200) Seg_Slow_Down = 0;
	if(a ==1){
		if(++Time_5s == 5000){
				Time_5s = 0;
				a = 0;
		}
	}
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	
	
}
/*������*/
void main(){
	Init();
	Init_Time0();
	Set_Rtc(ucRtc);
		while(1){
		Key_Process();
		Seg_Process();
		Led_Process();
		Relay_Process();
		}
}