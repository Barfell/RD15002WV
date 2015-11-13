#include  <includes.h>

/***************************************************************/
//������ƿ�
static  OS_TCB   AppTaskTASK1TCB;
static  OS_TCB   AppTaskTASK2TCB;
static  OS_TCB   AppTaskTASK3TCB;
//�����ջ
static  CPU_STK  AppTaskTASK1Stk[APP_CFG_TASK_TASK1_STK_SIZE];
static  CPU_STK  AppTaskTASK2Stk[APP_CFG_TASK_TASK2_STK_SIZE];
static  CPU_STK  AppTaskTASK3Stk[APP_CFG_TASK_TASK3_STK_SIZE];

/***************************************************************/
extern unsigned char UploadFlag;//�ϴ���IMS�ı�־
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//��ʼ����
void  AppTaskStart (void *p_arg)
{
	OS_ERR  err;
	CPU_Init();//��ʼ��cpu�������жϣ�����ʱ�����
	BSP_Tick_Init();//���õδ�ʱ��
/*------------------------------*/		 
	OSTaskCreate((OS_TCB       *)&AppTaskTASK1TCB,              
                 (CPU_CHAR     *)"task1",
                 (OS_TASK_PTR   )task1, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_TASK1_PRIO,
                 (CPU_STK      *)&AppTaskTASK1Stk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_TASK1_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_TASK1_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
/*------------------------------*/
	OSTaskCreate((OS_TCB       *)&AppTaskTASK2TCB,              
                 (CPU_CHAR     *)"task2",
                 (OS_TASK_PTR   )task2, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_TASK2_PRIO,
                 (CPU_STK      *)&AppTaskTASK2Stk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_TASK2_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_TASK2_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);  
/*-------------------------------*/
	OSTaskCreate((OS_TCB       *)&AppTaskTASK3TCB,              
                 (CPU_CHAR     *)"task3",
                 (OS_TASK_PTR   )task3, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_TASK3_PRIO,
                 (CPU_STK      *)&AppTaskTASK3Stk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_TASK3_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_TASK3_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
				 
	//����������������ɾ���Լ�
    OSTaskDel((OS_TCB*)0,&err);
}




















///////////////////////////////////////////////////////////////////////////////////////////////////////////
//�û�����1
void task1(void)
{
	OS_ERR      err;
	while(1)
	{
		OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_DLY,&err);
		if(UploadFlag == 0)
		{
			SW_12V(0);//��Դ
			SW_5V(0);//������Դ�����ڼ��ز�
			OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY,&err);
			ConfigPINToListen();
			PWR_EnterSTOPMode(PWR_Regulator_ON,PWR_STOPEntry_WFI);
			SetClock();//���ø���CPUʱ��
			bsp_io_init();//IO��
			SW_12V(1);//��Դ
			SW_5V(1);//������Դ�����ڼ��ز�
			OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY,&err);
			UsartConfig();//������������
		}
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//�û�����2 �����Ź���ι��
void task2(void)
{
	OS_ERR      err;
    while(1)
    {
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
    }
}

 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void task3(void)
{
	OS_ERR      err;
	unsigned char dev_ID[50]={'\0'};//����ID
	unsigned char str_buff[600];//��Ϣ����
	bsp_io_init();//IO��
	UsartConfig();//������������
	SW_12V(1);//��Դ
	SW_5V(1);//������Դ�����ڼ��ز�Z
	FreqModuleInit();//��Ƶ��ģ���ʼ��
	LTC2402Init();
	GetFreq(1);
	read_dev_id(dev_ID);//��ȡID

	while(1)
	{			
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_DLY,&err);
		if(UploadFlag == 1)//ȷ����Ҫ�ϴ�������
		{
			//printf("command confirmed!\r\n");
			FreqModuleInit();//��Ƶ��ģ���ʼ��
			LTC2402Init();
			GetFreq(1);
			read_dev_id(dev_ID);//��ȡID			
sprintf(str_buff,"SCT200T15003-Time: %s\r\n\
SCT200T15003-ID: %s\r\n\
SCT200T15003-Mode: xxx minutes Internal\r\n\
SCT200T15003-Current voltage = xx.xxxV\r\n\
SCT200T15003-Channel: 1  Temperature :%f\r\n\
SCT200T15003-Channel: 2  Temperature :%f\r\n\
SCT200T15003-Channel: 3  Temperature :%f\r\n\
SCT200T15003-Channel: 4  Temperature :%f\r\n\
SCT200T15003-Channel: 1  FREQUENCY:%f\r\n\
SCT200T15003-Channel: 2  FREQUENCY:%f\r\n\
SCT200T15003-Channel: 3  FREQUENCY:%f\r\n\
SCT200T15003-Channel: 4  FREQUENCY:%f\r\n",
get_time(),dev_ID,\
GetNTCTemperature(LTC2402_GetResistance(1)),\
GetNTCTemperature(LTC2402_GetResistance(2)),\
GetNTCTemperature(LTC2402_GetResistance(3)),\
GetNTCTemperature(LTC2402_GetResistance(4)),\
GetFreq(5),GetFreq(6),GetFreq(7),GetFreq(8));
			HandleDataPackage(str_buff);//���ͨ�����ڷ���
			

			UploadFlag = 0;
			SW_12V(0);//��Դ
			SW_5V(0);//������Դ�����ڼ��ز�
			OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_DLY,&err);
			ConfigPINToListen();
			PWR_EnterSTOPMode(PWR_Regulator_ON,PWR_STOPEntry_WFI);
			SetClock();//���ø���CPUʱ��
			bsp_io_init();//IO��
			SW_12V(1);//��Դ
			SW_5V(1);//������Դ�����ڼ��ز�
			//OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_DLY,&err);
			UsartConfig();//������������
			FreqModuleInit();//��Ƶ��ģ���ʼ��
			LTC2402Init();
			GetFreq(1);
			read_dev_id(dev_ID);//��ȡID
		}
	}
}



