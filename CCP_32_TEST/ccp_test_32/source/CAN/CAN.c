/*=======================================================================
**     KPV13 �����ⶨ���ļ�
**     �ļ���    : CAN.c
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
**					V0.4 2012-11-28 ����CAN������չ֡ʱֻ�յ�18λ������
**					V0.5 2012-12-27 ����CANID��չ֡�ͱ�׼֡����ͬʱ��������
**					V0.6 2013-01-25 ��ӳ�ʼ������1.42us��ʱ
=======================================================================*/
#include "MPC5644A.h"
#include "IntcInterrupts.h"     /* Implement functions from this file */
#include "ZTypes.h"
#include "CAN.h"
//#include "SPI.h"
/*
** ===================================================================
**	��������
** ===================================================================
*/
void OnCANTMsg(UINT_8 channel,UINT_8 MBindex);
void OnCANRMsg(UINT_8 channel,UINT_8 MBindex);

void OnCANTMsgCAN0MB0(void);
void OnCANTMsgCAN1MB0(void);
void OnCANTMsgCAN2MB0(void);

void OnCANRMsgCAN0MB12(void);
void OnCANRMsgCAN0MB13(void);
void OnCANRMsgCAN0MB14(void);
void OnCANRMsgCAN0MB15(void);

void OnCANRMsgCAN1MB12(void);
void OnCANRMsgCAN1MB13(void);
void OnCANRMsgCAN1MB14(void);
void OnCANRMsgCAN1MB15(void);

void OnCANRMsgCAN2MB12(void);
void OnCANRMsgCAN2MB13(void);
void OnCANRMsgCAN2MB14(void);
void OnCANRMsgCAN2MB15(void);

UINT_8 SetCANFilter(UINT_8 channel,UINT_8 MBindex,UINT_8 iren,\
					UINT_32 acccode,UINT_32 mask);

UINT_8 SendCANMsgMB(UINT_8 channel,UINT_8 MBindex,UINT_8 ext,\
				UINT_8 dataframe,UINT_32 canid,UINT_8 datalen,UINT_8 *data);
UINT_8 GetCANMsgMB(UINT_8 channel,UINT_8 MBindex,UINT_8 *ext,\
				UINT_8 *dataframe,UINT_32 *canid,UINT_8 *datalen,UINT_8 *data);
/*
** ===================================================================
**	CANģ���ж�����������ַ��
** ===================================================================
*/
void*OnCANRMsgArr[3][16];
void*OnCANTMsgArr[3][16]={};
CANRCBTYPE OnCANRmsgCB[3]={0L,0L,0L};
/*
** ===================================================================
**	CANģ���ж�������--VECTORNUM
** ===================================================================
*/
UINT_16	CAN_VECTOR_SNUM[3]={155,283,176} ;

/*
** ===================================================================
**	CANģ�鷢�����������
** ===================================================================
*/
#define		CAN_TMSGBUF_NUM 							250
UINT_8 		CAN_TMSG_ext 		[3][CAN_TMSGBUF_NUM];
UINT_8 		CAN_TMSG_dataframe 	[3][CAN_TMSGBUF_NUM];
UINT_32 	CAN_TMSG_canid 		[3][CAN_TMSGBUF_NUM];
UINT_8 		CAN_TMSG_datalen 	[3][CAN_TMSGBUF_NUM];
UINT_8 		CAN_TMSG_data 		[3][CAN_TMSGBUF_NUM][8];

UINT_8 		CAN_TMSG_in	[3]	=	{0,0,0};
UINT_8 		CAN_TMSG_out[3]	=	{0,0,0};
UINT_8 		CAN_TMSG_num[3]	=	{0,0,0};

/*
** ===================================================================
**	CANģ��������������
** ===================================================================
*/
#define 	CAN_MSGBUF_NUM 								40
UINT_8 		CAN_MSG_ext 		[3][CAN_MSGBUF_NUM];
UINT_8 		CAN_MSG_dataframe 	[3][CAN_MSGBUF_NUM];
UINT_32 	CAN_MSG_canid 		[3][CAN_MSGBUF_NUM];
UINT_8 		CAN_MSG_datalen 	[3][CAN_MSGBUF_NUM];
UINT_8 		CAN_MSG_data 		[3][CAN_MSGBUF_NUM][8];

