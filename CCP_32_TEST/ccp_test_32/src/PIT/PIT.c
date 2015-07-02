/*=======================================================================
**     KPB03 �����ⶨ���ļ�
**     �ļ���    : PIT.c
**     ������    : T5644ATest
**     ϵͳƽ̨  : BMS32
**     ����ƽ̨  : CodeWarrior IDE version 2.8
**     ��������  : 2012-9-27 11:02:27
**     ���      : PITģ�����
**     ��Ȩ      : (c) Copyright KeyPower, 2012
**     ������    : ���� 
**     ��ϵ��ʽ  : peng.zhang@key-power.com.cn
**     ��ʷ�汾  :
**                 	V0.1 2012-9-27  ʵ�ֻ������� 
=======================================================================*/

#include "MPC5644A.h"
#include "IntcInterrupts.h"     /* Implement functions from this file */
#include "ZTypes.h"
#include "PIT.h"
//#include "includes.h"


/*
** ===================================================================
**	��������
** ===================================================================
*/
typedef void (*ISRTYPE)(void);
/*
** ===================================================================
**	��������
** ===================================================================
*/
void PIT_ISR0(void);
void PIT_ISR1(void);
void PIT_ISR2(void);
void PIT_ISR3(void);
void PIT_ISR(UINT_8 channel);

/*
** ===================================================================
**	PITģ���ж�����������ַ��
** ===================================================================
*/
void*PIT_ISR_ARR[4]={PIT_ISR0,PIT_ISR1,PIT_ISR2,PIT_ISR3};
void*PIT_ISR_HOOK[4]={0,0,0,0};


/*
** ===================================================================
**	����:PIT_ISRX
**	����:	
**  ����:  PIT�жϷ������
**  ����ֵ: 
**			...
**	ע��:	
** ===================================================================
*/
void PIT_ISR0(void)
{
	PIT_ISR(0);
}
void PIT_ISR1(void)
{
	PIT_ISR(1);
}
void PIT_ISR2(void)
{
	PIT_ISR(2);
}
void PIT_ISR3(void)
{
	PIT_ISR(3);
}
void PIT_ISR(UINT_8 channel)
{
	if(PIT_ISR_HOOK[channel]!=0)
		(*((ISRTYPE)PIT_ISR_HOOK[channel]))();
	PIT.TIMER[channel].TFLG.B.TIF=1;	//ISR end
}
/*
** ===================================================================
**	����:InitPIT
**	����:	
**  ����:  ��ʼ��PIT�жϷ������
**  ����ֵ: 
**			...
**	ע��:	�����ڳ�ʼ����ſ�ʹ��PIT��������
** ===================================================================
*/
void InitPIT(void)
{
	UINT_8 channel=0;
	while(channel<4)
	{
		PIT.TIMER[channel].TCTRL.B.TIE = 0;
		PIT.TIMER[channel].TCTRL.B.TEN = 0;
		PIT.TIMER[channel].TFLG.B.TIF=1;	//Clear flag
		channel++;	
	}


	INTC_InstallINTCInterruptHandler(PIT_ISR_ARR[0],301,15);
	INTC_InstallINTCInterruptHandler(PIT_ISR_ARR[1],302,15);
	INTC_InstallINTCInterruptHandler(PIT_ISR_ARR[2],303,15);
	INTC_InstallINTCInterruptHandler(PIT_ISR_ARR[3],304,15);
	
}

/*
** ===================================================================
**	����:SetPITHook
**	����:	UINT_8 channel		ͨ��
**									0  PITͨ��0 
**									1  PITͨ��1 
**									2  PITͨ��2
**									3  PITͨ��3 
**			void*	func		�ص�����			
**										
**  ����:  ����PIT�ص���������������void XXXFUNC(void);
**  ����ֵ: 
**			...
**	ע��:	
** ===================================================================
*/

void SetPITHook(UINT_8 channel,void* func)
{
	PIT_ISR_HOOK[channel] = func;
}

/*
** ===================================================================
**	����:SetPIT
**	����:	UINT_8 channel		ͨ��
**									0  PITͨ��0 
**									1  PITͨ��1 
**									2  PITͨ��2
**									3  PITͨ��3 
**			UINT_32	time		PIT����ʱ�� ��λus	
**									0 �رո�ͨ��
**									1000,0000	���Ƶ�10s		
**										
**  ����:  ����PIT�ж�ʱ��
**  ����ֵ: 
**			...
**	ע��:	
** ===================================================================
*/
UINT_8 SetPIT(UINT_8 channel,UINT_32 time)
{
	PIT.TIMER[channel].TCTRL.B.TIE = 0;
	PIT.TIMER[channel].TCTRL.B.TEN = 0;
	PIT.TIMER[channel].TFLG.B.TIF=1;	//Clear flag
	
	if(time==0)return ERROR_PIT_OK;
	if(time>10000000UL)return ERROR_PIT_TIMERANGE;
	PIT.TIMER[channel].LDVAL.R = time*SYSTEM_CLOCK;
	
	PIT.TIMER[channel].TCTRL.B.TIE = 1;
	PIT.TIMER[channel].TCTRL.B.TEN = 1;
	return ERROR_PIT_OK;
}
