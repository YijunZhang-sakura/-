#ifndef _DS1302_H
#define _DS1302_H
#include <stc15f2k60s2.h>
#include <intrins.h>
sbit SCK = P1^7;
sbit SDA = P2^3;
sbit RST = P1^3;
void Write_Ds1302(unsigned  char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte ( unsigned char address );
void Set_Rtc(unsigned char * ucRtc);
void Read_Rtc(unsigned char * ucRtc);
#endif