UINT_8 		CAN_MSG_in	[3]	=	{0,0,0};
UINT_8 		CAN_MSG_out	[3]	=	{0,0,0};
UINT_8 		CAN_MSG_num	[3]	=	{0,0,0};

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
**			UINT_32 mask1		����MASK1 bit31 1��ʶ������չ֡ 0��������չ֡
**			UINT_32 acccode1	ACCODE1 bit31 1Ϊ��չ֡ 0 ��׼֡
**			UINT_32 mask2		����MASK2 ͬ��
**			UINT_32 acccode2	ACCODE2
**  ����:  ����CANͨ�������ʲ�ʹ��CANͨ��
**				 ��(canid&mask)==(acccode&mask)ʱ���ո�֡
**  ����ֵ: 
**			ERROR_OK		���óɹ�
**			ERROR_TIMESOUT	��ʼ����ʱ
**			�����
**	ע��:
** ===================================================================
*/

UINT_8 InitCAN(UINT_8 channel,UINT_32 baudrate,UINT_32 mask1,\
					UINT_32 acccode1,UINT_32 mask2,UINT_32 acccode2)
{
	volatile struct FLEXCAN2_tag * CAN_X;
	VUINT_32 t_timer;
	VUINT_32 message_buf_index;
	VUINT_32 times_out=10;				//10 1.42us ����0.3us
	VUINT_32 times_out_FRZ=10000000;	//10 1.42us ���� <<0.3us
										//MB�����жϺ���
	OnCANRMsgArr[0][12] = OnCANRMsgCAN0MB12;
	OnCANRMsgArr[0][13] = OnCANRMsgCAN0MB13;
	OnCANRMsgArr[0][14] = OnCANRMsgCAN0MB14;
	OnCANRMsgArr[0][15] = OnCANRMsgCAN0MB15;
	
	OnCANRMsgArr[1][12] = OnCANRMsgCAN1MB12;
	OnCANRMsgArr[1][13] = OnCANRMsgCAN1MB13;
	OnCANRMsgArr[1][14] = OnCANRMsgCAN1MB14;
	OnCANRMsgArr[1][15] = OnCANRMsgCAN1MB15;
	
	OnCANRMsgArr[2][12] = OnCANRMsgCAN2MB12;
	OnCANRMsgArr[2][13] = OnCANRMsgCAN2MB13;
	OnCANRMsgArr[2][14] = OnCANRMsgCAN2MB14;
	OnCANRMsgArr[2][15] = OnCANRMsgCAN2MB15;
										//MB�����жϺ���
	OnCANTMsgArr[0][0] = OnCANTMsgCAN0MB0;
	OnCANTMsgArr[1][0] = OnCANTMsgCAN1MB0;
	OnCANTMsgArr[2][0] = OnCANTMsgCAN2MB0;
	
										//��װMB0�жϺ��������ͻ���
	INTC_InstallINTCInterruptHandler(OnCANTMsgArr[channel][0],CAN_VECTOR_SNUM[channel]+0,INTERRUPT_PRIORITY);
	
	INTC_InstallINTCInterruptHandler(OnCANRMsgArr[channel][12],CAN_VECTOR_SNUM[channel]+12,INTERRUPT_PRIORITY);
	INTC_InstallINTCInterruptHandler(OnCANRMsgArr[channel][13],CAN_VECTOR_SNUM[channel]+13,INTERRUPT_PRIORITY);
	INTC_InstallINTCInterruptHandler(OnCANRMsgArr[channel][14],CAN_VECTOR_SNUM[channel]+14,INTERRUPT_PRIORITY);
	INTC_InstallINTCInterruptHandler(OnCANRMsgArr[channel][15],CAN_VECTOR_SNUM[channel]+15,INTERRUPT_PRIORITY);
	
										//ȷ�������Ĵ�������ַ
	if(channel==0)
	{	
		CAN_X = &CAN_A;
	}
	else if(channel==2)
	{	
		CAN_X = &CAN_C;
	}
	else if(channel==1)
	{
		CAN_X = &CAN_B;
	}
										//����Freezeģʽ
	(*CAN_X).MCR.B.FRZ = 1;				//�������Freezeģʽ
	(*CAN_X).MCR.B.HALT = 1;			//��FRZ==1ʱ,���ø�λ����CANģ�����Freezeģʽ
	while( (*CAN_X).MCR.B.FRZACK != 1 )
	{
										//�ȴ����뵽Freezeģʽ
		if(times_out_FRZ==0)break;
		times_out_FRZ--;
	}
	if(times_out_FRZ==0)return ERROR_TIMESOUT;
	(*CAN_X).MCR.B.MBFEN= 1;            //����ÿһ��MB���е�����MASK��GlobalMask��������
	(*CAN_X).MCR.B.WRNEN= 1;            //���о�
	(*CAN_X).MCR.B.SRXDIS= 1;         	//���о�
	(*CAN_X).MCR.B.FEN= 0;              //FIFO��ʹ��
	(*CAN_X).MCR.B.AEN= 1;              //abort����ʹ��
	(*CAN_X).MCR.B.LPRIO_EN= 0;         //��ʹ��Local Priority
	
	if(baudrate==250000UL)
	{
	
		(*CAN_X).CR.B.PRESDIV = 3;		//Sclock frequency = CPI clock frequency / (PRESDIV + 1)=4Mhz
		(*CAN_X).CR.B.RJW = 0;
		(*CAN_X).CR.B.PSEG1 = 3;
		(*CAN_X).CR.B.PSEG2 = 3;
		(*CAN_X).CR.B.PROPSEG = 6	;	//Bit Rate = Sclock/(SYNC_SEG+PROP_SEG+PSEG1+2+PSEG2+1)
										//Bit Rate = 4Mhz/((1+6++3+2+3+1))=250Khz
		
	}
	else if(baudrate==500000UL)
	{
		(*CAN_X).CR.B.PRESDIV = 1;		//Sclock frequency = CPI clock frequency / (PRESDIV + 1)=8Mhz
		(*CAN_X).CR.B.RJW = 0;
		(*CAN_X).CR.B.PSEG1 = 3;
		(*CAN_X).CR.B.PSEG2 = 3;
		(*CAN_X).CR.B.PROPSEG = 6	;	//Bit Rate = Sclock/(SYNC_SEG+PROP_SEG+PSEG1+2+PSEG2+1)
										//Bit Rate = 8Mhz/((1+6++3+2+3+1))=500Khz		
	}
	else if(baudrate==1000000UL)
	{
		(*CAN_X).CR.B.PRESDIV = 0;		//Sclock frequency = CPI clock frequency / (PRESDIV + 1)=16Mhz
		(*CAN_X).CR.B.RJW = 0;
		(*CAN_X).CR.B.PSEG1 = 3;
		(*CAN_X).CR.B.PSEG2 = 3;
		(*CAN_X).CR.B.PROPSEG = 6	;	//Bit Rate = Sclock/(SYNC_SEG+PROP_SEG+PSEG1+2+PSEG2+1)
										//Bit Rate = 16Mhz/((1+6++3+2+3+1))=1000Khz				
	}
	(*CAN_X).CR.B.LPB = 0;				//Disable Loop Back
	(*CAN_X).CR.B.BOFFMSK = 0;			//Busoff�жϽ�ֹʹ��
	(*CAN_X).CR.B.ERRMSK = 0;			//Error�жϽ�ֹʹ��
	(*CAN_X).CR.B.LBUF = 1;				//��ŵ͵�MB���ȷ���
										
										//��ʼ������MB
	message_buf_index = 0;
	while(message_buf_index<16)
	{									//MB ǿ�Ƶ�INACTIVE
		(*CAN_X).BUF[message_buf_index].CS.B.CODE = 0b0000;
		message_buf_index++;
	}
										//
	
	(*CAN_X).BUF[12].ID.B.STD_ID = acccode1&0x000007FFL;
	(*CAN_X).BUF[12].CS.B.IDE = 0;
	(*CAN_X).BUF[12].CS.B.RTR = 0;
	t_timer = (*CAN_X).TIMER.R;

	
	(*CAN_X).BUF[13].ID.R = acccode1&0x1FFFFFFFL;
	(*CAN_X).BUF[13].CS.B.IDE = 1;
	(*CAN_X).BUF[13].CS.B.RTR = 0;
	t_timer = (*CAN_X).TIMER.R;
	
	(*CAN_X).BUF[14].ID.B.STD_ID = acccode2&0x000007FFL;
	(*CAN_X).BUF[14].CS.B.IDE = 0;
	(*CAN_X).BUF[14].CS.B.RTR = 0;
	t_timer = (*CAN_X).TIMER.R;

	
	(*CAN_X).BUF[15].ID.R = acccode2&0x1FFFFFFFL;
	(*CAN_X).BUF[15].CS.B.IDE = 1;
	(*CAN_X).BUF[15].CS.B.RTR = 0;
	t_timer = (*CAN_X).TIMER.R;
	
	if(mask1&0x80000000)
	{
		if(acccode1&0x80000000)
		{
			(*CAN_X).BUF[13].CS.B.CODE = 0b0100;			
		}
		else
		{
			(*CAN_X).BUF[12].CS.B.CODE = 0b0100;			
		}
	}
	else
	{
		(*CAN_X).BUF[12].CS.B.CODE = 0b0100;
		(*CAN_X).BUF[13].CS.B.CODE = 0b0100;
	}
	if(mask2&0x80000000)
	{
		if(acccode2&0x80000000)
		{
			(*CAN_X).BUF[15].CS.B.CODE = 0b0100;			
		}
		else
		{
			(*CAN_X).BUF[14].CS.B.CODE = 0b0100;			
		}
	}
	else
	{
		(*CAN_X).BUF[14].CS.B.CODE = 0b0100;
		(*CAN_X).BUF[15].CS.B.CODE = 0b0100;
	}
	
	(*CAN_X).RXIMR[12].R = (mask1&0x7FFL)<<18;
										//����MASK1
	(*CAN_X).RXIMR[13].R = mask1&0x1FFFFFFF;
	
	(*CAN_X).RXIMR[14].R = (mask2&0x7FFL)<<18;
										//����MASK2
	(*CAN_X).RXIMR[15].R = mask2&0x1FFFFFFF;		

	
										//Setup MB interrupt
	(*CAN_X).IMRL.R |= 1UL;
										//Setup MB interrupt
	(*CAN_X).IMRL.R |= 1UL<<12;
	(*CAN_X).IMRL.R |= 1UL<<13;
	(*CAN_X).IMRL.R |= 1UL<<14;
	(*CAN_X).IMRL.R |= 1UL<<15;
	
										//�˳�freezeģʽ
	(*CAN_X).MCR.B.HALT = 0;
	while((*CAN_X).MCR.B.NOTRDY==1)
	{
										//wait can channel ready
		if(times_out==0)break;
		times_out--;
	}											
										//���ùܽ���ΪCAN
	if(channel==0)
	{	
		SIU.PCR[83].R = 0x0600;         // Configure pad as FlexCAN_B_Tx
  		SIU.PCR[84].R = 0x0500;        	// Configure pad as FlexCAN_B_Rx
	}
	else if(channel==2)
	{	
		SIU.PCR[87].R = 0x0600;         // Configure pad as FlexCAN_C_Tx
  		SIU.PCR[88].R = 0x0500;        	// Configure pad as FlexCAN_C_Rx
	}
	else if(channel==1)
	{
		SIU.PCR[85].R = 0x0600;         // Configure pad as FlexCAN_A_Tx
		SIU.PCR[86].R = 0x0500;         // Configure pad as FlexCAN_A_Rx	
	}	
	if(times_out==0)return ERROR_TIMESOUT;
	return ERROR_OK;
	
}
/*
** ===================================================================
**	����:SetupCANRMSGCB
**	����:	UINT_8 channel		ͨ��
**									0  ͨ��0 �Ӳ��{CANL0,CANH0}
**									1  ͨ��1 �Ӳ��{CANL1,CANH1}
**									2  ͨ��2 �Ӳ��{CANL2,CANH2}
**			CANRCBTYPE cb		���յ�CAN��Ϣ��Ļص�����
**  ����:  ����CAN���ܵ���Ϣ��Ļص�����
**  ����ֵ: 
**			ERROR_OK		���óɹ�
**	ע��:
** ===================================================================
*/

