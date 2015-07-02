/*=======================================================================
**     KPB03 �����ⶨ���ļ�
**     �ļ���    : SPI.h
**     ������    : T5644ATest
**     ϵͳƽ̨  : BMS32
**     ����ƽ̨  : CodeWarrior IDE version 2.8
**     ��������  : 2012-9-28 17:02:25
**     ���      : CANģ�����
**     ��Ȩ      : (c) Copyright KeyPower, 2012
**     ������    : ���� 
**     ��ϵ��ʽ  : peng.zhang@key-power.com.cn
**     ��ʷ�汾  :
**                 	V0.1 2012-9-28  ʵ�ֻ������� 
=======================================================================*/
#ifndef _BMS_SPI_H
#define _BMS_SPI_H
extern void InitDSPIB();
extern void DSPIB_Send(UINT_16*senddata,UINT_16*recvdata,UINT_16 datanum);
extern void DSPIB_Send_Ch(UINT_16*senddata,UINT_16*recvdata,UINT_16 datanum,UINT_8 cs);
extern void InitUJA1075A();
extern void GetUJA1075A();
extern void SleepUJA1075A(UINT_8 wake_cfg_can,UINT_8 wake_cfg_cc,UINT_8 wake_cfg_key);
extern UINT_8 SetAO(UINT_8 channel,UINT_16 vol);
#endif
