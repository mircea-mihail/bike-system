/*******************************************************************************
*  Copyright (C) 2010 - All Rights Reserved		
* ��������:	��ɼ���
* ��Ȩ����: ��ɵ���	
* ��������:	2010��6��10�� 
* ������ʷ:	2011��2��10���޸�
            2012��9��20�����޸�
* Version:  3.5 
* Demo �Ա���ַ��http://qifeidz.taobao.com/
**********************************************************************************************************************************************
����������������������������������������������������������������������������������������������������������������������������������������������
����������������������һ������������������������������������������������������������һ����������������������������һһһһһһһ��������������
������һһһһ��һһһһһһ������������һһһһһһһһ�������������������ą�����һһ������һ������������������һһһһ����һһһ������������
������������һ������һ��һ������������һһһһ�ą���һ����һһ������������һһһһһһһһһһһ������������������������һһһһ��������������
�������ą���һ��������������������������������������һ��һһ��������������һһ����һһ������һһ������������������������һһ������������������
������һ����һһһһһһһһһһһ������������������һһ������������������һһһһһһһһһһ������������һһһһһһһһһһһһһһһ������
������һһһһһ����һһ��һ����������������������һһһһһ��������������һһ����һһ������һ������������һһ����������һһ������һһ��������
��������������һ��һ��һ��һ������������������������һ����һһ������������һһһһһһһһһһ��������������������������һһ������������������
����һһһ��������һһһ��һ������������������������һһ����������������������������һ��������������һ������������������һһ������������������
������������һ����������һһ��������������������������һһһһһһ������������������һһһһһһһһһ������������������һһ������������������
��������һһһ������һһһһ����������������������������һһһһһ��������������������һһһһһһһ��������������һһһһ��������������������
����������������������������������������������������������������������������������������������������������������������������������������������
**********************************************************************************************************************************************/


#ifndef TFT_H
#define TFT_H

///////////////////////////////////////////////////////
//TFT colors
#define	BLACK	0x0000
#define	BLUE	0x001F
#define	RED 	0xF800
#define	GREEN 	0x07E0
#define CYAN	0x07FF
#define MAGENTA 0xF81F
#define YELLOW	0xFFE0
#define WHITE	0xFFFF		

#define AM         011



//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Tft_WR_DATA16(unsigned int inputdata);
void TftWrite16(unsigned int index);
void TftWrite(unsigned int index,unsigned int inputdata);
void Tft_Init(void);
void TftClear(unsigned int Color);
void TftWR_Str(unsigned int x,unsigned int y,unsigned char *s,unsigned int color,unsigned int xcolor);
void TftWR_SingleStr(unsigned char inputdata,unsigned int color,unsigned int xcolor);
void Tft_SetBackground(unsigned int StartX,unsigned int StartY,unsigned int LongX,unsigned int LongY);
void Tft_SetCursor(unsigned int Xpos, unsigned int Ypos);
//-----------------------------------------------------------------------------
#endif                             