UINT_8 SetupCANRMSGCB(UINT_8 channel,CANRCBTYPE cb)
{
	OnCANRmsgCB[channel]=cb;
	return ERROR_OK;
}
/*
** ===================================================================
**	����:SendCANMsgMB
**	����:	UINT_8 channel		ͨ��
**									0  ͨ��0 �Ӳ��{CANL0,CANH0}
**									1  ͨ��1 �Ӳ��{CANL1,CANH1}
**									2  ͨ��2 �Ӳ��{CANL2,CANH2}
**			UINT_8 MBindex		MB����ֵ
**									0	Message Buffer 0
**									...	...
**									15	Message Buffer 16
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
**  ����:  ֱ��ͨ��MB����CAN��Ϣ
**  ����ֵ: 
**			ERROR_OK			д��MB�ɹ�
**			ERROR_MBINDEX_RANGE	MB index������Χ
**			ERROR_MB_BUSY		MB���ڷ���
**			...
**	ע��:	���ú�������ERROR_MB_BUSY��,��Ҫ��ѭ�����øú�����
**			while(ERROR_OK!=SendCANMsgMB(xxxxxx));
** ===================================================================
*/

UINT_8 SendCANMsgMB(UINT_8 channel,UINT_8 MBindex,UINT_8 ext,\
				UINT_8 dataframe,UINT_32 canid,UINT_8 datalen,UINT_8 *data)
{
	volatile struct FLEXCAN2_tag * CAN_X;
	if(channel==0)
	{
		CAN_X = &CAN_A;
	}
	else if(channel==2)
	{	
		CAN_X = &CAN_C;
	}
	else if(channel==1)
	{
		CAN_X = &CAN_B;
	}
	
	if(MBindex>=16)
		return ERROR_MBINDEX_RANGE;		//MB index������Χ
										//MB �Ƿ��Ѿ�д��
	(*CAN_X).IFRL.R = 1UL<<MBindex;		//����жϱ�־
	if((*CAN_X).BUF[MBindex].CS.B.CODE==0b1100)
		return	ERROR_MB_BUSY;
										//MB ǿ�Ƶ�INACTIVE
	(*CAN_X).BUF[MBindex].CS.B.CODE = 0b1000;
										//�����Ƿ���Զ��֡
	(*CAN_X).BUF[MBindex].CS.B.RTR  =dataframe?0:1;
										//�������ݳ���
	(*CAN_X).BUF[MBindex].CS.B.LENGTH = datalen;	
										//�����Ƿ�����չ֡
	(*CAN_X).BUF[MBindex].CS.B.IDE = ext;
	if(ext==1)
	{
			(*CAN_X).BUF[MBindex].ID.R	= canid;
			(*CAN_X).BUF[MBindex].CS.B.SRR=1;
	}
	else
	{
		(*CAN_X).BUF[MBindex].ID.B.STD_ID = canid;
		(*CAN_X).BUF[MBindex].CS.B.SRR=0;
	}
	
	
	while(datalen>0)					//��������
	{
		(*CAN_X).BUF[MBindex].DATA.B[datalen-1] = data[datalen-1];
		datalen--;
	}
										//ǿ�Ʒ���
	(*CAN_X).BUF[MBindex].CS.B.CODE = 0b1100;

	return ERROR_OK;
	
}
/*
** ===================================================================
**	����:GetCANMsgMB
**	����:	UINT_8 channel		ͨ��
**									0  ͨ��0 �Ӳ��{CANL0,CANH0}
**									1  ͨ��1 �Ӳ��{CANL1,CANH1}
**									2  ͨ��2 �Ӳ��{CANL2,CANH2}
**			UINT_8 MBindex		MB����ֵ
**									0	Message Buffer 0
**									...	...
**									15	Message Buffer 16
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
**  ����:  ��ȡMB��CAN��Ϣ
**  ����ֵ: 
**			ERROR_OK			д��MB�ɹ�
**			ERROR_MBINDEX_RANGE	MB index������Χ
**			ERROR_MBBUF_EMPTY	MB ��
**			ERROR_MB_OVERRUN	MB overrun
**			ERROR_MB_BUSY		MB Busy
**			...
**	ע��:	
** ===================================================================
*/

