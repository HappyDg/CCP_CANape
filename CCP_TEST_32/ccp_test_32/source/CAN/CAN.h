/*=======================================================================
**     KPV13 �����ⶨ���ļ�
**     �ļ���    : CAN.h
**     ������    : T5644ATest
**     ϵͳƽ̨  : KPV13
**     ����ƽ̨  : CodeWarrior IDE version 5.9.0
**     ��������  : 2012-9-24 18:07:46
**     ���      : CANģ�����
**     ��Ȩ      : (c) Copyright KeyPower, 2012
**     ������    : ���� 
**     ��ϵ��ʽ  : peng.zhang@key-power.com.cn
**     ��ʷ�汾  :
**                 	V0.1 2012-9-24  ʵ�ֻ������� 
**					V0.2 2012-9-25	������������,�޸�һЩBUG
**					V0.3 2012-11-26 ����CAN ͨ���ͽӲ�����岻һ������
**					V0.4 2013-01-25 ��ӳ�ʼ������1.42us��ʱ
=======================================================================*/
#ifndef _KPV_CAN_H
#define _KPV_CAN_H

#define ERROR_OK					((UINT_8)0)
#define ERROR_MBINDEX_RANGE			((UINT_8)1)
#define ERROR_CHANNEL_RANGE			((UINT_8)2)
#define ERROR_MBBUF_EMPTY			((UINT_8)3)
#define ERROR_MB_OVERRUN			((UINT_8)4)
#define ERROR_MB_BUSY				((UINT_8)5)
#define ERROR_SFBUF_EMPTY			((UINT_8)6)
#define ERROR_SFTBUF_FULL			((UINT_8)7)
#define ERROR_TIMESOUT				((UINT_8)8)

/*
** ===================================================================
**	����:InitCAN
**	����:	UINT_8 channel		ͨ��
**									0  ͨ��0 �Ӳ��{CANL0,CANH0}
**									1  ͨ��1 �Ӳ��{CANL1,CANH1}
**									2  ͨ��2 �Ӳ��{CANL2,CANH2}
**			UINT_32 baudrate	ͨ��������
**									250000	250kbps
**									500000	500kbps
**									1000000	1Mbps
**			UINT_32 mask1		����MASK1
**			UINT_32 acccode1	ACCODE1
**			UINT_32 mask2		����MASK2
**			UINT_32 acccode2	ACCODE2
**  ����:  ����CANͨ�������ʲ�ʹ��CANͨ��
**  ����ֵ: 
**			ERROR_OK		���óɹ�
**			ERROR_TIMESOUT	��ʼ����ʱ
**			�����
**	ע��:
** ===================================================================
*/
extern UINT_8 InitCAN(UINT_8 channel,UINT_32 baudrate,UINT_32 mask1,\
					UINT_32 acccode1,UINT_32 mask2,UINT_32 acccode2);

/*
** ===================================================================
**	����:SendCANMsg
**	����:	UINT_8 channel		ͨ��
**									0  ͨ��0 �Ӳ��{CANL0,CANH0}
**									1  ͨ��1 �Ӳ��{CANL1,CANH1}
**									2  ͨ��2 �Ӳ��{CANL2,CANH2}
**			UINT_8 ext			�Ƿ�����չ֡
**									0	��׼֡
**									1	��չ֡
**			UINT_8 dataframe	�Ƿ�������֡
**									0	Զ��֡
**									1	����֡
**			UINT_32 canid		CAN ID
**			UINT_8 datalen		�����ֽ��� 0-8
**			UINT_8 *data		�洢�����������ʼ��ַָ��
**										
**  ����:  ����CAN�������������
**  ����ֵ: 
**			ERROR_OK			�������ݳɹ�
**			ERROR_SFTBUF_FULL	�����������		
**			...
**	ע��:	
** ===================================================================
*/


UINT_8 SendCANMsg(UINT_8 channel,UINT_8 ext,UINT_8 dataframe,\
					UINT_32 canid,UINT_8 datalen,UINT_8 *data);
					
/*
** ===================================================================
**	����:GetCANMsg
**	����:	UINT_8 channel		ͨ��
**									0  ͨ��0 �Ӳ��{CANL0,CANH0}
**									1  ͨ��1 �Ӳ��{CANL1,CANH1}
**									2  ͨ��2 �Ӳ��{CANL2,CANH2}
**			UINT_8 *ext			�Ƿ�����չ֡
**									0	��׼֡
**									1	��չ֡
**			UINT_8 *dataframe	�Ƿ�������֡
**									0	Զ��֡
**									1	����֡
**			UINT_32 *canid		CAN ID
**			UINT_8 *datalen		�����ֽ��� 0-8
**			UINT_8 *data		�洢�����������ʼ��ַָ��
**										
**  ����:  ��ȡCAN����������
**  ����ֵ: 
**			ERROR_OK			��ȡ���ݳɹ�
**			ERROR_SFBUF_EMPTY	�����������		
**			...
**	ע��:	
** ===================================================================
*/

UINT_8 GetCANMsg(UINT_8 channel,UINT_8 *ext,UINT_8 *dataframe,\
					UINT_32 *canid,UINT_8 *datalen,UINT_8 *data);
typedef void (*CANRCBTYPE)(void);

extern  int CanTransmitPossible(void);

#endif