UINT_8 GetCANMsgMB(UINT_8 channel,UINT_8 MBindex,UINT_8 *ext,\
				UINT_8 *dataframe,UINT_32 *canid,UINT_8 *datalen,UINT_8 *data)
{
	VUINT_32 mb_code;
	VUINT_32 tmplen;
	VUINT_32 timer;

	volatile struct FLEXCAN2_tag * CAN_X;
	if(channel==0)
	{
		CAN_X = &CAN_A;
	}
	else if(channel==2)
	{	
		CAN_X = &CAN_C;
	}
	else if(channel==1)
	{
		CAN_X = &CAN_B;
	}
	if(MBindex>=16)
		return ERROR_MBINDEX_RANGE;		//MB index������Χ
	if(!((*CAN_X).IFRL.R&(1UL<<MBindex)))
	{
		return ERROR_MBBUF_EMPTY;
	}									//����жϱ�־λ
	//(*CAN_X).IFRL.R|=1UL<<MBindex;
										//��ȡMB��Code
	mb_code = (*CAN_X).BUF[MBindex].CS.B.CODE;
	if(mb_code==0b0010)
	{
										//��������
		*dataframe 	= (*CAN_X).BUF[MBindex].CS.B.RTR?0:1;
		*datalen 	= (UINT_8)(*CAN_X).BUF[MBindex].CS.B.LENGTH;
		*ext		= (*CAN_X).BUF[MBindex].CS.B.IDE?1:0;
	 	if(*ext==1)
	 	{
	 		*canid = (*CAN_X).BUF[MBindex].ID.R;	
	 	}
	 	else
	 	{
	 		*canid = (*CAN_X).BUF[MBindex].ID.B.STD_ID;	
	 	}
	 	tmplen = *datalen;
	 	while(tmplen>0)
	 	{
	 		data[tmplen-1] = (*CAN_X).BUF[MBindex].DATA.B[tmplen-1];
	 		tmplen--;
	 	}
	 	timer = (*CAN_X).TIMER.R;
	 	(*CAN_X).BUF[MBindex].CS.B.CODE = 0b0100;
	 	return ERROR_OK;

	}else if(mb_code==0b0110)
	{
		
		timer = (*CAN_X).TIMER.R;		//OVERRUN
		(*CAN_X).BUF[MBindex].CS.B.CODE = 0b0100;
		return ERROR_MB_OVERRUN;
	}
	else if((mb_code&0b1001)==0b0001)
	{
		timer = (*CAN_X).TIMER.R;		//���ڽ��и��²���
		return ERROR_MB_BUSY;
	}
	return ERROR_OK;
}
/*
** ===================================================================
**	����:OnCANRMsg
**	����:	
**  ����:  CANͨ�������жϻص�
**  ����ֵ: 
**			...
**	ע��:	
** ===================================================================
*/


void OnCANRMsg(UINT_8 channel,UINT_8 MBindex)
{
	VUINT_8  sfbufindex;
	volatile struct FLEXCAN2_tag * CAN_X;
	if(channel==0)
	{
		CAN_X = &CAN_A;
	}
	else if(channel==2)
	{	
		CAN_X = &CAN_C;
	}
	else if(channel==1)
	{
		CAN_X = &CAN_B;
	}
	if((*CAN_X).IFRL.R&(1UL<<MBindex))
	{
		sfbufindex = CAN_MSG_in[channel];
		if(CAN_MSG_num[channel]<CAN_MSGBUF_NUM)
		{
			CAN_MSG_in[channel] = (CAN_MSG_in[channel]+1)%CAN_MSGBUF_NUM;
			CAN_MSG_num[channel]++;
		}
		else
		{
										//OVERRUN
		}
		GetCANMsgMB(channel,MBindex,&(CAN_MSG_ext[channel][sfbufindex]),\
						&(CAN_MSG_dataframe[channel][sfbufindex]),\
						&(CAN_MSG_canid[channel][sfbufindex]),\
						&(CAN_MSG_datalen[channel][sfbufindex]),\
						CAN_MSG_data[channel][sfbufindex]);
	}
	(*CAN_X).IFRL.R|=(1UL<<MBindex);
	if(OnCANRmsgCB[channel]!=0)
	OnCANRmsgCB[channel]();

}
/*
** ===================================================================
**	����:OnCANTMsg
**	����:	
**  ����:  CANͨ�������ж�
**  ����ֵ: 
**			...
**	ע��:	
** ===================================================================
*/
void OnCANTMsg(UINT_8 channel,UINT_8 MBindex)
{

	VUINT_8  sfbufindex=0;
	volatile struct FLEXCAN2_tag * CAN_X;

	if(channel==0)
	{
		CAN_X = &CAN_A;
	}
	else if(channel==2)
	{	
		CAN_X = &CAN_C;
	}
	else if(channel==1)
	{
		CAN_X = &CAN_B;
	}
	if(CAN_TMSG_num[channel]==0)
	{
		(*CAN_X).IFRL.R=(1UL<<MBindex);
		return ;
	}
	sfbufindex = CAN_TMSG_out[channel];
	if((*CAN_X).IFRL.R&(1UL))
	{
		if(ERROR_OK==SendCANMsgMB(channel,0,CAN_TMSG_ext[channel][sfbufindex],\
					CAN_TMSG_dataframe[channel][sfbufindex],\
					CAN_TMSG_canid[channel][sfbufindex],\
					CAN_TMSG_datalen[channel][sfbufindex],\
					CAN_TMSG_data[channel][sfbufindex]))
		{
			CAN_TMSG_out[channel] = (CAN_TMSG_out[channel]+1)%CAN_TMSGBUF_NUM;
			CAN_TMSG_num[channel]--;		
		}
	}
		
	(*CAN_X).IFRL.R=(1UL<<MBindex);

}

/*
** ===================================================================
**	����:OnCANMsgCANXMBX
**	����:	
**  ����:  CANͨ���ж�
**  ����ֵ: 
**			...
**	ע��:	
** ===================================================================
*/
void OnCANTMsgCAN0MB0(void)
{
	OnCANTMsg(0,0);	
}

void OnCANTMsgCAN1MB0(void)
{
	OnCANTMsg(1,0);	
}

void OnCANTMsgCAN2MB0(void)
{
	OnCANTMsg(2,0);	
}

void OnCANRMsgCAN0MB12(void)
{
	OnCANRMsg(0,12);	
}
void OnCANRMsgCAN0MB13(void)
{
	OnCANRMsg(0,13);	
}
void OnCANRMsgCAN0MB14(void)
{
	OnCANRMsg(0,14);	
}
void OnCANRMsgCAN0MB15(void)
{
	OnCANRMsg(0,15);	
}
void OnCANRMsgCAN1MB12(void)
{
	OnCANRMsg(1,12);	
}
void OnCANRMsgCAN1MB13(void)
{
	OnCANRMsg(1,13);	
}
void OnCANRMsgCAN1MB14(void)
{
	OnCANRMsg(1,14);	
}
void OnCANRMsgCAN1MB15(void)
{
	OnCANRMsg(1,15);	
}
void OnCANRMsgCAN2MB12(void)
{
	OnCANRMsg(2,12);	
}
void OnCANRMsgCAN2MB13(void)
{
	OnCANRMsg(2,13);	
}
void OnCANRMsgCAN2MB14(void)
{
	OnCANRMsg(2,14);	
}
void OnCANRMsgCAN2MB15(void)
{
	OnCANRMsg(2,15);	
}

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
					UINT_32 *canid,UINT_8 *datalen,UINT_8 *data)
{
	VUINT_8 can_data_len;
	VUINT_32 temp_cur_pri;
	if(CAN_MSG_num[channel]==0)
	{
		return ERROR_SFBUF_EMPTY;
	}
	temp_cur_pri = INTC.CPR.B.PRI;			//�����ٽ���
	INTC.CPR.B.PRI = 15;	
	
	CAN_MSG_num[channel]--;
	*ext 		= CAN_MSG_ext[channel][CAN_MSG_out[channel]];
	*dataframe  = CAN_MSG_dataframe[channel][CAN_MSG_out[channel]];
	*canid		= CAN_MSG_canid[channel][CAN_MSG_out[channel]];
	*datalen	= CAN_MSG_datalen[channel][CAN_MSG_out[channel]];
	can_data_len = *datalen;
	while(can_data_len>0)
	{
		data[can_data_len-1] = CAN_MSG_data[channel][CAN_MSG_out[channel]][can_data_len-1];
		can_data_len--;
	}
	CAN_MSG_out[channel] = (CAN_MSG_out[channel]+1)%CAN_MSGBUF_NUM;
	
	INTC.CPR.B.PRI = temp_cur_pri;			//�˳��ٽ���	
	return ERROR_OK;

	
}
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
					UINT_32 canid,UINT_8 datalen,UINT_8 *data)
{
	VUINT_8 can_data_len;
	VUINT_32 temp_cur_pri;
	
	
	if(CAN_TMSG_num[channel]>=CAN_TMSGBUF_NUM)
	{
		return ERROR_SFTBUF_FULL;
	}
	if(CAN_TMSG_num[channel]==0)
	{
											//���MB�գ����Է���
		if(ERROR_OK==SendCANMsgMB(channel,0,ext,dataframe,canid,datalen,data))
		{
			return ERROR_OK;
		}
	}										//���MB���ڷ��ͣ���д�뻺����
	temp_cur_pri = INTC.CPR.B.PRI;			//�����ٽ���
	INTC.CPR.B.PRI = 15;								
	CAN_TMSG_num[channel]++;
	CAN_TMSG_ext[channel][CAN_TMSG_in[channel]] = ext;
	CAN_TMSG_dataframe[channel][CAN_TMSG_in[channel]] = dataframe;
	CAN_TMSG_canid[channel][CAN_TMSG_in[channel]] = canid;
	CAN_TMSG_datalen[channel][CAN_TMSG_in[channel]] = datalen;
	can_data_len = datalen;
	while(can_data_len>0)
	{
		CAN_TMSG_data[channel][CAN_TMSG_in[channel]][can_data_len-1] = data[can_data_len-1];
		can_data_len--;
	}
	CAN_TMSG_in[channel] = (CAN_TMSG_in[channel]+1)%CAN_TMSGBUF_NUM;
	INTC.CPR.B.PRI = temp_cur_pri;			//�˳��ٽ���
	return ERROR_OK;

	
}


int CanTransmitPossible(void)
{
	if(CAN_TMSG_num[1]>=CAN_TMSGBUF_NUM)
	{
		return 0; // buffer ����
	}	
	
	else
		return 1;    //���Է���
}